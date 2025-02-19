#!/usr/bin/env python3
#
# Vendor:Product ID for Raspberry Pi Pico is 2E8A:0005
#
from serial.tools import list_ports
import serial, time

def find_port():
    for u in list(serial.tools.list_ports.comports()):
        if "USB Serial Device" in u.description:
            return u.name
    return -1

def main():
    port = find_port()
    with serial.Serial(port=port, parity=serial.PARITY_EVEN, stopbits=serial.STOPBITS_ONE, timeout=1) as s:
        for i in range(5):
            syncMSG = 'T'+str( int(time.time()) )
            s.write(bytes(syncMSG, "ascii"))
            print( "Time sync epoch USB MSG: "+syncMSG )
            time.sleep(2)
    

if __name__ == "__main__":
    main()