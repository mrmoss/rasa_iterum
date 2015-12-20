#ifndef PERIPHERALS_H
#define PERIPHERALS_H

#include <Arduino.h>
#include "bts.h"
#include "json.h"
#include "list.h"
#include "packetize.h"
#include <Servo.h>

#define MAX_PINS 70

#define SERVO_DIR_LIMIT 40

namespace peripherals
{
    list_t<uint8_t> outputs;
    list_t<uint8_t> inputs;
    list_t<Servo> servos;
    list_t<bts_t> btss;

    inline void configure_outputs(json_ro_t& json)
    {
        list_t<uint8_t>::node_t* output=outputs.head();

        while(output!=NULL)
        {
            pinMode(output->data,INPUT);
            output=output->next;
        }

        outputs.clear();

        for(uint32_t ii=0;ii<MAX_PINS;++ii)
        {
            std::string pin_str(json["c.o["+std::to_string(ii)+"]"]);

            if(pin_str.size()<=0)
                break;

            uint32_t pin=std::stoul(pin_str);

            if(pin>1)
            {
                outputs.append((uint8_t)pin);
                pinMode(pin,OUTPUT);
                digitalWrite(pin,0);
            }
        }
    }

    inline void configure_inputs(json_ro_t& json)
    {
        inputs.clear();

        for(uint32_t ii=0;ii<MAX_PINS;++ii)
        {
            std::string pin_str(json["c.i["+std::to_string(ii)+"]"]);

            if(pin_str.size()<=0)
                break;

            uint32_t pin=std::stoul(pin_str);

            if(pin>1)
            {
                inputs.append((uint8_t)pin);
                pinMode(pin,INPUT);
            }
        }
    }

    inline void configure_btss(json_ro_t& json)
    {
        btss.clear();

        for(uint32_t ii=0;ii<MAX_PINS;++ii)
        {
            std::string left_pin_str(json["c.b["+std::to_string(ii)+"].l"]);
            std::string right_pin_str(json["c.b["+std::to_string(ii)+"].r"]);

            if(left_pin_str.size()<=0||right_pin_str.size()<=0)
                break;

            uint32_t left_pin=std::stoul(left_pin_str);
            uint32_t right_pin=std::stoul(right_pin_str);

            if(left_pin>1&&right_pin>1)
            {
                bts_t bts(left_pin,right_pin);
                btss.append(bts);
            }
        }
    }

    inline void configure_servos(json_ro_t& json)
    {
        list_t<Servo>::node_t* servo=servos.head();

        while(servo!=NULL)
        {
            servo->data.detach();
            servo=servo->next;
        }

        servos.clear();

        for(uint32_t ii=0;ii<MAX_PINS;++ii)
        {
            std::string pin_str(json["c.s["+std::to_string(ii)+"]"]);

            if(pin_str.size()<=0)
                break;

            uint32_t pin=std::stoul(pin_str);

            if(pin>1)
            {
                Servo servo;
                servo.attach(pin);
                servo.write(90);
                servos.append(servo);
            }
        }
    }

    inline void configure(json_ro_t& json)
    {
        configure_outputs(json);
        configure_inputs(json);
        configure_servos(json);
        configure_btss(json);
    }

    inline bool receive_outputs(json_ro_t& json,std::string& sensor_json)
    {
        bool changed=false;
        list_t<uint8_t>::node_t* output=outputs.head();

        if(output!=NULL)
        {
            changed=true;
            uint32_t count=0;
            sensor_json+="\"o\":[";

            while(output!=NULL)
            {
                uint32_t pin=output->data;
                uint32_t value=0;
                std::string value_str(json["u.o["+std::to_string(count)+"]"]);

                if(value_str.size()>0)
                {
                    value=std::stoul(value_str);

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
                }

                if(digitalPinToTimer(pin)==NOT_ON_TIMER)
                    value=digitalRead(pin);
                else
                    value=analogRead(pin);

                sensor_json+=std::to_string(value);

                ++count;
                output=output->next;

                if(output!=NULL)
                    sensor_json+=",";
            }

            sensor_json+="]";
        }

        return changed;
    }

    inline bool receive_inputs(json_ro_t& json,std::string& sensor_json)
    {
        bool changed=false;
        list_t<uint8_t>::node_t* input=inputs.head();

        if(input!=NULL)
        {
            changed=true;
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
            }

            sensor_json+="]";
        }

        return changed;
    }

    inline bool receive_servos(json_ro_t& json,std::string& sensor_json)
    {
        bool changed=false;
        list_t<Servo>::node_t* servo=servos.head();

        if(servo!=NULL)
        {
            changed=true;
            uint32_t count=0;
            sensor_json+="\"s\":[";

            while(servo!=NULL)
            {
                std::string value_str(json["u.s["+std::to_string(count)+"]"]);

                if(value_str.size()>0)
                {
                    uint32_t value=std::stoul(value_str);
    
                    if(value<SERVO_DIR_LIMIT)
                        value=SERVO_DIR_LIMIT;
    
                    if(value>180-SERVO_DIR_LIMIT)
                        value=180-SERVO_DIR_LIMIT;

                    servo->data.write(value);
                }

                sensor_json+=std::to_string((uint32_t)servo->data.read());
                ++count;
                servo=servo->next;

                if(servo!=NULL)
                    sensor_json+=",";
            }

            sensor_json+="]";
        }

        return changed;
    }

    inline bool receive_btss(json_ro_t& json,std::string& sensor_json)
    {
        bool changed=false;
        list_t<bts_t>::node_t* bts=btss.head();

        if(bts!=NULL)
        {
            changed=true;
            uint32_t count=0;
            sensor_json+="\"b\":[";

            while(bts!=NULL)
            {
                std::string value_str(json["u.b["+std::to_string(count)+"]"]);

                if(value_str.size()>0)
                {
                    int32_t value=std::stol(value_str);
                    bts->data.drive(value);
                }
   
                sensor_json+=std::to_string(bts->data.power());
                ++count;
                bts=bts->next;

                if(bts!=NULL)
                    sensor_json+=",";
            }

            sensor_json+="]";
        }

        return changed;
    }

    inline void receive(json_ro_t& json)
    {
       if(json["c"].size()>0)
            configure(json);
            
        std::string sensor_json;
        sensor_json+="{";

        if(json["u"].size()>0)
        {
            if(receive_outputs(json,sensor_json))
                sensor_json+=",";
            if(receive_inputs(json,sensor_json))
                sensor_json+=",";
            if(receive_servos(json,sensor_json))
                sensor_json+=",";
            receive_btss(json,sensor_json);

            if(sensor_json[sensor_json.size()-1]==',')
                sensor_json.resize(sensor_json.size()-1);

            sensor_json+="}";
            send_packet(sensor_json,Serial);
        }
    }

    inline void loop()
    {
        list_t<bts_t>::node_t* bts=btss.head();

        while(bts!=NULL)
        {
            bts->data.loop();
            bts=bts->next;
        }
    }
}

#endif
