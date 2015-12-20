#ifndef OUTPUT_PIN_H
#define OUTPUT_PIN_H

#include <Arduino.h>
#include "peripheral_list.h"

struct output_pin_t
{
    uint8_t pin;
    uint8_t value;
};

class output_pin_list_t:public peripheral_list_t<output_pin_t>
{
    public:
        inline uint32_t max_count() const
        {
            return 70;
        }

        inline void on_remove(output_pin_t& output)
        {
            pinMode(output.pin,INPUT);
        }

        inline bool on_add(const std::string& pin_str,output_pin_t& output)
        {
            output.pin=std::stoul(pin_str);

            if(output.pin>1)
            {
                pinMode(output.pin,OUTPUT);
                output.value=0;
                digitalWrite(output.pin,output.value);
                return true;
            }

            return false;
        }

        inline std::string key() const
        {
            return "o";
        }

        inline std::string on_get(output_pin_t& output)
        {
            return std::to_string((uint32_t)output.value);
        }

       inline  void on_set(output_pin_t& output,const std::string& str_value)
        {
            uint32_t value=std::stoul(str_value);
            output.value=value;

            if(digitalPinToTimer(output.pin)!=NOT_ON_TIMER)
            {
                analogWrite(output.pin,output.value);
                return;
            }

            if(output.value>0)
                output.value=1;

            digitalWrite(output.pin,output.value);
        }

        inline void on_loop(uint8_t& pin)
        {}
};

#endif
