#!/usr/bin/env python

import serial
import struct
import time

def serial_list():
	valid_ports=[]

	try:
		import serial.tools.list_ports

		for port in serial.tools.list_ports.comports():
			if port[2]!='n/a':
				valid_ports.append(port[0])

	except:
		None

	return valid_ports

def millis():
	return int(round(time.time()*1000))

def bytes(data):
	width=value.bit_length()
	width+=(8-((width%8) or 8))//4
	return unhexlify(('%%0%dx'%width)%value)[::-1]

def packetize(data):
	packet=""
	packet+=chr(0x5f)
	packet+=struct.pack('<H',len(data))
	packet+=data
	return packet

if __name__=="__main__":
	ser=serial.Serial()
	timer=millis()+2000
	pos=40;

	while True:
		try:
			print("connecting...")
			ser.baudrate=115200
			ser.port="/dev/ttyACM0"
			ser.open()

			if ser.isOpen():
				print("connected")
				time.sleep(2);
				ser.write(packetize('{"c":{"s":[10]}}'))

				while True:
					if millis()>timer:
						print("Writing")
						ser.write(packetize('{"u":{"s":['+str(pos)+']}}'))

						pos=pos+10
						if pos>140:
							pos=40
						timer=millis()+100

					if ser.inWaiting()>0:
						print ser.read(),

		except KeyboardInterrupt:
			exit(0)

		except Exception as error:
			print(error)
			ser.close()
			time.sleep(0.3)
			pass