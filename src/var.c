#include "var.h"
#include "mem.h"
#include <string.h>
#include <stdio.h>

hashmap* global_map = NULL;

/* ============================================================================
 * FNV-1a hash (32-bit)
 * Returns full uint32_t — modulo is applied in index calculation, not here.
 * ============================================================================ */

uint32_t var_hash_function(const char *varname)
{
    uint32_t hash = 2166136261u;
    int c;

    while ((c = *varname++))
    {
        hash ^= (uint8_t)c;
        hash *= 16777619u;
    }

    return hash;
}

/* ============================================================================
 * Internal helpers
 * ============================================================================ */

static inline bool is_empty(const variable_t *entry)
{
    return entry->name[0] == '\0';
}

static inline bool is_tombstone(const variable_t *entry)
{
    return entry->name[0] == '\x7f';
}

static inline bool is_available(const variable_t *entry)
{
    return is_empty(entry) || is_tombstone(entry);
}

/* Find slot for `name`: returns the entry where this name lives or should go */
static variable_t *find_entry(variable_t *entries, size_t capacity, const char *name)
{
    uint32_t index = var_hash_function(name) & (capacity - 1);
    variable_t *first_tombstone = NULL;

    for (;;)
    {
        variable_t *entry = &entries[index];

        if (is_empty(entry))
            return first_tombstone ? first_tombstone : entry;

        if (is_tombstone(entry))
        {
            if (!first_tombstone)
                first_tombstone = entry;
        }
        else if (strcmp(entry->name, name) == 0)
        {
            return entry;
        }

        index = (index + 1) & (capacity - 1);
    }
}

/* Grow the table when load factor is exceeded */
static bool hashmap_resize(hashmap *map, size_t new_cap)
{
    variable_t *new_entries = pli_alloc(new_cap * sizeof(variable_t));
    if (!new_entries)
        return false;

    memset(new_entries, 0, new_cap * sizeof(variable_t));

    /* Rehash all active entries into the new table */
    for (size_t i = 0; i < map->capacity; i++)
    {
        variable_t *src = &map->entries[i];
        if (is_available(src))
            continue;

        variable_t *dst = find_entry(new_entries, new_cap, src->name);
        *dst = *src;
    }

    pli_free(map->entries);
    map->entries  = new_entries;
    map->capacity = new_cap;

    return true;
}

/* ============================================================================
 * Public API
 * ============================================================================ */

hashmap *hashmap_new(void)
{
    hashmap *map = pli_alloc(sizeof(hashmap));
    if (!map)
        return NULL;

    map->entries = pli_alloc(HASHMAP_INIT_CAP * sizeof(variable_t));
    if (!map->entries)
    {
        pli_free(map);
        return NULL;
    }

    memset(map->entries, 0, HASHMAP_INIT_CAP * sizeof(variable_t));
    map->capacity = HASHMAP_INIT_CAP;
    map->count    = 0;

    return map;
}

void hashmap_free(hashmap *map)
{
    if (!map)
        return;
    pli_free(map->entries);
    pli_free(map);
}

/* Insert or update a variable. Returns false on allocation failure. */
bool hashmap_set(hashmap *map, const char *name, value_t val, bool is_const)
{
    /* Grow if load factor exceeded */
    if (map->count + 1 > (size_t)(map->capacity * HASHMAP_LOAD_FACTOR))
    {
        if (!hashmap_resize(map, map->capacity * 2))
            return false;
    }

    variable_t *entry = find_entry(map->entries, map->capacity, name);

    bool is_new = is_available(entry);

    strncpy(entry->name, name, MAX_VAR_SIZE - 1);
    entry->name[MAX_VAR_SIZE - 1] = '\0';
    entry->value    = val;
    entry->is_const = is_const;

    if (is_new)
        map->count++;

    return true;
}

/* Find variable by name. Returns NULL if not found. */
variable_t *hashmap_lookup(hashmap *map, const char *name)
{
    if (map->count == 0)
        return NULL;

    variable_t *entry = find_entry(map->entries, map->capacity, name);

    if (is_available(entry))
        return NULL;

    return entry;
}

/* Delete variable by name. Returns true if found and deleted. */
bool hashmap_delete(hashmap *map, const char *name)
{
    if (map->count == 0)
        return false;

    variable_t *entry = find_entry(map->entries, map->capacity, name);

    if (is_available(entry))
        return false;

    /* Place tombstone: \x7f marks a deleted slot */
    entry->name[0] = '\x7f';
    entry->name[1] = '\0';
    map->count--;

    return true;
}
