# PLI — источники и ссылки

Список внешних источников, стандартов и методик, использованных при разработке интерпретатора PLI.

---

## Синтаксис и грамматика

### Extended Backus–Naur Form (EBNF)

Файл `syntax.ebnf` описывает грамматику PLI в нотации EBNF.

- **Wikipedia**: https://en.wikipedia.org/wiki/Extended_Backus-Naur_form
- **ISO/IEC 14977:1996** (стандарт EBNF): https://www.iso.org/standard/26153.html
- **Краткое описание ISO EBNF** (M. Kuhn): https://www.cl.cam.ac.uk/~mgk25/iso-ebnf.html

---

## Парсинг и разбор

### Рекурсивный спуск (Recursive Descent Parsing)

Парсер выражений (`src/parse_expr.c`) и парсер стейтментов (`src/parser.c`, `src/parser_api.c`) реализуют метод рекурсивного спуска: каждое правило грамматики — отдельная функция, приоритет операторов задаётся порядком вызовов.

- **Wikipedia**: https://en.wikipedia.org/wiki/Recursive_descent_parser

### Лексический анализ (Lexical Analysis / Tokenization)

Лексер (`src/lexer.c`) преобразует исходный текст в поток токенов.

- **Wikipedia**: https://en.wikipedia.org/wiki/Lexical_analysis

---

## Структуры данных

### Abstract Syntax Tree (AST)

Дерево разбора хранится в структурах из `include/parser.h` (expr_node_t, stmt_node_t, program_t и т.п.).

- **Wikipedia**: https://en.wikipedia.org/wiki/Abstract_syntax_tree

### Hash table (хэш-таблица)

Переменные хранятся в хэш-таблице с открытой адресацией и линейным пробированием (`src/var.c`).

- **Wikipedia (Hash table)**: https://en.wikipedia.org/wiki/Hash_table
- **Wikipedia (Open addressing)**: https://en.wikipedia.org/wiki/Open_addressing

---

## Хэш-функции

### FNV-1a (Fowler–Noll–Vo)

Для хэширования имён переменных используется FNV-1a (32-bit) в `src/var.c`.

- **Wikipedia**: https://en.wikipedia.org/wiki/Fowler%E2%80%93Noll%E2%80%93Vo_hash_function
- **Официальная страница** (Landon Curt Noll): http://www.isthe.com/chongo/tech/comp/fnv/index.html
- **IETF Draft** (FNV hash): https://www.ietf.org/archive/id/draft-eastlake-fnv-21.html

---

## Выполнение и интерпретация

### Tree-walking interpreter

Интерпретатор обходит AST и выполняет узлы по мере обхода (`src/eval_expr.c`, `src/executor.c`).

- **Wikipedia (Interpreter)**: https://en.wikipedia.org/wiki/Interpreter_(computing)
- **Crafting Interpreters** (R. Nystrom): https://craftinginterpreters.com/ — книга о реализации интерпретаторов

---

## Системное программирование (C)

### atexit

Регистрация `emergency_cleanup` и `print_errors` через `atexit()` (`src/err.c`) для освобождения памяти и вывода ошибок при выходе.

- **cppreference** (C17, 7.22.4.4): https://en.cppreference.com/w/c/program/atexit

---

## Управление памятью (`src/memctl/mem.c`)

Модуль реализует **tracking allocator**: цепочка `MEMORY_BLOCK` хранит все аллокации, `emergency_cleanup` освобождает всё при выходе (через `atexit`).

### Общие концепции

- **Wikipedia (Memory management)**: https://en.wikipedia.org/wiki/Memory_management
- **Wikipedia (Memory allocator)**: https://en.wikipedia.org/wiki/Memory_management#Dynamic_memory_allocation

### Tracking allocator (отслеживание аллокаций)

- **GNU libavl — Memory Manager**: https://benpfaff.org/avl/libavl.html/Memory-Manager.html — паттерн «обёртка над malloc» с цепочкой блоков
- **The Developer's Cry — tmalloc**: https://devcry.heiho.net/html/2023/20231001-tmalloc.html — tracking allocator с метаданными

### Arena / Bump allocator (альтернатива для фазовой жизни)

