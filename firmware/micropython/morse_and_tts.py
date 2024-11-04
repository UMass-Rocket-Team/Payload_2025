from machine import Pin, PWM
import syn6988
import time
import random

# Morse code dictionary
MORSE_CODE = {
    'A': '.-',    'B': '-...',  'C': '-.-.',  'D': '-..',   'E': '.',     'F': '..-.',
    'G': '--.',   'H': '....',  'I': '..',    'J': '.---',  'K': '-.-',   'L': '.-..',
    'M': '--',    'N': '-.',    'O': '---',   'P': '.--.',  'Q': '--.-',  'R': '.-.',
    'S': '...',   'T': '-',     'U': '..-',   'V': '...-',  'W': '.--',   'X': '-..-',
    'Y': '-.--',  'Z': '--..',  '1': '.----', '2': '..---', '3': '...--', '4': '....-',
    '5': '.....', '6': '-....', '7': '--...', '8': '---..', '9': '----.', '0': '-----',
    ' ': ' '  # Space between words
}

### setup device
ser = machine.UART(
    0, baudrate=9600, bits=8, parity=None, stop=1
)  # tx=Pin(0), rx=Pin(1)

busyPin = machine.Pin(2, machine.Pin.IN, machine.Pin.PULL_UP)
s = syn6988.SYN6988(ser, busyPin)

# Setup PWM on a specific pin (e.g., GP15)
pwm_pin = PWM(Pin(3))
pwm_pin.freq(1000)  # Frequency for Morse tone, e.g., 1kHz

def speak_and_spell(r):
    # print then speak string r
    print(r)
    s.speak(r)

def fabricate_data():
    batteryV = random.randint(6,8)
    batteryv = random.randint(0,9)
    compass = random.randint(0,360)
    angle = random.randint(0,360)
    impact = random.randint(100,300)
    speak_and_spell("[g2]This is K C 1 V D N, doing a test transmission using a T T S.")
    speak_and_spell(f"Battery voltage is {batteryV} point {batteryv} volts")
    speak_and_spell(f"Compass heading is {compass} degrees")
    speak_and_spell(f"Detected orientation angle is {angle} degrees")
    speak_and_spell(f"Impact velocity is {impact} feet per second")
    play_text_as_morse(f"BV {batteryV}.{batteryv}")
    play_text_as_morse(f"C {compass}")
    play_text_as_morse(f"A {angle}")
    play_text_as_morse(f"IV {impact}")


# Timing rules for Morse code (in seconds)
DOT_TIME = 0.2  # Duration of one dot
DASH_TIME = DOT_TIME * 3  # Duration of one dash
LETTER_SPACE = DOT_TIME * 3  # Space between letters
WORD_SPACE = DOT_TIME * 7  # Space between words

def play_dot():
    pwm_pin.duty_u16(32768)  # 50% duty cycle to play tone
    time.sleep(DOT_TIME)
    pwm_pin.duty_u16(0)  # Turn off
    time.sleep(DOT_TIME)  # Pause between elements in a letter

def play_dash():
    pwm_pin.duty_u16(32768)
    time.sleep(DASH_TIME)
    pwm_pin.duty_u16(0)
    time.sleep(DOT_TIME)

def play_morse_for_letter(letter):
    morse_code = MORSE_CODE.get(letter.upper(), '')
    for symbol in morse_code:
        if symbol == '.':
            play_dot()
        elif symbol == '-':
            play_dash()

def play_text_as_morse(text):
    for char in text:
        if char == ' ':
            time.sleep(WORD_SPACE - DOT_TIME)  # Long space between words
        else:
            play_morse_for_letter(char)
            time.sleep(LETTER_SPACE - DOT_TIME)  # Space between letters





fabricate_data()
# while True:
#     fabricate_data()
#     time.sleep(5)


