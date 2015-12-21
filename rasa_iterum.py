#!/usr/bin/env python

import arduino
import packetize
import sys
import time

def millis():
	return int(round(time.time()*1000))

if __name__=="__main__":
	ard=arduino.arduino_t()
	parser=packetize.parser_t()
	parser.reset()

	while True:
		try:
			print('Searching for an Arduino...')
			ard.connect()

			print('Arduino found on port "'+ard.name()+'".')
			print('Checking Arduino...')

			check_send_timer=millis()
			check_timer=millis()+2000
			checked=False

			while ard.is_opened() and millis()<=check_timer:
				if millis()>check_send_timer:
					packetize.send_packet('{"s":{}}',ard)
					check_send_timer=millis()+100

				sensors=parser.parse(ard)

				if sensors and 'm' in sensors:
					checked=True
					break

			if not checked:
				raise Exception('Firmware not responding.')

			print('Sending configuration...')
			packetize.send_packet('{"c":{"o":[47,48,13],"i":[11,54],"b":[{"l":5,"r":6}]}}',ard)
			print('Connected:')

			send_timer=millis()+10
			send_start_time=0

			while ard.is_opened():
				if millis()>send_timer:
					packetize.send_packet('{"u":{}}',ard)
					send_timer=millis()+10
					send_start_time=millis()

				sensors=parser.parse(ard)

				if sensors:
					print(str(sensors)+' ('+str(millis()-send_start_time)+'ms)')

				time.sleep(0.1)

			print('Arduino disconnected.')

		except KeyboardInterrupt:
			ard.close()
			exit(0)

		except Exception as error:
			ard.close()
			print('Error:  '+str(error))
			time.sleep(1)
			pass