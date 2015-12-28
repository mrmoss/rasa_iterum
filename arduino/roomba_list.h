#ifndef ROOMBA_LIST_H
#define ROOMBA_LIST_H

#include <Arduino.h>
#include "json.h"
#include "peripheral_list.h"
#include "roomba.h"

class roomba_list_t:public peripheral_list_t<roomba_t>
{
  public:
    inline uint32_t max_count() const
    {
      return 3;
    }

    inline void on_remove(roomba_t& roomba)
    {
      if(roomba.wake_pin>1)
        pinMode(roomba.wake_pin,INPUT);
    }

    inline bool on_add(roomba_t& roomba,json_ro_t& json,const std::string& path)
    {
      std::string serial_channel_str(json[path+".s"]);
      std::string wake_pin_str(json[path+".w"]);

      if(serial_channel_str.size()<=0&&wake_pin_str.size()<=0)
          return false;

      uint8_t serial_channel=std::stoul(serial_channel_str);
      uint8_t wake_pin=std::stoul(wake_pin_str);

      if(serial_channel<=0||serial_channel>3||wake_pin<=1)
        return false;

      #if defined(HAVE_HWSERIAL1)&&defined(HAVE_HWSERIAL2)&&defined(HAVE_HWSERIAL3)
        HardwareSerial* serial=&Serial3;
  
        if(serial_channel==1)
          serial=&Serial1;
        else if(serial_channel==2)
          serial=&Serial2;
  
        serial->begin(115200);
  
        roomba.set_serial(*serial);
        roomba.setup(wake_pin);
  
        return true;
      #endif

      return false;
    }

    inline std::string key() const
    {
      return "r";
    }

    inline std::string on_get(roomba_t& roomba)
    {
      return "{\"enc\":{\"l\":"+std::to_string((uint32_t)roomba.get_sensors().encoder.L)+",\"r\":"+std::to_string((uint32_t)roomba.get_sensors().encoder.R)+"}}";
    }

    inline void on_set(roomba_t& roomba,json_ro_t& json,const std::string& path)
    {
      roomba.drive(std::stol(json[path+".l"]),std::stol(json[path+".r"]));
    }

    inline void on_loop(roomba_t& roomba)
    {
      roomba.update();
    }
};

#endif
