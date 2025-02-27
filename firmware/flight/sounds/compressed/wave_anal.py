import wave
import csv
import os

def wav_to_csv(wav_file):
    # Open the wav file
    with wave.open(wav_file, 'rb') as wav:
        # Get the number of channels
        n_channels = wav.getnchannels()
        print(wav.getsampwidth())
        # Get the number of frames
        n_frames = wav.getnframes()
        # Read the frames
        frames = wav.readframes(n_frames)
        
        # Convert frames to a list of samples
        samples = []
        last = 0
        lsb: bool = False
        for frame in frames:
            if not lsb:
                last = frame
            else:
                fullFrame = (last * 256) + frame
                if fullFrame > 32767:
                    fullFrame = fullFrame - 65536
                samples.append(fullFrame)
                
            lsb = not lsb
        
        # Get the base name of the wav file
        base_name = os.path.splitext(wav_file)[0]
        # Create the csv file name
        csv_file = base_name + '.csv'
        
        # Write the samples to the csv file
        with open(csv_file, 'w', newline='') as csvfile:
            csv_writer = csv.writer(csvfile)
            for i in range(0, len(samples)):
                csv_writer.writerow([samples[i]])
            
                # csv_writer.writerow([(samples[i*2] * 256) + samples[i*2 + 1]])

        # Plot the samples
        # import matplotlib.pyplot as plt

        # plt.figure(figsize=(10, 4))
        # plt.plot(samples, label='Waveform')
        # plt.title('Waveform of ' + wav_file)
        # plt.xlabel('Sample Index')
        # plt.ylabel('Amplitude')
        # plt.legend()
        # plt.show()

# Example usage
# wav_to_csv('example.wav')

wavs = [

]

for i in range(10):
    wavs.append(f"{i}.wav")

for fname in wavs:
    wav_to_csv(fname)
