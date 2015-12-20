#ifndef BTS_H
#define BTS_H

#include <Arduino.h>

class bts_t
{
    public:
        bts_t(const uint8_t left_pin=0,const uint8_t right_pin=0):
            left_pin_m(left_pin),right_pin_m(right_pin),power_m(0),timeout_m(0)
        {
            Serial.print("Pins:  ");
            Serial.print(left_pin_m);
            Serial.print("  ");
            Serial.println(right_pin_m);
        }

        ~bts_t()
        {
            if(left_pin_m>1&&right_pin_m>1)
            {
                pinMode(left_pin_m,OUTPUT);
                pinMode(right_pin_m,OUTPUT);
                digitalWrite(left_pin_m,0);
                digitalWrite(right_pin_m,0);
                pinMode(left_pin_m,INPUT);
                pinMode(right_pin_m,INPUT);
            }
        }

        void drive(int32_t power,const bool timeout=true)
        {
            if(left_pin_m>1&&right_pin_m>1)
            {
                power_m=power;

                if(power_m<-255)
                    power_m=-255;

                if(power_m>255)
                    power_m=255;

                pinMode(left_pin_m,OUTPUT);
                pinMode(right_pin_m,OUTPUT);

                if(power_m<0)
                {
                    analogWrite(left_pin_m,-power_m);
                    analogWrite(right_pin_m,0);
                }
                else
                {
                    analogWrite(left_pin_m,0);
                    analogWrite(right_pin_m,power_m);
                }

                if(timeout)
                    timeout_m=millis()+500;
            }
        }

        void loop()
        {
            if((uint32_t)millis()>=timeout_m)
                drive(0,false);
        }

        int32_t power() const
        {
            return power_m;
        }

    private:
        uint8_t left_pin_m;
        uint8_t right_pin_m;
        int32_t power_m;
        uint32_t timeout_m;
};

#endif
