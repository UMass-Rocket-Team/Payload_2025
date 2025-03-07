import pyttsx3
from ffmpy import FFmpeg
import os

engine = pyttsx3.init()

tts_phrases = [
    ("0", "0"),
    ("1", "1"),
    ("2", "2"),
    ("3", "3"),
    ("4", "4"),
    ("5", "5"),
    ("6", "6"),
    ("7", "7"),
    ("8", "8"),
    ("9", "9"),
    ("dot", "decimal"),
    ("kilo", "kilo"),
    ("charlie", "Charlie"),
    ("whiskey", "Whiskey"),
    ("callsign", "U M R T Payload"),
    ("apogee", "Maximum Altitude"),
    ("t_land", "Landing Time"),
    ("temp", "Temperature"),
    ("batt", "Battery Voltage"),
    ("stem", "STEM Not"),
    ("orient", "Orientation"),
    ("survival", "Survival Status"),
    ("land_vel", "Landing Velocity"),
    ("gforce", "Maximum G Force"),
    ("mps", "Meters per second"),
    ("celsius", "Celsius"),
    ("volts", "Volts"),
    ("g", "G"),
    ("meters", "Meters"),
    ("second", "Second"),
    ("minute", "Minute"),
    ("hour", "Hour"),
    ("stem_alive", "Survived"),
    ("stem_dead", "Not survived"),
    ("yaw", "Yaw"),
    ("pitch", "Pitch"),
    ("roll", "Roll"),
    ("degree", "Degrees"),
]

mp3_dir = "generated"
wav_dir = "compressed"

os.makedirs(mp3_dir, exist_ok=True)
os.makedirs(wav_dir, exist_ok=True)

engine.setProperty('volume',1.0)    # setting up volume level  between 0 and 1
engine.setProperty('rate', 120)     # setting up new voice rate

for filename, phrase in tts_phrases:
    engine.save_to_file(phrase, f"{mp3_dir}/{filename}.mp3")
    engine.runAndWait()
    FFmpeg(inputs={f"{mp3_dir}/{filename}.mp3": None}, outputs={f"{wav_dir}/{filename}.wav": '-ar 8000 -ac 1'}).run()

# for filename, _ in tts_phrases:
#     with wave.open(f"{wav_dir}/{filename}.wav", 'rb') as wav_file:
#         n_frames = wav_file.getnframes()
#         print(f"Channels: {wav_file.getnchannels()}")
#         print(f"Sample width: {wav_file.getsampwidth()}")
#         print(f"Frame rate: {wav_file.getframerate()}")
#         print(f"Number of frames: {n_frames}")
#         print(f"Compression type: {wav_file.getcomptype()}")
#         print(f"Compression name: {wav_file.getcompname()}")
#         print(f"n frames: {wav_file.getnframes()}")
        # audio_data = wav_file.readframes(n_frames)
        
        # with open(f"{wav_dir}/{filename}.bin", 'wb') as bin_file:
        #     bin_file.write(audio_data)