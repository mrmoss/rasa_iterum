#include <Servo.h>//WHY I NEED YOU?!
#include "communications.h"
#include "packetize.h"

parser_t parser;

void setup()
{
    Serial.begin(57600);
}

void loop()
{
    std::string str(parser.parse(Serial));

    if(str.size()>0)
    {
        json_ro_t json(str);
        Serial.println(str.c_str());
        communications::receive(json);
    }

    communications::loop();
}
