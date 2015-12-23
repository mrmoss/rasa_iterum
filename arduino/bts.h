#ifndef BTS_H
#define BTS_H

#include <Arduino.h>
#include "json.h"
#include "peripheral_list.h"

struct bts_t
{
  uint8_t left_pin;
  uint8_t right_pin;
  int16_t speed;
  uint32_t timeout;
};

class bts_list_t:public peripheral_list_t<bts_t>
{
  public:
    inline uint32_t max_count() const
    {
      return 35;
    }

    inline void on_remove(bts_t& bts)
    {
      pinMode(bts.left_pin,INPUT);
      pinMode(bts.right_pin,INPUT);
    }

    inline bool on_add(bts_t& bts,json_ro_t& json,const std::string& path)
    {
      std::string left_pin(json[path+".l"]);
      std::string right_pin(json[path+".r"]);

      if(left_pin.size()<=0||right_pin.size()<=0)
          return false;

      bts.left_pin=std::stoul(left_pin);
      bts.right_pin=std::stoul(right_pin);

      Serial.println(bts.left_pin);
      Serial.println(bts.right_pin);

      if(bts.left_pin>1&&bts.right_pin>1)
      {
        pinMode(bts.left_pin,OUTPUT);
        pinMode(bts.right_pin,OUTPUT);
        bts.speed=0;
        digitalWrite(bts.left_pin,bts.speed);
        digitalWrite(bts.right_pin,bts.speed);
        bts.timeout=0;
        return true;
      }

      return false;
    }

    inline std::string key() const
    {
      return "b";
    }

    inline std::string on_get(bts_t& bts)
    {
      return std::to_string((int32_t)bts.speed);
    }

    inline void on_set(bts_t& bts,json_ro_t& json,const std::string& path)
    {
      bts.timeout=millis()+500;

      if(bts.left_pin>1&&bts.right_pin>1)
      {
        bts.speed=std::stol(json[path]);
        digitalWrite(bts.left_pin,0);
        digitalWrite(bts.right_pin,0);

        if(bts.speed<-255)
          bts.speed=-255;

        if(bts.speed>255)
          bts.speed=255;

        if(bts.speed<0)
          analogWrite(bts.right_pin,-bts.speed);
        else
          analogWrite(bts.left_pin,bts.speed);
      }
    }

    inline void on_loop(bts_t& bts)
    {
      if((uint32_t)millis()>=bts.timeout&&bts.speed!=0)
      {
        bts.speed=0;
        digitalWrite(bts.left_pin,0);
        digitalWrite(bts.right_pin,0);
      }
    }
};

#endif
