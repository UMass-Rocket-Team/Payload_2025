"""
troubleshooting
- if module not found -- pip install pyserial (not "serial") then from the same terminal instance, python3 datetime_windows.py
- if port is wrong check device manager
- 
"""

import serial
import serial.tools.list_ports
import time

def find_port():
    for u in list(serial.tools.list_ports.comports()):
        if "USB Serial Device" in u.description:
            return u.name
    return -1

def main():
    port = find_port()
    s = serial.Serial(port=port, parity=serial.PARITY_EVEN, stopbits=serial.STOPBITS_ONE, timeout=1)
    s.flush()
    t = str(time.time()) + "\r"

    # send time to rpi
    s.write(t.encode())

    # read response from rpi
    mes = s.read_until().strip()
    print(mes.decode())

    # wait and check sync
    time.sleep(5)
    mes = s.read_until().strip()
    print(mes.decode())
    print(time.time())



if __name__ == "__main__":
    main()