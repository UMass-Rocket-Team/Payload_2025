import serial
import time

# Replace 'COM3' with the appropriate COM port for your system
com_port = 'COM11'
baud_rate = 115200

try:
    ser = serial.Serial(com_port, baud_rate, timeout=1)
    if ser.is_open:
        print("Serial port opened successfully.")
        unix_time = int(time.time())
        ser.write(f"{unix_time}\n".encode('utf-8'))
        print(f"Unix Epoch Time: {unix_time} seconds")
        ser.close()
    else:
        print("Failed to open serial port.")
except serial.SerialException as e:
    print(f"Error: {e}")