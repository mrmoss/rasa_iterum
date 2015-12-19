#ifndef PACKET_H
#define PACKET_H

#include <Arduino.h>
#include "string.h"

#define PACKET_HEADER 0x5f

inline void send_packet(const std::string& data,HardwareSerial& serial)
{
    uint8_t header=PACKET_HEADER;
    uint16_t size=data.size();
    serial.write((uint8_t*)&header,1);
    serial.write((uint8_t*)&size,2);
    serial.write((uint8_t*)data.c_str(),size);
}

class parser_t
{
    enum state_t
    {
        HEADER,
        SIZE_0,
        SIZE_1,
        DATA
    };

    public:
        parser_t():buffer_m(""),size_m(0),ptr_m(0),state_m(HEADER)
        {}

        std::string parse(HardwareSerial& serial)
        {
            uint8_t temp;

            while(serial.available()>0&&serial.readBytes(&temp,1)==1)
            {
                if(state_m==HEADER&&temp==PACKET_HEADER)
                {
                    state_m=SIZE_0;
                }
                else if(state_m==SIZE_0)
                {
                    ((uint8_t*)&size_m)[0]=temp;
                    state_m=SIZE_1;
                }
                else if(state_m==SIZE_1)
                {
                    ((uint8_t*)&size_m)[1]=temp;
                    state_m=DATA;
                    buffer_m.resize(size_m);
                }
                else if(state_m==DATA)
                {
                    buffer_m[ptr_m]=temp;
                    ++ptr_m;

                    if(ptr_m>=size_m)
                    {
                        std::string ret;
                        ret.swap(buffer_m);
                        reset();
                        return ret;
                    }
                }
            }

            return "";
        }

        void reset()
        {
            buffer_m.clear();
            size_m=0;
            ptr_m=0;
            state_m=HEADER;
        }

    private:
        std::string buffer_m;
        uint16_t size_m;
        uint16_t ptr_m;
        state_t state_m;
};

#endif