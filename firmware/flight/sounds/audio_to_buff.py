import os
import sys

def generate_array_data(file_path):
    with open(file_path, "rb") as f:
        content = f.read()
    # Convert bytes to comma-separated hex values
    arr = ", ".join("0x{:02X}".format(b) for b in content)
    return arr, len(content)

def main(directory):
    header_lines = [
        "// Auto-generated header file with wave file arrays",
        "#ifndef AUDIO_FILES_H",
        "#define AUDIO_FILES_H",
        ""
    ]
    total_size = 0  # Initialize total buffer size
    for file in os.listdir(directory):
        if file.lower().endswith(".wav"):
            base_name = os.path.splitext(file)[0]
            identifier = base_name.replace(" ", "_")
            file_path = os.path.join(directory, file)
            arr, length = generate_array_data(file_path)
            total_size += length  # Accumulate size of current buffer
            header_lines.append(f"const unsigned char f_{identifier}_wav[] = {{")
            header_lines.append(f"    {arr}")
            header_lines.append("};")
            header_lines.append(f"const unsigned int f_{identifier}_wav_len = {length};")
            header_lines.append("")
    header_lines.append("#endif // AUDIO_FILES_H")
    output_file = os.path.join(directory, "audio_files.h")
    with open(output_file, "w") as f:
        f.write("\n".join(header_lines))
    print(f"Header file generated: {output_file}")
    print(f"Total size of all buffers: {total_size} bytes")

if __name__ == '__main__':
    if len(sys.argv) != 2:
        print("Usage: python audio_to_buff.py <directory>")
        sys.exit(1)
    target_directory = sys.argv[1]
    if not os.path.isdir(target_directory):
        print(f"Directory does not exist: {target_directory}")
        sys.exit(1)
    main(target_directory)
