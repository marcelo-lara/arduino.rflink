#pragma once
#include "Arduino.h"

struct Channel {
public:
  u8  id;       //channel
  u8  node;     //node
  u32 payload;  //value
};

class Channels {
public:
  Channel channelList[]{};
};