- **Arena Allocators** (William Fedele): https://www.williamfedele.com/blog/arena-allocators — объяснение, структура, API
- **An Arena/Bump Allocator in C** (Code Review): https://codereview.stackexchange.com/questions/291416/an-arena-bump-allocator-in-c — пример реализации
- **tsoding/arena** (stb-style, C): https://github.com/tsoding/arena — однострочная библиотека

### Custom allocator (собственный аллокатор)

- **Everything About Memory Allocators**: https://mohitmishra786.github.io/chessman/2024/11/24/Everything-About-Memory-Allocators-Write-a-simple-memory-allocator.html — от простого к сложному
- **Custom Memory Allocator** (GitHub): https://github.com/ShaheenJawadi/Custom-Memory-Allocator — блоки, free list, фрагментация

---

## Виртуальная машина и байткод (для будущей реализации на Rust)

Ресурсы, чтобы понять, что такое VM (как JVM), как она устроена, откуда берётся байткод и как он выполняется.

### Общие концепции

- **Wikipedia (Bytecode)**: https://en.wikipedia.org/wiki/Bytecode
- **Wikipedia (Stack machine)**: https://en.wikipedia.org/wiki/Stack_machine — стековая VM (JVM, clox)
- **Wikipedia (Register machine)**: https://en.wikipedia.org/wiki/Register_machine — регистровая VM (Lua)

### JVM (Java Virtual Machine)

- **Oracle JVM Specification**: https://docs.oracle.com/javase/specs/jvms/se23/html/ — полная спецификация
- **Compiling for the JVM** (глава 3): https://docs.oracle.com/javase/specs/jvms/se22/html/jvms-3.html — как Java-код превращается в байткод
- **Java Code to Bytecode** (James D. Bloom): https://blog.jamesdbloom.com/JavaCodeToByteCode_PartOne.html — пошаговый разбор
- **Bytecode basics** (InfoWorld): https://www.infoworld.com/article/2077233/bytecode-basics.html

### Crafting Interpreters (C/Java) — база для понимания

Часть II книги посвящена байткод-VM (clox):

- **Chunks of Bytecode**: https://craftinginterpreters.com/chunks-of-bytecode.html — структура байткода
- **A Virtual Machine**: https://craftinginterpreters.com/a-virtual-machine.html — главный цикл VM
- **Compiling Expressions**: https://craftinginterpreters.com/compiling-expressions.html — компиляция выражений в байткод
- **Calls and Functions**: https://craftinginterpreters.com/calls-and-functions.html — вызовы функций в VM

### Lua VM (регистровая архитектура)

- **The Implementation of Lua 5.0** (статья): https://www.jucs.org/jucs_11_7/the_implementation_of_lua/jucs_11_7_1159_1176_defigueiredo.html
- **lopcodes.h** (исходники Lua 5.0): https://www.lua.org/source/5.0/lopcodes.h.html — форматы инструкций

### Rust: реализация VM с нуля

- **Writing Interpreters in Rust: a Guide**: https://rust-hosted-langs.github.io/book/print.html  
  Главы: Bytecode Interpreter, VM Implementation. GitHub: https://github.com/rust-hosted-langs/book
- **Create Your Own Programming Language with Rust**: https://createlang.rs/ — VM и компилятор, глава calculator/vm
- **OrganicLox** (Crafting Interpreters в Rust): https://benjamingeer.name/en/post/organiclox/ — Lox на Rust с байткод-VM
- **crafting-interpreters-rs** (порт книги на Rust): https://github.com/tdp2110/crafting-interpreters-rs

### Порядок изучения (для VM на Rust)

1. Crafting Interpreters, часть II (clox) — понять Chunk, opcodes, главный цикл.
2. Writing Interpreters in Rust — перенести идеи на Rust.
3. Create Your Own Programming Language with Rust — практика от простого к сложному.
4. OrganicLox / crafting-interpreters-rs — примеры готовой реализации на Rust.

---

## Дополнительная литература

- **Compilers: Principles, Techniques, and Tools** (Aho, Lam, Sethi, Ullman) — «Dragon Book»
- **Modern Compiler Implementation in C** (A. Appel)
- **Crafting Interpreters** (R. Nystrom) — https://craftinginterpreters.com/
