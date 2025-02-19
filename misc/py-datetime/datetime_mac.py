import serial
import time


def main():
    s = serial.Serial(port="/dev/tty.usbmodem14101", parity=serial.PARITY_EVEN, stopbits=serial.STOPBITS_ONE, timeout=1)
    s.flush()
    t = str(time.time()) + "\r"

    s.write(t.encode())
    mes = s.read_until().strip()
    print(mes.decode())


if __name__ == "__main__":
    main()