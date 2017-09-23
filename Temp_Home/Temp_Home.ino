#include <SPI.h>
#include "RF24.h"

RF24 radio(7,8);
const uint64_t pipe = 0xE8E8F0F0E1LL; //channel to recieve
byte addresses[][6] = {"00001","00002"};
//unsigned long msg;
const byte NodeID = 1;
float NodeData = 420;
const int Max_Nodes = 256;
typedef struct {
  byte ID; //Node ID number
  int path [Max_Nodes]; //The path to go down
  byte Place_In_Path; //Where in the array are we
  byte cmd; //go to sleep, other odd commands
  bool return_flag;//Return to home node, go from ++ to --
  float sensor1;
}MsgData;

  MsgData My_Data;
  MsgData Received_Data;


//Initializing the Data in Structs.
//These can be altered Later by using Struct_name.Struct_access 

int TransAMOUNT=5;
int DataTRANS=false;
int i;
int Timeout=5000;

void setup(void){
    for( i=0; i<256; i++){
      My_Data.path[i] = -1;
    }
   
    My_Data.ID = NodeID;
    My_Data.sensor1 = NodeData;
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
 My_Data.return_flag=0;
My_Data.Place_In_Path=0;
 i=0;
 transmit(My_Data);
 while(DataTRANS){   //recieve until hear response from different direction same ID
    if(i==0){
      Serial.println("---Listening For Response---");
    }
    recieve();
    if((Received_Data.return_flag == 1)&&(Received_Data.path[Received_Data.Place_In_Path])){
      Serial.print("Data recieved: "); Serial.print(Received_Data.ID);Serial.print(", ");Serial.println(Received_Data.sensor1);
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
            radio.read(&Received_Data, sizeof(MsgData));  //byte value
            delay(5);
      }
    return;
}

void transmit(MsgData My_Data){
    radio.openWritingPipe(addresses[0]);
    radio.stopListening();
  for(byte i=0; i<TransAMOUNT; i++){ 
        int temp= digitalRead(4);
        int temp1= digitalRead(5);
        int temp2= digitalRead(6);
        if(temp==LOW){
        My_Data.path[0] = 0;
        My_Data.path[1] = 1;
        DataTRANS=true;
        }
        if(temp1==LOW){
        My_Data.path[0] = 0;
        My_Data.path[1] = 2;
        DataTRANS=true;
        }
        if(temp2==LOW){
        My_Data.path[0] = 0;
        My_Data.path[1] = 1;
        My_Data.path[1] = 2;
        DataTRANS=true;
        }
        if(temp==HIGH&&temp1==HIGH&&temp2==HIGH){
          return;
        }
        radio.write(&My_Data, sizeof(MsgData));
        delay(5);
        Serial.print("sending_msg: ");Serial.print(My_Data.ID);Serial.print(", ");Serial.println(My_Data.sensor1);
  }
}

