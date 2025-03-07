import os
import sys
import numpy as np
from scipy.io import wavfile

def trim_zeros(data):
    # Determine mask: for multichannel, a sample is nonzero if any channel != 0.
    if data.ndim == 1:
        mask = data != 0
    else:
        mask = np.any(data != 0, axis=1)
    if not np.any(mask):
        return data  # All zeros, return unmodified
    first = np.argmax(mask)
    last = len(mask) - np.argmax(mask[::-1])
    return data[first:last]

def convert_to_8bit(data):
    # Assumes input is int16: range -32768 to 32767.
    # Convert to unsigned 8-bit: range 0-255.
    # First, shift to unsigned then scale.
    data_int32 = data.astype(np.int32) + 32768  # unsigned version in int32
    data_8 = np.round((data_int32 / 65535) * 255).astype(np.uint8)
    return data_8

def process_wav_file(filepath):
    samplerate, data = wavfile.read(filepath)
    # Trim leading and trailing zeros.
    trimmed = trim_zeros(data)
    # Convert bit depth to 8-bit. For multichannel, process each channel.
    if trimmed.ndim == 1:
        data_8 = convert_to_8bit(trimmed)
    else:
        # Process each channel separately.
        channels = []
        for i in range(trimmed.shape[1]):
            channels.append(convert_to_8bit(trimmed[:, i]))
        # Reassemble along the second axis.
        data_8 = np.stack(channels, axis=-1)
    # Save processed file with '_trimmed' appended to filename.
    dirname, filename = os.path.split(filepath)
    name, ext = os.path.splitext(filename)
    compressed_dir = os.path.join(dirname, "compressed")
    os.makedirs(compressed_dir, exist_ok=True)
    outpath = os.path.join(compressed_dir, f"{name}{ext}")
    wavfile.write(outpath, samplerate, data_8)
    print(f"Processed: {filepath} -> {outpath}")

def process_directory(directory):
    # Process all .wav files in the given directory.
    for file in os.listdir(directory):
        if file.lower().endswith('.wav'):
            process_wav_file(os.path.join(directory, file))

if __name__ == '__main__':
    if len(sys.argv) != 2:
        print("Usage: python audio_trim_reduce.py <directory>")
        sys.exit(1)
    target_dir = sys.argv[1]
    if not os.path.isdir(target_dir):
        print(f"Directory does not exist: {target_dir}")
        sys.exit(1)
    process_directory(target_dir)
