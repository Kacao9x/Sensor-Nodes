#include <SPI.h>
#include "RF24.h"

RF24 radio(7,8);  //CNS, CE pins
const uint64_t pipe = 0xE8E8F0F0E1LL; //channel to recieve
const byte addresses[][6] = {"00001","00002"};  //transmit / recieve pipe 
//unsigned long msg;

typedef struct{
  byte Direction;   //1= home to node, 0= node to home
  int ID;
  float Data;
}package;

//Initializing the Data in Structs.
//These can be altered Later by using Struct_name.Struct_access 

package recievedData={0,0};
package myData={0,5,20};    //0= node, ID=5, Data=20 


//Tim's defined variables
int TransAMOUNT=10;
int sleep_amount;
int IDhistory=NULL;
int DIRhistory=NULL;
//Reserved: Node 0, Data ID -1,


void setup(void){
    Serial.begin(9600);
    radio.begin();
    radio.setAutoAck(false);
    radio.openReadingPipe(1,pipe);
    radio.startListening();
}





void loop(void){
    //myData=sensorREAD();                   //take sensor reading
    recieve();
}



void recieve(){
    radio.openReadingPipe(1,addresses[0]);
    radio.startListening();  
       if(radio.available()){ 
            radio.read(&recievedData, sizeof(package));  //byte value
            delay(5);
            Serial.print(recievedData.ID);
            Serial.print(": ");
           Serial.println(recievedData.Data);
      }
      else{
      Serial.println("NULL");
      }
    return;
}

void transmit(package Transmit_Msg){
    radio.openWritingPipe(addresses[0]);
    radio.stopListening();
  for(byte i=0; i<TransAMOUNT; i++){  
        radio.write(&Transmit_Msg, sizeof(package));
        delay(5);
  }
}

