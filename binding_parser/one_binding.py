import os
import re

valid_suffixes = ("_init_param", "_ini_param", "_ip", "_init_par")


def format_struct_body(struct_body):
    """
    Formats the struct body for consistent parsing.
    :param struct_body: The raw content of the struct body.
    :return: A cleaned and formatted struct body.
    """
    # Normalize multiline comments into single-line comments
    struct_body = re.sub(r'/\*\*(.*?)\*/', lambda m: f"/* {m.group(1).strip()} */", struct_body, flags=re.DOTALL)
    
    # Ensure each variable declaration starts on a new line
    struct_body = re.sub(r';\s*', ';\n', struct_body)
    
    # Remove excess whitespace and normalize indentation
    struct_body = "\n".join(line.strip() for line in struct_body.splitlines() if line.strip())
    
    return struct_body

def load_h_files(directory):
    """
    Recursively loads all .h files from the given directory, excluding specific folders.
    :param directory: The root directory to search in.
    :return: A dictionary mapping file paths to their contents.
    """
    h_files_content = {}
    excluded_dirs = {"api", "platform"}  # Folders to exclude
    
    for root, dirs, files in os.walk(directory):
        # Modify dirs in-place to exclude specific folders
        dirs[:] = [d for d in dirs if d not in excluded_dirs]
        
        for file in files:
            if file.endswith(".h"):
                file_path = os.path.join(root, file)
                try:
                    with open(file_path, 'r', encoding='utf-8') as f:
                        h_files_content[file_path] = format_struct_body(f.read())
                except Exception as e:
                    print(f"Failed to read {file_path}: {e}")
    
    return h_files_content

def generate_binding_header(yaml_file_path, struct_name, includes):
     
     with open(yaml_file_path, 'w', encoding='utf-8') as yaml_file:
        yaml_file.write(f"# Copyright (c) 2025 Analog Devices Inc.\n")
        yaml_file.write(f"\n")
        yaml_file.write(f"description: No-os binding for {struct_name} device\n")
        yaml_file.write(f"\n")
        yaml_file.write(f"compatible: \"adi,no-os-{struct_name}\"\n")
        yaml_file.write(f"\n")
        if includes:
            yaml_file.write(f"include: [{', '.join(includes)}]\n")  # Format as a YAML list with double quotes
        yaml_file.write(f"\n")
        yaml_file.write(f"properties:")
        yaml_file.write(f"\n")

def append_variable_to_yaml(yaml_file_path, variable, available_elements=None):
    """
    Appends a variable's details to an existing YAML file.
    :param yaml_file_path: Path to the YAML file.
    :param variable: A list containing [comment, type, name] of the variable.
    :param available_elements: Optional list of available elements to include in the YAML.
    """
    with open(yaml_file_path, 'a', encoding='utf-8') as yaml_file:
        yaml_file.write(f"  {variable[2]}:\n")
        # yaml_file.write(f"    description: \"{variable[0]}\"\n")
        yaml_file.write(f"    type: {variable[1]}\n")
        if available_elements:
            yaml_file.write(f"    enum:\n")
            for element in available_elements:
                yaml_file.write(f"      - {element}\n")


def resolve_enum_type(var_type, content, resolved_enums=None):
    """
    Recursively resolves enum types and collects their elements.
    :param var_type: The enum type to resolve.
    :param content: The content of the file to search for the enum definition.
    :param resolved_enums: A dictionary to store resolved enums and their elements.
    :return: A tuple containing the resolved type and available elements.
    """
    if resolved_enums is None:
        resolved_enums = {}

    if var_type in resolved_enums:
        return "int", resolved_enums[var_type]  # Replace enum type with int

    enum_pattern = re.compile(rf'enum\s+{re.escape(var_type.split()[-1])}\s*{{(.*?)}};', re.DOTALL)
    enum_match = enum_pattern.search(content)
    if enum_match:
        enum_body = enum_match.group(1)
        # Remove comments from the enum body
        enum_body = re.sub(r'/\*.*?\*/|//.*', '', enum_body, flags=re.DOTALL)
        available_elements = [elem.strip().split('=')[0] for elem in enum_body.split(',') if elem.strip()]
        resolved_enums[var_type] = available_elements
        return "int", available_elements  # Replace enum type with int

    return var_type, None  # Return the original type if not resolvable


def extract_referenced_structs(struct_name, struct_body, content):
    """
    Looks for specific struct references inside a struct body and prints their names.
    Creates a YAML file for each function call.
    :param struct_name: The name of the struct.
    :param struct_body: The content of the struct body.
    """
    # Remove valid_suffixes from struct_name
    for suffix in valid_suffixes:
        if struct_name.endswith(suffix):
            struct_name = struct_name[: -len(suffix)]
            break

    # Create directory
    yaml_dir = "./dts/bindings"
    os.makedirs(yaml_dir, exist_ok=True)
    yaml_file_path = os.path.join(yaml_dir, f"no-os-{struct_name}.yaml")

    struct_references = {
        "struct no_os_spi_init_param": "spi-device.yaml",
        "struct no_os_i2c_init_param": "i2c-device.yaml",
        "struct no_os_gpio_init_param": "gpio-controller.yaml",
    }

    include = []
    for struct_ref, yaml_file in struct_references.items():
        if struct_ref in struct_body:
            include.append(f"{yaml_file}")

    
    generate_binding_header(yaml_file_path, struct_name, include)

    variable_pattern = re.compile(
       r'(?:/\*\*\s*([\s\S]*?)\s*\*/|/\*\s*([\s\S]*?)\s*\*/|//\s*(.*?))?\s*([\w\s\*\[\]]+)\s+([\w\[\]]+);\s*(?:/\*\s*(.*?)\s*\*/|//\s*(.*))?',
        re.DOTALL
    )

    variables = []
    resolved_enums = {}

    for match in variable_pattern.findall(struct_body):
        comment_above = (match[0] or match[1] or match[2] or "").strip()
        var_type = match[3].strip()
        var_name = match[4].strip()
        available_elements = None

        # Skip tuples where var_type contains "struct"
        if "struct" in var_type:
            continue

        if var_type.startswith("enum"):
            var_type, available_elements = resolve_enum_type(var_type, content, resolved_enums)

        append_variable_to_yaml(yaml_file_path, (comment_above, var_type, var_name), available_elements)


def find_struct_definitions(h_files):
    """
    Searches for structure definitions ending with _init_param, _ini_param, or _ip.
    :param h_files: Dictionary of file paths and contents.
    """
    struct_pattern = re.compile(r'struct\s+(\w+)\s*{([^}]*)};', re.DOTALL)
    

    for file_path, content in h_files.items():
        print(f"\n>>>>{file_path}:")
        matches = struct_pattern.findall(content)
        for match in matches:
            struct_name, struct_body = match
            if struct_name.endswith(valid_suffixes):
                # print(f"Found struct {struct_name} in {file_path}:")
                # print(f"struct {struct_name} {{ {struct_body} }};\n")
                # format struct body

                struct_body = format_struct_body(struct_body)
                print(struct_body)
                extract_referenced_structs(struct_name, struct_body, content)
                

if __name__ == "__main__":
#     directory = input("Enter the directory to search for .h files: ")
    directory = "/home/rbudai/zephyrproject/modules/lib/no-os/drivers"
    h_files = load_h_files(directory)
    
    print(f"Loaded {len(h_files)} .h files.")
    find_struct_definitions(h_files)