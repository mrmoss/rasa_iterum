from enum import Enum
import struct

class state_t(Enum):
	HEADER=0
	SIZE_0=1
	SIZE_1=2
	DATA=3
	CRC=4

def make_crc(data):
	crc=0

	for ii in data:
		crc^=ord(ii)

	return chr(crc)

def send_packet(data,serial):
	packet=""
	packet+=chr(0x5f)
	packet+=struct.pack('<H',len(data))
	packet+=data
	packet+=make_crc(data)
	serial.write(packet)
	serial.flushOutput()

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
					self.state_m=state_t.CRC

			elif self.state_m==state_t.CRC:
				ret=""

				if temp==make_crc(self.buffer_m):
					ret=self.buffer_m

				self.reset()
				return ret

		return ""

	def reset(self):
		self.buffer_m=""
		self.size_m=0
		self.state_m=state_t.HEADER
