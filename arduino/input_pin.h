#ifndef INPUT_PIN_H
#define INPUT_PIN_H

#include <Arduino.h>
#include "peripheral_list.h"

class input_pin_list_t:public peripheral_list_t<uint8_t>
{
  public:
    inline uint32_t max_count() const
    {
      return 70;
    }

    inline void on_remove(uint8_t& pin)
    {}

    inline bool on_add(uint8_t& pin,json_ro_t& json,const std::string& path)
    {
      pin=std::stoul(json[path]);

      if(pin>1)
      {
        pinMode(pin,INPUT);
        return true;
      }

      return false;
    }

    inline std::string key() const
    {
      return "i";
    }

    inline std::string on_get(uint8_t& pin)
    {
      uint32_t value=0;

      if(pin>=A0)
        value=analogRead(pin);
      else
        value=digitalRead(pin);

      return std::to_string(value);
    }

    inline void on_set(uint8_t& pin,json_ro_t& json,const std::string& path)
    {}

    inline void on_loop(uint8_t& pin)
    {}
};

#endif
