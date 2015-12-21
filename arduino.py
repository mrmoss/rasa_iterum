import serial
import time

def millis():
	return int(round(time.time()*1000))

def list():
	valid_ports=[]

	try:
		import serial.tools.list_ports

		for port in serial.tools.list_ports.comports():
			if port[2]!='n/a':
				valid_ports.append(port[0])

	except:
		None

	return valid_ports

def connect(port=None,baud=57600):
	if port:
		ard=serial.Serial()
		ard.baudrate=baud
		ard.port=port

		ard.open()

		if ard.isOpen():
			ard.setDTR(False)
			time.sleep(1)
			ard.flushInput()
			ard.setDTR(True)
			time.sleep(1)
			print('Opened Arduino on serial port "'+port+'".')
			return ard

		raise Exception('Could not open Arduino on serial port "'+port+'".')

	return serial.Serial()