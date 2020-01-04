#pragma once
#include "Arduino.h"
#include "RFsettings.h"
#include "RFcodec.h"
#include "QueueArray.h"
#include "Channels.h"

struct RfPacket{
  RfPacket(Packet packet, unsigned long txPacket){
    this->packet=packet;
    this->txPacket=txPacket;
  };
  unsigned long txPacket;
  long sendTs = 0;
  int  retry  = 0;

  Packet packet;
};

typedef void (*RFrxCallback)(Packet _Packet);
typedef void (*RFfailCallback)(Packet _Packet);

class RFlink{
public:
    RFlink(RFrxCallback rxEvent, RFfailCallback failEvent);
    void setup();
    void update(); 
    bool ready;
    
    //tx commands
    bool send(Packet dev);
    RFcodec* codec;

private:
    QueueArray<RfPacket> sendQueue;
    void deliver();
    unsigned long deviceEncode(Packet pkt);
    RFrxCallback raiseRxEvent;
    RFfailCallback raiseFailCallback;
};
