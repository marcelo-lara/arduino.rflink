#pragma once
#include "Arduino.h"
#include "RFsettings.h"
#include "RFcodec.h"
#include "Channels.h"

typedef void (*RFrxCallback)(Packet _Packet);

class RFlink{
public:
    RFlink(RFrxCallback rxEvent);
    void setup();
    void update(); 
    bool ready;
    
    //tx commands
    bool send(Packet dev);

private:
    unsigned long deviceEncode(Packet pkt);
    RFrxCallback raiseRxEvent;
};
