// Serial Monitor settings
#define SERIAL_EN      //comment this out when deploying to an installed SM to save a few KB of sketch size
#define SERIAL_BAUD    115200
#ifdef SERIAL_EN
  #define DEBUG(input)   Serial.print(input)
  #define DEBUGln(input) Serial.println(input)
  #define DEBUGFlush() Serial.flush()
#else
  #define DEBUG(input)
  #define DEBUGln(input)
  #define DEBUGFlush()
#endif
