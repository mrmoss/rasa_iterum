#!/usr/bin/env python

import arduino
import packetize
import sys
import time

def millis():
	return int(round(time.time()*1000))

if __name__=="__main__":
	parser=packetize.parser_t()

	ard=arduino.arduino_t()

	while True:
		try:
			print('Looking for an Arduino...')
			ard.connect()

			print('Sending configuration.')
			packetize.send_packet('{"c":{"o":[47,48,13],"i":[11,54],"b":[{"l":5,"r":6}]}}',ard)
			print('Connected:')

			timer=millis()+10
			start_time=0

			while ard.is_opened():
				if millis()>timer:
					packetize.send_packet('{"u":{}}',ard)
					start_time=millis()
					timer=millis()+10

				sensors=parser.parse(ard)

				if len(sensors)>0:
					print(sensors+' ('+str(millis()-start_time)+'ms)')

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