#!/usr/bin/env python

import serial
import time
import packetize
from packetize import parser_t

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

if __name__=="__main__":
	ser=serial.Serial()
	parser=parser_t()
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
				packetize.send_packet('{"c":{"s":[10],"o":[47,13],"i":[11]}}',ser)
				#packetize.send_packet('{"c":{"o":[47,13],"i":[11]}}',ser)

				while True:
					if millis()>timer:
						print("Writing")
						packetize.send_packet('{"u":{"s":['+str(pos)+'],'+
							'"o":['+str(pos)+','+str(pos/100)+']}}',ser)
						pos=pos+10
						if pos>140:
							pos=40
						timer=millis()+300

					sensors=parser.parse(ser)
					if len(sensors)>0:
						print(sensors)

		except KeyboardInterrupt:
			exit(0)

		except Exception as error:
			print(error)
			ser.close()
			time.sleep(0.3)
			pass