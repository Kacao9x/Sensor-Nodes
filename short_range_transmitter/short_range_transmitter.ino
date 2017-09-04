#include <SPI.h>
#include "RF24.h"
#include "DHT.h"

RF24 radio(7,8);
const uint64_t pipe = 0xE8E8F0F0E1LL; //channel to recieve
byte addresses[][6] = {"00001","00002"};
//unsigned long msg;

typedef struct{
  byte Direction;
  int ID;
  float Data;
}package;


//Initializing the Data in Structs.
//These can be altered Later by using Struct_name.Struct_access 
package recievedData = {0, 0};
package myData = {1, 2, 1};
int TransAMOUNT=5;
int DataTRANS=false;
int i;
int Timeout=5000;

void setup(void){
    Serial.begin(9600);
    radio.begin();
    radio.setAutoAck(false);
    radio.openReadingPipe(1,pipe);
    radio.startListening();
    pinMode(4,INPUT_PULLUP);  
    pinMode(5,INPUT_PULLUP); 
    pinMode(6,INPUT_PULLUP);
}

void loop(void){
 DataTRANS=false;
 i=0;
 transmit(myData);
 while(DataTRANS){   //recieve until hear response from different direction same ID
    if(i==0){
      Serial.println("---Listening For Response---");
    }
    recieve();
    if((recievedData.Direction==0)&&(recievedData.ID==myData.ID)){
      Serial.print("Data recieved: "); Serial.print(recievedData.ID);Serial.print(", ");Serial.println(recievedData.Data);
      break;
    }
    if(i>Timeout){
      Serial.println("recieve timed out");
      break;
    }
    i++;
 }
}



void recieve(){
    radio.openWritingPipe(addresses[1]);
    radio.openReadingPipe(1,addresses[0]);
    radio.startListening();  
       if(radio.available()){ 
            radio.read(&recievedData, sizeof(package));  //byte value
            delay(5);
      }
    return;
}

void transmit(package myData){
    radio.openWritingPipe(addresses[0]);
    radio.stopListening();
  for(byte i=0; i<TransAMOUNT; i++){ 
        int temp= digitalRead(4);
        int temp1= digitalRead(5);
        int temp2= digitalRead(6);
        if(temp==LOW){
        myData = {1, 5, 223};
        DataTRANS=true;
        }
        if(temp1==LOW){
        myData = {1, 3, 32};
        DataTRANS=true;
        }
        if(temp2==LOW){
        myData = {1, -1, 800};
        DataTRANS=true;
        }
        if(temp==HIGH&&temp1==HIGH&&temp2==HIGH){
          return;
        }
        radio.write(&myData, sizeof(package));
        delay(5);
        Serial.print("sending_msg: ");Serial.print(myData.ID);Serial.print(", ");Serial.println(myData.Data);
  }
}

