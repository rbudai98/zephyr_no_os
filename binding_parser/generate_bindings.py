import os
import re
import yaml

valid_suffixes = ("_init_param", "_ini_param", "_ip", "_init_par")

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
                        h_files_content[file_path] = f.read()
                except Exception as e:
                    print(f"Failed to read {file_path}: {e}")
    
    return h_files_content
def generate_binding_file(struct_name, struct_body):
    """
    Generates a Zephyr binding file for the given structure.
    :param struct_name: Name of the structure.
    :param struct_body: Body of the structure.
    """
    binding_content = {
        "title": f"{struct_name} Binding",
        "description": f"Generated binding for {struct_name}.",
        "properties": {}
    }

    # Parse the structure body to extract fields
    field_pattern = re.compile(r'(\w+)\s+(\w+);')
    fields = field_pattern.findall(struct_body)

    def resolve_enum_type(field_type):
        """
        Resolves the type of a field if it is an enum.
        :param field_type: The type of the field.
        :return: The resolved type or None if not resolvable.
        """
        enum_pattern = re.compile(r'enum\s+' + re.escape(field_type) + r'\s*{([^}]*)};', re.DOTALL)
        for file_content in h_files.values():
            match = enum_pattern.search(file_content)
            if match:
                enum_values = match.group(1).split(',')
                enum_values = [v.strip().split('=')[0].strip() for v in enum_values]
                return enum_values
        return None

    for field_type, field_name in fields:
        enum_values = resolve_enum_type(field_type)
        if enum_values:
            binding_content["properties"][field_name] = {
                "type": "string",
                "enum": enum_values,
                "description": f"Field {field_name} of enum type {field_type}."
            }
        else:
            binding_content["properties"][field_name] = {
                "type": "string" if field_type == "char" else "integer",
                "description": f"Field {field_name} of type {field_type}."
            }

    # Generate the YAML file
    binding_file_name = f"dts/bindings/{struct_name.lower()}_binding.yaml"
    binding_file_path = os.path.join(os.getcwd(), binding_file_name)

    try:
        with open(binding_file_path, 'w', encoding='utf-8') as f:
            yaml.dump(binding_content, f, default_flow_style=False)
        print(f"Binding file generated: {binding_file_path}")
    except ImportError:
        print("PyYAML is not installed. Please install it to generate the binding file.")
    except Exception as e:
        print(f"Failed to generate binding file: {e}")
    """
    Generates a Zephyr binding file for the given structure.
    :param struct_name: Name of the structure.
    :param struct_body: Body of the structure.
    """
    binding_content = {
        "title": f"{struct_name} Binding",
        "description": f"Generated binding for {struct_name}.",
        "properties": {}
    }

    # Parse the structure body to extract fields
    field_pattern = re.compile(r'(\w+)\s+(\w+);')
    fields = field_pattern.findall(struct_body)

    for field_type, field_name in fields:
        binding_content["properties"][field_name] = {
            "type": "string" if field_type == "char" else "integer",
            "description": f"Field {field_name} of type {field_type}."
        }

    # Generate the YAML file
    binding_file_name = f"dts/bindings/{struct_name.lower()}_binding.yaml"
    binding_file_path = os.path.join(os.getcwd(), binding_file_name)

    try:
        with open(binding_file_path, 'w', encoding='utf-8') as f:
            yaml.dump(binding_content, f, default_flow_style=False)
        print(f"Binding file generated: {binding_file_path}")
    except ImportError:
        print("PyYAML is not installed. Please install it to generate the binding file.")
    except Exception as e:
        print(f"Failed to generate binding file: {e}")


def extract_referenced_structs(struct_name, struct_body):
    """
    Extracts referenced structs and generates a binding file.
    :param struct_name: Name of the structure.
    :param struct_body: Body of the structure.
    """
    print(f"Processing struct {struct_name}...")
    generate_binding_file(struct_name, struct_body)

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
                extract_referenced_structs(struct_name, struct_body)
                

if __name__ == "__main__":
#     directory = input("Enter the directory to search for .h files: ")
    directory = "/home/rbudai/zephyrproject/modules/lib/no-os/drivers"
    h_files = load_h_files(directory)
    
    print(f"Loaded {len(h_files)} .h files.")
    find_struct_definitions(h_files)