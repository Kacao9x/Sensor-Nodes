#include <SPI.h>
#include "RF24.h"
#include "DHT.h"

RF24 radio(7,8);
const uint64_t pipe = 0xE8E8F0F0E1LL; //channel to recieve
byte addresses[][6] = {"1Node","2Node"};
//unsigned long msg;

typedef struct {
  byte Priority;
  byte ID;
  float sensor1;
}MsgData;

//Initializing the Data in Structs.
//These can be altered Later by using Struct_name.Struct_access 
MsgData Recieved_Data = {0, 0};
MsgData My_Data = {1, 2, 1};

void setup(void){
    Serial.begin(9600);
    radio.begin();
    radio.setAutoAck(false);
    radio.openReadingPipe(1,pipe);
    radio.startListening();
}

void loop(void){
 transmit(My_Data);
//recieve();
 //Serial.println(Recieved_Data.sensor1);
  
}



void recieve(){
    radio.openWritingPipe(addresses[1]);
    radio.openReadingPipe(1,addresses[0]);
    radio.startListening();  
       if(radio.available()){
        while(radio.available()){   
            radio.read(&Recieved_Data, sizeof(MsgData));  //byte value
            delay(5);
         }
      }
    return;
}

void transmit(MsgData Transmit_Msg){
    radio.openWritingPipe(addresses[0]);
    radio.openReadingPipe(1,addresses[1]);
    radio.stopListening();
    //unsigned long msg = value;
  for(byte i=0; i<15; i++){ 
        int temp= digitalRead(4);
        if(temp==HIGH){
        My_Data = {1, 2, 5};
        }
        else{
          My_Data = {1, 2, 1};
        }
        radio.write(&Transmit_Msg, sizeof(MsgData));
        delay(5);
        
  }
}

