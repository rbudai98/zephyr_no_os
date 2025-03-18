import os
import re

def reformat_comments(directory):
        # Iterate through all files in the directory
        for root, _, files in os.walk(directory):
                for file in files:
                        if file.endswith(".h"):
                                file_path = os.path.join(root, file)
                                process_file(file_path)

def process_file(file_path):
        with open(file_path, 'r', encoding='utf-8', errors='replace') as f:
                lines = f.readlines()

        reformatted_lines = []
        inline_comment_pattern = re.compile(r'(.*?)(\s*//\s*(.*))$')  # Match inline comments
        block_comment_pattern = re.compile(r'/\*(.*?)\*/', re.DOTALL)  # Match block comments

        for line in lines:
                line = line.replace('\t', '    ')  # Replace tabs with spaces
                line = re.sub(r' +', ' ', line)  # Replace multiple spaces with a single space

                # Replace block comments /* */ with /** */
                line = block_comment_pattern.sub(lambda m: f"/**{m.group(1)}*/", line)

                # Process inline comments
                match = inline_comment_pattern.match(line)
                if match:
                        code_part = match.group(1).strip()
                        comment_text = match.group(3).strip()
                        if code_part:  # If there's code before the comment
                                reformatted_lines.append(f"/** {comment_text} */\n")
                                reformatted_lines.append(f"{code_part}\n")
                        else:  # If it's just a comment
                                reformatted_lines.append(f"/** {comment_text} */\n")
                else:
                        reformatted_lines.append(line)

        with open(file_path, 'w') as f:
                f.writelines(reformatted_lines)

if __name__ == "__main__":
        directory = "/home/rbudai/zephyrproject/modules/lib/no-os/drivers/"
        reformat_comments(directory)