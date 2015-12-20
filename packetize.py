from enum import Enum
import struct

def send_packet(data,serial):
	packet=""
	packet+=chr(0x5f)
	packet+=struct.pack('<H',len(data))
	packet+=data
	serial.write(packet)

class state_t(Enum):
	HEADER=0
	SIZE_0=1
	SIZE_1=2
	DATA=3

class parser_t:
	PACKET_HEADER=chr(0x5f)

	def __init__(self):
		self.reset()

	def parse(self,serial):
		while serial.inWaiting()>0:
			temp=serial.read()

			if self.state_m==state_t.HEADER and temp==self.PACKET_HEADER:
				self.state_m=state_t.SIZE_0

			elif self.state_m==state_t.SIZE_0:
				self.size_m+=ord(temp)
				self.state_m=state_t.SIZE_1

			elif self.state_m==state_t.SIZE_1:
				self.size_m+=ord(temp)<<8
				self.state_m=state_t.DATA

			elif self.state_m==state_t.DATA:
				self.buffer_m+=temp

				if len(self.buffer_m)>=self.size_m:
					ret=self.buffer_m
					self.reset()
					return ret

		return ""

	def reset(self):
		self.buffer_m=""
		self.size_m=0
		self.state_m=state_t.HEADER
