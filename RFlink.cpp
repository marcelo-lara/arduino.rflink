#include "RFlink.h"
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>


#define rf_ce   2 //D4
#define rf_csn 15 //D8
RF24 radio(rf_ce,rf_csn);

RFlink::RFlink(RFrxCallback rxEvent, RFfailCallback failEvent){
    raiseRxEvent = rxEvent;
    raiseFailCallback = failEvent;
}

//setup nRF24L01 
void RFlink::setup(){
    ready=false;
    radio.begin();
    radio.setPALevel(RF24_PA_HIGH); //RF24_PA_HIGH | RF24_PA_LOW | RF24_PA_MAX
    radio.setDataRate(RF24_1MBPS);
    radio.enableDynamicAck();
    radio.setCRCLength(RF24_CRC_8);
    radio.setChannel(rfChannel);
    radio.setRetries(20, 10);  

    //pipes setup
    radio.openWritingPipe(rf_server_tx);
    radio.openReadingPipe(1, rf_office_tx);
    radio.openReadingPipe(2, rf_suite_tx);
    radio.openReadingPipe(3, rf_living_tx);

    //debug
    ready = (rfChannel == radio.getChannel()); //test if radio is enabled
    //radio.printDetails();
    Serial.println(ready?"rf online":"rf offline");
}

//handle received packets -> raise RF
void RFlink::update(){

	//if radio is not enabled, discard anything
	if (!ready) return;

  //check receive buffer, otherwise, proceed to deliver
	uint8_t _radioNode;
	if (!radio.available(&_radioNode))
    return this->deliver();

	//get payload
	unsigned long _radioPayLoad;
	while (radio.available()) {
		radio.read(&_radioPayLoad, sizeof(unsigned long));
	}

	//prepare for next packet
	radio.startListening();

	//decode payload
	Packet dev = codec->decode(_radioPayLoad, _radioNode);
  Serial.printf("RF<-%X|%X\n", _radioNode, _radioPayLoad);

  //delegate
  raiseRxEvent(dev);
}

bool RFlink::send(Packet dev){
  sendQueue.enqueue(RfPacket(dev, deviceEncode(dev)));
  return true;
}

void RFlink::deliver(){
	//if radio is not enabled, discard anything<
	if (!ready) return;
  if(sendQueue.isEmpty()) return;

  //get next packet to deliver
  RfPacket rfPacket = sendQueue.dequeue();
  if(rfPacket.sendTs>millis())
    return sendQueue.enqueue(rfPacket);

	//open write pipe
	uint64_t writeAddress;
  switch (rfPacket.packet.node) {
		case office_node: writeAddress = rf_office_rx; break;
		case suite_node: writeAddress = rf_suite_rx; break;
		case living_node: writeAddress = rf_living_rx; break;
  };
  

  //rf send
  radio.stopListening();
  radio.openWritingPipe(writeAddress);
  bool result = radio.write(&rfPacket.txPacket, sizeof(unsigned long), 0);
  radio.startListening();
  Serial.printf("RF->%i|%X\t%s\n", rfPacket.packet.node, rfPacket.txPacket, result?"ok":"error");

  //retry handling
  if(!result){
    rfPacket.sendTs = millis() + 500;
    Serial.print("\t\tretry ");
    rfPacket.retry++;
    if(rfPacket.retry<3){
      Serial.println(rfPacket.retry);
      sendQueue.enqueue(rfPacket);}
    else{
      Serial.println("drop");
      raiseFailCallback(rfPacket.packet);
    }
  }
};

unsigned long RFlink::deviceEncode(Packet pkt){
	unsigned long retVal = 0xD;
	retVal = (retVal << 4) + pkt.cmd;
	retVal = (retVal << 8) + pkt.id;
	retVal = (retVal << 16) + pkt.payload;
	return retVal;
}