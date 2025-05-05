import serial
import csv
import time

# Configure the serial port
serial_port = 'COM19'  # Change this to your serial port
baud_rate = 115200  # Change this to your baud rate

# Open the serial port
ser = serial.Serial(serial_port, baud_rate)

with open('timing_log.csv', mode='w', newline='') as file:
    writer = csv.writer(file)
    writer.writerow(['Received Time (s)', 'Decoded Time (us)'])

# Open the CSV file for writing
logcnt = 0
while True:
    # Read a line from the serial port
    line = ser.readline().decode('utf-8').strip()
    
    # Get the current time
    received_time = time.time_ns()
    
    # Convert the line to an integer (assuming the line contains the time in microseconds)
    try:
        decoded_time = int(line)
    except ValueError:
        print(f"Invalid data received: {line}")
        continue

    with open('timing_log.csv', mode='a', newline='') as file:
        writer = csv.writer(file)
        # writer.writerow(['Received Time (s)', 'Decoded Time (us)'])

        # Log the received time and decoded time to the CSV file
        writer.writerow([received_time, decoded_time])

        logcnt += 1

        if logcnt % 100 == 0:        
            print(f"Logged: Received Time = {received_time}, Decoded Time = {decoded_time}")
