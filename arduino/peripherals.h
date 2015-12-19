#ifndef PERIPHERALS_H
#define PERIPHERALS_H

#include <Arduino.h>
#include "json.h"
#include "list.h"
#include <Servo.h>

#define MAX_OUTPUTS 70
#define MAX_INPUTS  70
#define MAX_ROOMBAS 3

#define SERVO_DIR_LIMIT 40

namespace peripherals
{
    list_t<Servo> servos;
    list_t<uint8_t> outputs;
    list_t<uint8_t> inputs;
    list_t<uint8_t> roombas;

    void configure_servos(json_ro_t& json)
    {
        list_t<Servo>::node_t* servo=servos.head();
        uint32_t count=0;
    
        while(servo!=NULL)
        {
            servo->data.detach();
            ++count;
            servo=servo->next;
        }

        servos.clear();
    
        for(uint32_t ii=0;ii<MAX_OUTPUTS;++ii)
        {
            std::string pin_str(json["c.s["+std::to_string(ii)+"]"]);
    
            if(pin_str.size()<=0)
                break;
    
            uint32_t pin=std::stoul(pin_str);
    
            if(pin>1)
            {
                ("Added servo on pin: "+std::to_string(pin)).println(Serial);
                Servo servo;
                servo.attach(pin);
                servo.write(90);
                servos.append(servo);
            }
        }
    }
    
    void configure_outputs(json_ro_t& json)
    {
        outputs.clear();
    
        for(uint32_t ii=0;ii<MAX_OUTPUTS;++ii)
        {
            std::string pin_str(json["c.o["+std::to_string(ii)+"]"]);
    
            if(pin_str.size()<=0)
                break;
    
            uint32_t pin=std::stoul(pin_str);
    
            if(pin>1)
            {
                ("Added output on pin: "+std::to_string(pin)).println(Serial);
                outputs.append((uint8_t)pin);
                pinMode(pin,OUTPUT);
                digitalWrite(pin,0);
            }
        }
    }

    void configure_inputs(json_ro_t& json)
    {
        inputs.clear();
    
        for(uint32_t ii=0;ii<MAX_INPUTS;++ii)
        {
            std::string pin_str(json["c.i["+std::to_string(ii)+"]"]);
    
            if(pin_str.size()<=0)
                break;
    
            uint32_t pin=std::stoul(pin_str);
    
            if(pin>1)
            {
                ("Added input on pin: "+std::to_string(pin)).println(Serial);
                inputs.append((uint8_t)pin);
                pinMode(pin,INPUT);
            }
        }
    }

    void configure_roombas(json_ro_t& json)
    {
        roombas.clear();

        for(uint32_t ii=0;ii<MAX_ROOMBAS;++ii)
        {
            std::string serial_str(json["c.r["+std::to_string(ii)+"]"]);

            if(serial_str.size()<=0)
                break;

            uint32_t serial=std::stoul(serial_str);

            if(serial>0)
            {
                ("Added roomba on serial: X"+std::to_string(serial)).println(Serial);
                roombas.append((uint8_t)serial);
            }
        }
    }
    
    void configure(json_ro_t& json)
    {
        configure_servos(json);
        configure_outputs(json);
        configure_inputs(json);
        configure_roombas(json);
    }
    
    void update_servos(json_ro_t& json)
    {
        list_t<Servo>::node_t* servo=servos.head();
        uint32_t count=0;
    
        while(servo!=NULL)
        {
            std::string value_str(json["u.s["+std::to_string(count)+"]"]);
    
            if(value_str.size()<=0)
                break;
    
            uint32_t value=std::stoul(value_str);

            if(value<SERVO_DIR_LIMIT)
                value=SERVO_DIR_LIMIT;

            if(value>180-SERVO_DIR_LIMIT)
                value=180-SERVO_DIR_LIMIT;

            servo->data.write(value);
            ("Setting servo["+std::to_string(count)+"]="+std::to_string(value)).println(Serial);
            ++count;
            servo=servo->next;
        }
    }
    
    void update_outputs(json_ro_t& json)
    {
        list_t<uint8_t>::node_t* output=outputs.head();
        uint32_t count=0;
    
        while(output!=NULL)
        {
            std::string value_str(json["u.o["+std::to_string(count)+"]"]);
    
            if(value_str.size()<=0)
                break;
    
            uint32_t pin=output->data;
            uint32_t value=std::stoul(value_str);

            if(digitalPinToTimer(pin)==NOT_ON_TIMER)
            {
                if(value>0)
                    value=1;

                digitalWrite(pin,value);
                ("Setting digtital output pin "+std::to_string(pin)+"="+std::to_string(value)).println(Serial);
            }
            else
            {
                analogWrite(pin,value);
                ("Setting analog output pin "+std::to_string(pin)+"="+std::to_string(value)).println(Serial);
            }

            ++count;
            output=output->next;
        }
    }

    void update_inputs(json_ro_t& json)
    {
        list_t<uint8_t>::node_t* input=inputs.head();
    
        while(input!=NULL)
        {
            uint32_t pin=input->data;
            uint32_t value=0;

            if(pin>=A0)
                value=analogRead(pin);
            else
                value=digitalRead(pin);

            ("Reading input pin "+std::to_string(pin)+"="+std::to_string(value)).println(Serial);
            input=input->next;
        }
    }

    void update(json_ro_t& json)
    {
        update_servos(json);
        update_outputs(json);
        update_inputs(json);
    }

    void loop(json_ro_t& json)
    {
        if(json["c"].size()>0)
            configure(json);
        if(json["u"].size()>0)
            update(json);
    }
}

#endif
