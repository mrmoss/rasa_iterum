import arduino
from arduino import millis
import packetize

def get_status_blocking(ard,parser,tries=3):
	for ii in range(0,tries):
		try_timer=millis()+2000
		check_timer=millis()

		while ard.is_opened() and millis()<=try_timer:
			if millis()>check_timer:
				packetize.send_packet('{s:{}}',ard)
				check_timer=millis()+100

			status=parser.parse(ard)

			if status and 'p' in status:
				return status

	raise Exception('Firmware not responding to status check.')

def send_configuration(ard,parser,configuration,tries=3):
	for ii in range(0,tries):
		try_timer=millis()+2000
		check_timer=millis()

		while ard.is_opened() and millis()<=try_timer:
			if millis()>check_timer:
				packetize.send_packet(configuration,ard)
				check_timer=millis()+100

			config=parser.parse(ard)

			if config and 'c' in config:
				return config

	raise Exception('Firmware not responding to configuration check.')