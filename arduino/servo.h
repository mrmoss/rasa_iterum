#ifndef SERVO_PERIPHERAL_H
#define SERVO_PERIPHERAL_H

#include <Arduino.h>
#include <Servo.h>
#include "peripheral_list.h"

class servo_list_t:public peripheral_list_t<Servo>
{
	public:
		inline uint32_t max_count() const
		{
			return 70;
		}

		inline void on_remove(Servo& servo)
		{
			servo.detach();
		}

		inline bool on_add(const std::string& pin_str,Servo& servo)
		{
			uint32_t pin=std::stoul(pin_str);

			if(pin>1)
			{
				servo.attach(pin);
				servo.write(90);
				return true;
			}

			return false;
		}

		inline std::string key() const
		{
			return "s";
		}

		inline std::string on_get(Servo& servo)
		{
			return std::to_string((uint32_t)servo.read());
		}

		inline void on_set(Servo& servo,const std::string& str_value)
		{
			uint32_t value=std::stoul(str_value);

			if(value<40)
				value=40;
			if(value>140)
				value=140;

			servo.write(value);
		}

		inline void on_loop(Servo& servo)
		{}
};

#endif
