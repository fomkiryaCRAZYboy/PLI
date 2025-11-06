import os
import re
import json

def remove_comments(content):
    """Remove C-style comments from code"""
    # Remove single-line comments
    content = re.sub(r'//.*?\n', '\n', content)
    # Remove multi-line comments
    content = re.sub(r'/\*.*?\*/', '', content, flags=re.DOTALL)
    return content

def find_typedefs(content):
    """Find all typedef declarations to understand custom types"""
    typedefs = {}
    
    # Pattern for typedef: typedef <type> <name>;
    # or typedef struct/union/enum { ... } <name>;
    patterns = [
        # typedef existing_type new_name;
        r'typedef\s+([a-zA-Z_][a-zA-Z0-9_]*\s*\*?)\s+([a-zA-Z_][a-zA-Z0-9_]*)\s*;',
        # typedef struct/union/enum { ... } name;
        r'typedef\s+(?:struct|union|enum)\s+(?:\w+\s*)?\{[^}]*\}\s*([a-zA-Z_][a-zA-Z0-9_]*)\s*;',
    ]
    
    for pattern in patterns:
        matches = re.finditer(pattern, content)
        for match in matches:
            if len(match.groups()) == 2:
                typedefs[match.group(2)] = match.group(1).strip()
            else:
                typedefs[match.group(1)] = 'custom_type'
    
    return typedefs

def parse_function_signature(line, typedefs):
    """Parse a function signature and extract return type and name"""
    # Pattern to match function declarations/definitions
    # Handles: return_type function_name(params);
    # or: return_type function_name(params) {
    # Supports pointers, multiple words in return type
    
    # Remove leading/trailing whitespace
    line = line.strip()
    
    # Skip if empty or just a semicolon
    if not line or line == ';':
        return None
    
    # Skip lines that are clearly not function declarations:
    # - Lines starting with operators or keywords that indicate statements
    if re.match(r'^\s*(if|for|while|switch|return|break|continue|goto|else|case|default)\s*\(', line):
        return None
    
    # Skip lines that look like variable declarations (have = before ;)
    if '=' in line and ';' in line and line.index('=') < line.index(';'):
        # But allow function pointers: int (*func)(params) = ...;
        if not re.search(r'\(\s*\*', line):
            return None
    
    # Skip lines that are function calls (have operators before function name)
    # This is a heuristic: if there's a non-whitespace char before what looks like a function call
    # But we want to catch: "void func(...)" at start of line
    
    # Pattern for identifier: starts with letter or underscore, followed by letters, digits, underscores
    identifier = r'[a-zA-Z_][a-zA-Z0-9_]*'
    
    # Return type can be:
    # - Basic types: void, int, char, etc.
    # - With qualifiers: const char*
    # - Typedef types: TOKEN_STREAM, f_result, etc.
    # - With pointers: char*, void*, TOKEN_STREAM*, etc.
    # - Multiple words: const char*, unsigned int, etc.
    
    # Match return type: identifier(s) optionally followed by *
    # Handle: "void", "int", "char*", "TOKEN_STREAM*", "const char*", "f_result", etc.
    return_type = r'(?:const\s+)?' + identifier + r'(?:\s+\*|\s+' + identifier + r')*(?:\s*\*)?'
    
    # Pattern: return_type function_name(params) [;{]
    # Must start at beginning of line (after optional whitespace)
    pattern = r'^\s*(' + return_type + r')\s+(' + identifier + r')\s*\([^)]*\)\s*[;{]'
    
    match = re.match(pattern, line)
    
    if match:
        return_type = match.group(1).strip()
        function_name = match.group(2).strip()
        
        # Clean up return type (remove extra spaces)
        return_type = re.sub(r'\s+', ' ', return_type)
        return_type = return_type.strip()
        
        # Check if it's a typedef
        base_type = return_type.replace('*', '').strip()
        if base_type in typedefs:
            return_type = f"{return_type} (typedef)"
        
        return {
            'name': function_name,
            'return_type': return_type
        }
    
    return None

