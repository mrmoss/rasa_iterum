#ifndef PERIPHERALS_H
#define PERIPHERALS_H

#include <Arduino.h>
#include "json.h"
#include "list.h"
#include "packetize.h"
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

    inline void configure_servos(json_ro_t& json)
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
    
    inline void configure_outputs(json_ro_t& json)
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

    inline void configure_inputs(json_ro_t& json)
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

    inline void configure_roombas(json_ro_t& json)
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
    
    inline void configure(json_ro_t& json)
    {
        configure_servos(json);
        configure_outputs(json);
        configure_inputs(json);
        configure_roombas(json);
    }
    
    inline void update_servos(json_ro_t& json,std::string& sensor_json)
    {
        list_t<Servo>::node_t* servo=servos.head();
        uint32_t count=0;
        sensor_json+="\"o\":[";
    
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
            sensor_json+=std::to_string(value);
            ++count;
            servo=servo->next;

            if(servo!=NULL)
                sensor_json+=",";
            else
                sensor_json+="]";
        }
    }
    
    inline void update_outputs(json_ro_t& json,std::string& sensor_json)
    {
        list_t<uint8_t>::node_t* output=outputs.head();
        uint32_t count=0;
        sensor_json+="\"o\":[";
    
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
            }
            else
            {
                analogWrite(pin,value);
            }

            sensor_json+=std::to_string(value);
            ++count;
            output=output->next;

            if(output!=NULL)
                sensor_json+=",";
            else
                sensor_json+="]";
        }
    }

    inline void update_inputs(json_ro_t& json,std::string& sensor_json)
    {
        list_t<uint8_t>::node_t* input=inputs.head();
        sensor_json+="\"i\":[";
    
        while(input!=NULL)
        {
            uint32_t pin=input->data;
            uint32_t value=0;

            if(pin>=A0)
                value=analogRead(pin);
            else
                value=digitalRead(pin);

            sensor_json+=std::to_string(value);
            input=input->next;

            if(input!=NULL)
                sensor_json+=",";
            else
                sensor_json+="]";
        }
    }

    inline void update(json_ro_t& json)
    {
        std::string sensor_json;
        sensor_json+="{";
            update_servos(json,sensor_json);
        sensor_json+=",";
            update_outputs(json,sensor_json);
        sensor_json+=",";
            update_inputs(json,sensor_json);
        sensor_json+="}";
        send_packet(sensor_json,Serial);
    }

    inline void loop(json_ro_t& json)
    {
        if(json["c"].size()>0)
            configure(json);
        if(json["u"].size()>0)
            update(json);
    }
}

#endif
