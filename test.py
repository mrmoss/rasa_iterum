#!/usr/bin/env python

import serial
import time
import packetize
from packetize import parser_t
import random

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
	pos=40
	print(ord(packetize.make_crc("hello")))
	test=True

	while True:
		try:
			print("connecting...")
			ser.baudrate=57600
			ser.port="/dev/ttyACM1"
			ser.open()

			if ser.isOpen():
				ser.setDTR(False)
				time.sleep(2)
				ser.flushInput()
				ser.setDTR(True)
				time.sleep(2)
				print("connected")
				#packetize.send_packet('{"c":{"o":[47,48,13],"i":[11,54]}}',ser)
				#packetize.send_packet('{"c":{"o":[47,48,13],"i":[11,54],"s":[10]}}',ser)
				packetize.send_packet('{"c":{"o":[47,48,13],"i":[11,54],"b":[{"l":5,"r":6}]}}',ser)

				while ser.isOpen():
					if millis()>timer:
						if test:
							#packetize.send_packet('{"u":{"o":['+str(pos)+','+str(pos)+']}}',ser)
							#packetize.send_packet('{"u":{"s":['+str(pos)+']}}',ser);
							packetize.send_packet('{"u":{"s":['+str(pos)+'],'+
								'"o":['+str(pos)+','+str(int(pos>90))+','+str(random.randint(0,255))+'],'+
								'"b":['+str(pos-70)+']}}',ser)
							#packetize.send_packet('{"u":{"o":[0,1,255]}}',ser)
							#packetize.send_packet('{}',ser)
							test=False
						else:
							packetize.send_packet('{"u":{}}',ser)
						start_time=millis()
						timer=millis()+700
						pos+=10

						if(pos>140):
							pos=0

					#while ser.inWaiting()>0:
					#	print ser.read(),

					sensors=parser.parse(ser)
					if len(sensors)>0:
						print(sensors+" ("+str(millis()-start_time)+"ms)")

		except KeyboardInterrupt:
			ser.close()
			exit(0)

		except Exception as error:
			print(error)
			ser.close()
			time.sleep(0.3)
			pass