def find_functions_in_file(file_path, typedefs):
    """Find all functions in a C source or header file"""
    functions = []
    
    try:
        with open(file_path, 'r', encoding='utf-8') as f:
            content = f.read()
        
        # Remove comments
        content = remove_comments(content)
        
        # Split into lines
        lines = content.split('\n')
        
        # Track brace depth to identify function definitions vs calls
        brace_depth = 0
        in_function = False
        
        # Track multi-line declarations
        current_declaration = ""
        declaration_start_line = 0
        
        for line_num, line in enumerate(lines, 1):
            original_line = line
            stripped = line.strip()
            
            # Skip preprocessor directives
            if stripped.startswith('#'):
                continue
            
            # Track braces to know if we're inside a function body
            # But check BEFORE updating brace_depth, so we can catch function definitions
            was_in_function = in_function
            
            # Update brace depth
            brace_depth += stripped.count('{') - stripped.count('}')
            if brace_depth > 0:
                in_function = True
            elif brace_depth == 0 and in_function:
                in_function = False
            
            # Skip lines that are clearly inside function bodies
            # For .h files, we want declarations (ending with ;)
            # For .c files, we want definitions (ending with {), but skip body content
            if was_in_function and file_path.endswith('.c'):
                # Already inside function body, skip
                continue
            
            # Build multi-line declaration
            if not stripped:
                if current_declaration:
                    current_declaration += " "
                continue
            
            if current_declaration:
                current_declaration += " " + stripped
            else:
                current_declaration = stripped
                declaration_start_line = line_num
            
            # Check if declaration is complete (ends with ; or {)
            if ';' in current_declaration or '{' in current_declaration:
                # Try to parse as function
                func_info = parse_function_signature(current_declaration, typedefs)
                if func_info:
                    func_info['file'] = file_path
                    func_info['line'] = declaration_start_line
                    functions.append(func_info)
                
                current_declaration = ""
                declaration_start_line = 0
    
    except Exception as e:
        print(f"Error reading {file_path}: {e}")
    
    return functions

def find_all_functions(project_path="."):
    """Find all functions in C source and header files"""
    all_functions = {}
    all_typedefs = {}
    
    # First pass: collect all typedefs
    for root, dirs, files in os.walk(project_path):
        # Ignore git folder and other irrelevant dirs
        if '.git' in dirs:
            dirs.remove('.git')
        if 'bin' in dirs:
            dirs.remove('bin')
        
        for file in files:
            if file.endswith(('.c', '.h')):
                file_path = os.path.join(root, file)
                try:
                    with open(file_path, 'r', encoding='utf-8') as f:
                        content = remove_comments(f.read())
                        typedefs = find_typedefs(content)
                        all_typedefs.update(typedefs)
                except:
                    pass
    
    # Second pass: find all functions
    for root, dirs, files in os.walk(project_path):
        if '.git' in dirs:
            dirs.remove('.git')
        if 'bin' in dirs:
            dirs.remove('bin')
        
        for file in files:
            if file.endswith(('.c', '.h')):
                file_path = os.path.join(root, file)
                functions = find_functions_in_file(file_path, all_typedefs)
                
                for func in functions:
                    func_name = func['name']
                    # If function already found, prefer definition over declaration
                    if func_name not in all_functions or '.c' in func['file']:
                        all_functions[func_name] = func
    
    return all_functions

def save_to_json(functions, output_file="functions.json"):
    """Save functions in JSON format"""
    with open(output_file, 'w', encoding='utf-8') as f:
        json.dump(functions, f, ensure_ascii=False, indent=2)
    print(f"Saved {len(functions)} functions to {output_file}")

def main():
    project_path = '.'
    
    print("Searching for functions...")
    functions = find_all_functions(project_path)
    
    if not functions:
        print("No functions found!")
        return
    
    # Print results
    print(f"\nFound {len(functions)} functions:\n")
    for func_name, func_info in sorted(functions.items()):
        print(f"{func_name}:")
        print(f"  Return type: {func_info['return_type']}")
        print(f"  File: {func_info['file']}")
        print()
    
    # Save to JSON
    save_to_json(functions)

if __name__ == "__main__":
    main()
