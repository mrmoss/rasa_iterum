import serial
import time

def millis():
	return int(round(time.time()*1000))

class arduino_t:
	def __init__(self):
		self.serial=serial.Serial()

	def list(self):
		valid_ports=[]

		try:
			import serial.tools.list_ports

			for port in serial.tools.list_ports.comports():
				if port[2]!='n/a':
					valid_ports.append(port[0])

		except:
			pass

		return valid_ports

	def connect(self,port=None,baud=57600):
		self.serial=serial.Serial()
		self.serial.baudrate=baud

		if not port:
			ports=self.list()

			if len(ports)<=0:
				raise Exception('Could not find an Arduino.')

			port=ports[0]

		self.serial.port=port
		try:
			self.serial.open()
		except:
			self.close()

		if not self.is_opened():
			raise Exception('Could not open Arduino on serial port "'+str(port)+'".')

		self.serial.setDTR(False)
		time.sleep(2)
		self.serial.flushInput()
		self.serial.setDTR(True)
		time.sleep(2)

	def close(self):
		self.serial.close()
		self.serial=serial.Serial()

	def is_opened(self):
		return self.serial.isOpen()

	def write(self,data):
		return self.serial.write(data)

	def read(self):
		return self.serial.read()

	def available(self):
		return self.serial.inWaiting()

	def flush(self):
		self.serial.flushOutput()

	def name(self):
		return str(self.serial.port)