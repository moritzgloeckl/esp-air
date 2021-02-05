import serial
import codecs
from textwrap import wrap
import sys

ser = serial.Serial(
    port='/dev/cu.usbserial-141220',
    baudrate=9600,
    parity=serial.PARITY_NONE,
    stopbits=serial.STOPBITS_ONE,
    bytesize=serial.EIGHTBITS)

while True:
    line = wrap(ser.read(17).hex(), 2)

    check = 0
    for c in range(0, len(line)-1):
        check += int(line[c], 16)
    
    sys.stdout.write("Line read: " + " ".join(line) + "\n")
    sys.stdout.write("eCO2: " + str(int(line[2], 16) * 256 + int(line[3], 16)) + "\n")
    sys.stdout.write("eCH2O: " + str(int(line[4], 16) * 256 + int(line[5], 16)) + "\n")
    sys.stdout.write("TVOC: " + str(int(line[6], 16) * 256 + int(line[7], 16)) + "\n") 
    sys.stdout.write("PM2.5: " + str(int(line[8], 16) * 256 + int(line[9], 16)) + "\n")
    sys.stdout.write("PM10: " + str(int(line[10], 16) * 256 + int(line[11], 16)) + "\n")
    sys.stdout.write("Temp: " + str(int(line[12], 16) + int(line[13], 16) * 0.1) + "\n")
    sys.stdout.write("Hum: " + str(int(line[14], 16) + int(line[15], 16) * 0.1) + "\n")
    sys.stdout.write("Check: " + line[16] + " = " + hex(check)[3:] + "\n")
    sys.stdout.flush()
    
ser.close()
