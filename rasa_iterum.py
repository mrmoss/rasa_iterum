#!/usr/bin/env python


import arduino
import packetize
import sys
import time

def handle_same_line():
	global same_line

	if same_line:
		print('')
		same_line=False

if __name__=="__main__":
	global same_line
	same_line=False
	parser=packetize.parser_t()
	ard=arduino.connect()

	while True:
		try:
			same_line=False
			sys.stdout.write('Looking for an Arduino...')
			time.sleep(1)
			sys.stdout.flush()
			ards=arduino.list()

			if len(ards)>0:
				print('found.')
				ard=arduino.connect(ards[0])
				parser.reset()
				time.sleep(2)
			else:
				print('none found.')
				ard=arduino.connect()
				continue

			print('Sending configuration.')
			packetize.send_packet('{"c":{"o":[47,48,13],"i":[11,54],"b":[{"l":5,"r":6}]}}',ard)
			print('Connected:')

			timer=arduino.millis()+10
			start_time=0

			while ard.isOpen():
				if arduino.millis()>timer:
					packetize.send_packet('{"u":{}}',ard)
					start_time=arduino.millis()
					timer=arduino.millis()+10

				sensors=parser.parse(ard)

				if len(sensors)>0:
					same_line=True
					sys.stdout.write('\r'+sensors+' ('+str(arduino.millis()-start_time)+'ms)')
					sys.stdout.flush()

				time.sleep(0.1)

			handle_same_line()
			print('Arduino disconnected.')

		except KeyboardInterrupt:
			ard.close()
			handle_same_line()
			exit(0)

		except Exception as error:
			ard.close()
			handle_same_line()
			print('Error:  '+str(error))
			pass