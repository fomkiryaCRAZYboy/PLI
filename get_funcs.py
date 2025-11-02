import os
import re
import json

def find_function_enums(project_path="."):
    """
    Find function return value enums in format F_<func_name>_RESULT
    """
    function_enums = {}
    
    for root, dirs, files in os.walk(project_path):
        # Ignore git folder
        if '.git' in dirs:
            dirs.remove('.git')
        
        for file in files:
            if file.endswith(('.c', '.h', '.cpp', '.hpp')):
                file_path = os.path.join(root, file)
                
                try:
                    with open(file_path, 'r', encoding='utf-8') as f:
                        content = f.read()
                        
                        # Remove comments
                        content = re.sub(r'//.*?\n', '\n', content)
                        content = re.sub(r'/\*.*?\*/', '', content, flags=re.DOTALL)
                        
                        # Find enum patterns: typedef enum f_<name>_result { ... } F_<NAME>_RESULT;
                        pattern = r'typedef\s+enum\s+(\w+)\s*\{([^}]+)\}\s*(\w+)\s*;'
                        matches = re.finditer(pattern, content)
                        
                        for match in matches:
                            enum_name = match.group(3)  # F_<NAME>_RESULT
                            
                            # Check if enum name matches our pattern
                            if re.match(r'F_(\w+)_RESULT', enum_name, re.IGNORECASE):
                                func_name_match = re.match(r'F_(\w+)_RESULT', enum_name, re.IGNORECASE)
                                function_name = func_name_match.group(1).lower()
                                
                                # Parse enum values
                                enum_body = match.group(2)
                                enum_values = []
                                
                                # Split by commas and clean up
                                value_lines = enum_body.split(',')
                                for i, line in enumerate(value_lines):
                                    line = line.strip()
                                    if line:
                                        # Take only the first identifier (before any = or comment)
                                        value_name = re.split(r'[=\s]', line)[0].strip()
                                        if value_name:
                                            enum_values.append(f"{i}:{value_name}")
                                
                                function_enums[function_name] = enum_values
                                
                except Exception as e:
                    print(f"Error reading {file_path}: {e}")
    
    return function_enums

def save_to_libconfig(function_enums, output_file="function_enums.conf"):
    """
    Save function enums in libconfig format
    """
    with open(output_file, 'w', encoding='utf-8') as f:
        for func_name, enum_values in function_enums.items():
            f.write(f"{func_name}\n")
            f.write("{\n")
            for value in enum_values:
                f.write(f"    {value},\n")
            f.write("}\n\n")
    print(f"Saved {len(function_enums)} function enums to {output_file}")

def save_to_json(function_enums, output_file="function_enums.json"):
    """
    Save function enums in JSON format
    """
    with open(output_file, 'w', encoding='utf-8') as f:
        json.dump(function_enums, f, ensure_ascii=False, indent=2)
    print(f"Saved {len(function_enums)} function enums to {output_file}")

def main():
    project_path = '.'
    
    print("Searching for function enums...")
    function_enums = find_function_enums(project_path)
    
    if not function_enums:
        print("No function enums found!")
        return
    
    # Print results
    print(f"\nFound {len(function_enums)} function enums:\n")
    for func_name, enum_values in function_enums.items():
        print(f"{func_name}:")
        for value in enum_values:
            print(f"  {value}")
        print()
    
    # Save in both formats
    save_to_libconfig(function_enums)
    save_to_json(function_enums)

if __name__ == "__main__":
    main()