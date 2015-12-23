#ifndef PERIPHERAL_LIST_H
#define PERIPHERAL_LIST_H

#include <Arduino.h>
#include "list.h"
#include "string.h"

template<typename T> class peripheral_list_t
{
  public:
    list_t<T> list;

    inline virtual ~peripheral_list_t()
    {}

    inline virtual uint32_t max_count() const
    {
      //return max possible objects that can be stored
      return 0;
    }

    inline virtual void on_remove(T& peripheral)
    {
      //cleanup for the peripheral (destructor is called for you...)
    }

    inline virtual bool on_add(T& peripheral,json_ro_t& json,const std::string& path)
    {
      //i make a new peripheral, but you set it up with the json string received
      //return true if it is a good object, return false if the json string was invalid and i won't actually make one
      return false;
    }

    inline virtual std::string key() const
    {
      //what is the key for your json array?
      return "im_a_key";
    }

    inline virtual std::string on_get(T& peripheral)
    {
      //return json str
      return "";
    }

    inline virtual void on_set(T& peripheral,json_ro_t& json,const std::string& path)
    {
      //set value to the passed json object path
    }

    inline virtual void on_loop(T& peripheral)
    {
      //general looping
    }
};

#endif
