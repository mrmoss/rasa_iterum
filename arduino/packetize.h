#ifndef PACKET_H
#define PACKET_H

#include <Arduino.h>
#include "string.h"

#define PACKET_HEADER 0x5f

inline uint8_t make_crc(const std::string& data)
{
  uint8_t crc=0x00;

  for(uint32_t ii=0;ii<data.size();++ii)
    crc^=data[ii];

  return crc;
}

inline void send_packet(const std::string& data,HardwareSerial& serial)
{
  uint8_t header=PACKET_HEADER;
  uint16_t size=data.size();
  serial.write(&header,1);
  serial.write((uint8_t*)&size,2);
  serial.write((uint8_t*)data.c_str(),size);
  uint8_t crc=make_crc(data);
  serial.write(&crc,1);
}

class parser_t
{
  enum state_t
  {
    HEADER,
    SIZE_0,
    SIZE_1,
    DATA,
    CRC
  };

  public:
    parser_t():buffer_m(""),size_m(0),state_m(HEADER)
    {}

    inline std::string parse(HardwareSerial& serial)
    {
      uint32_t count=0;
      uint8_t temp;

      while(count<1000&&serial.available()>0&&serial.readBytes(&temp,1)==1)
      {
        ++count;

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
          buffer_m.reserve(size_m);
        }
        else if(state_m==DATA)
        {
          buffer_m+=temp;

          if(buffer_m.size()>=size_m)
            state_m=CRC;
        }
        else if(state_m==CRC)
        {
          std::string ret;

          uint8_t crc=make_crc(buffer_m);

          if(temp==crc)
            ret=buffer_m;

          reset();
          return ret;
        }
      }

      return "";
    }

    inline void reset()
    {
      buffer_m.clear();
      size_m=0;
      state_m=HEADER;
    }

  private:
    std::string buffer_m;
    uint16_t size_m;
    state_t state_m;
};

#endif
