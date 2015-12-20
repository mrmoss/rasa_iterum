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
	start_time=0
	pos=40;

	while True:
		try:
			print("connecting...")
			ser.baudrate=115200
			ser.port="/dev/ttyACM1"
			ser.open()

			if ser.isOpen():
				print("connected")
				time.sleep(2);
				#packetize.send_packet('{"c":{"o":[47,48,13],"i":[11,54]}}',ser)
				#packetize.send_packet('{"c":{"o":[47,48,13],"i":[11,54],"s":[10]}}',ser)
				packetize.send_packet('{"c":{"o":[47,48,13],"i":[11,54],"b":[{"l":5,"r":6}]}}',ser)

				while True:
					if millis()>timer:
						print("Writing")
						packetize.send_packet('{"u":{"s":['+str(pos)+'],'+
							'"o":['+str(pos)+','+str(int(pos>90))+','+str(int(pos>90))+'],'+
							'"b":[15]}}',ser)
						start_time=millis()
						pos=pos+10
						if pos>140:
							pos=40
						timer=millis()+100

					sensors=parser.parse(ser)
					if len(sensors)>0:
						print(sensors+" ("+str(millis()-start_time)+"ms)")

		except KeyboardInterrupt:
			exit(0)

		except Exception as error:
			print(error)
			ser.close()
			time.sleep(0.3)
			pass