#ifndef COMMUNICATIONS_H
#define COMMUNICATIONS_H

#include <Arduino.h>
#include "bts.h"
#include "input_pin.h"
#include "json.h"
#include "output_pin.h"
#include "packetize.h"
#include "servo.h"

#include "MemoryFree.h"

namespace communications
{
    input_pin_list_t inputs;
    output_pin_list_t outputs;
    servo_list_t servos;
    bts_list_t btss;

    template<typename T> void configure(peripheral_list_t<T>& peripherals,json_ro_t& json)
    {
        list_node_t<T>* node=peripherals.list.head();

        while(node!=NULL)
        {
            peripherals.on_remove(node->data);
            node=node->next;
        }

        peripherals.list.clear();

        for(uint32_t ii=0;ii<peripherals.max_count();++ii)
        {
            std::string str(json["c."+peripherals.key()+"["+std::to_string(ii)+"]"]);

            if(str.size()<=0)
                break;

            T element;

            if(peripherals.on_add(str,element))
                peripherals.list.append(element);
        }
    }

    template<typename T> bool update(peripheral_list_t<T>& peripherals,json_ro_t& json,std::string& sensor_json)
    {
        bool changed=false;
        list_node_t<T>* node=peripherals.list.head();

        if(node!=NULL)
        {
            changed=true;
            uint32_t count=0;
            sensor_json+="\""+peripherals.key()+"\":[";

            while(node!=NULL)
            {
                std::string value_str(json["u."+peripherals.key()+"["+std::to_string(count)+"]"]);

                if(value_str.size()>0)
                    peripherals.on_set(node->data,value_str);

                sensor_json+=peripherals.on_get(node->data);

                ++count;
                node=node->next;

                if(node!=NULL)
                    sensor_json+=",";
            }

            sensor_json+="]";
        }

        return changed;
    }

    inline void receive(json_ro_t& json)
    {
        if(json["c"].size()>0)
        {
            configure(inputs,json);
            configure(outputs,json);
            configure(servos,json);
            configure(btss,json);
        }

        if(json["u"].size()>0)
        {
            std::string sensor_json;
            sensor_json+="{\"m\":";
            sensor_json+=std::to_string((uint32_t)freeMemory());
            sensor_json+=',';
  
            if(update(inputs,json,sensor_json))
                sensor_json+=",";

            if(update(outputs,json,sensor_json))
                sensor_json+=",";

            if(update(servos,json,sensor_json))
                sensor_json+=",";

            update(btss,json,sensor_json);

            if(sensor_json.size()>0&&sensor_json[sensor_json.size()-1]==',')
                sensor_json.erase(sensor_json.size()-1,1);

            sensor_json+="}";

            send_packet(sensor_json,Serial);
        }
    }

    template<typename T> void loop(peripheral_list_t<T>& peripherals)
    {
        list_node_t<T>* node=peripherals.list.head();

        while(node!=NULL)
        {
            peripherals.on_loop(node->data);
            node=node->next;
        }
    }

    inline void loop()
    {
        //loop(inputs);
        //loop(outputs);
        //loop(servos);
        loop(btss);
    }
}

#endif