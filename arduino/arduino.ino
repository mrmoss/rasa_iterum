#include <Servo.h>//WHY I NEED YOU?!
#include "peripherals.h"
#include "packetize.h"

parser_t parser;

void setup()
{
    Serial.begin(115200);
}

void loop()
{
    std::string str(parser.parse(Serial));

    if(str.size()>0)
    {
        json_ro_t json(str);
        peripherals::loop(json);
    }
}
