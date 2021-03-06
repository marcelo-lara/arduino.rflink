#include "Arduino.h"

//RF packet
struct Packet {
public:
  Packet(){};
  Packet(u8 id, u8 rfCommand, u32 payload, int node){
    this->id=id;
    this->cmd=rfCommand;
    this->payload=payload;
    this->node=node;
  };

	u8	 id      = 0; //channel id
	u8	 cmd     = 0; //rfcmd enum
	u32  payload = 0; //32b payload

	byte node    = 0; //rf station id
	u32  raw     = 0; //encoded packet

  void toString(){
    Serial.printf("[PKT|id:%i|payload:%lu]", this->id, (unsigned long)(this->payload));
  };
};

class RFcodec{
    public:
        unsigned long encode(int _cmd, int deviceId, unsigned long devicePayload);
    	  Packet decode(unsigned long rawData, unsigned long rfNodeStation);
        Packet decode(String str);


        unsigned long StrToHex(char str[]);
        unsigned long StrToHex(String str);
        
        unsigned long pressureEncode(float devicePayload);
        float pressureDecode(unsigned long devicePayload);
        unsigned long floatEncode(float devicePayload);
        float floatDecode(unsigned long devicePayload);
        
    private:
};