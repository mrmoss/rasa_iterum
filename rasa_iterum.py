#!/usr/bin/env python

import arduino
from arduino import millis
import cli
import firmware
import packetize
import sys
import time
import window

if __name__=="__main__":
	values={"--gui":False,"--serial":True}

	try:
		values=cli.parse(sys.argv[1:],{"--no-gui":False,"--serial":True})
	except Exception as error:
		print(error)
		print("Usage: ./rasa_iterum.py [--no-gui][--serial NAME]")
		exit(1)

	if not values["--no-gui"]:
		window=window.window_t()
		exit(0)

	print("Rasa Iterum")
	ard=arduino.arduino_t()
	parser=packetize.parser_t()
	parser.reset()

	while True:
		try:
			if values["--serial"]==True:
				print('Searching for an Arduino...')
				ard.connect()
				print('Arduino found on port "'+ard.name()+'".')

			else:
				print('Attempting to connect to "'+values["--serial"]+'"...')
				ard.connect(values["--serial"])

			print('Checking firmware...')
			status=firmware.get_status_blocking(ard,parser)
			print('Firmware confirmed ('+status['p']+' with '+str(status['m'])+' bytes of free RAM).')

			print('Sending configuration...')
			firmware.send_configuration(ard,parser,'{c:{i:[14,15],b:[{l:5,r:6}]}}')
			print('Configuration verified.')
			print('Connected.')

			while ard.is_opened():
				packetize.send_packet('{u:{}}',ard)
				sensors=parser.parse(ard)
#
				if sensors:
					sys.stdout.write(str(sensors)+"\n")
					sys.stdout.flush()

				time.sleep(0.1)

			print('Disconnected.')

		except KeyboardInterrupt:
			ard.close()
			exit(0)

		except Exception as error:
			ard.close()
			print(str(error))
			time.sleep(1)
			pass