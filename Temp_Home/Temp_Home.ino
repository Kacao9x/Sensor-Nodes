#include <SPI.h>
#include "RF24.h"

RF24 radio(7,8);
const uint64_t pipe = 0xE8E8F0F0E1LL; //channel to recieve
byte addresses[][6] = {"1Node","2Node"};
//unsigned long msg;
const byte NodeID = 0;
float NodeData = 0;
const int Max_Nodes = 20;
typedef struct {
  byte ID; //Node ID number
  byte path [Max_Nodes]; //The path to go down    //up to 256 differnt node names only a path of 31
  byte Place_In_Path; //Where in the array are we
  byte cmd; //go to sleep, other odd commands
  bool return_flag;//Return to home node, go from ++ to --
  float sensor1;
}MsgData;

  MsgData My_Data;
  MsgData Received_Data;

   //byte path[Max_Nodes];
//Initializing the Data in Structs.
//These can be altered Later by using Struct_name.Struct_access 

int TransAMOUNT=15;
int DataTRANS=false;
int i;
int Timeout=500;
int count=0;
int start_time;


void setup(void){
    for( i=0; i<Max_Nodes; i++){
      My_Data.path[i] = 0;
    }
     My_Data.return_flag=0;
     My_Data.Place_In_Path=1;

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
 My_Data.Place_In_Path=1;

 transmit(My_Data);
 start_time=millis();
 if(DataTRANS){
  Serial.println("---Listening For Response---");
 }
 while(DataTRANS){   //recieve until hear response from different direction same ID

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
        My_Data.ID=0;
        My_Data.path[0] = 0;
        My_Data.path[1] = 1;
        DataTRANS=true;
        }
        if(temp1==LOW){
        My_Data.ID=2;
        for(i=0;i<Max_Nodes;i++){
          My_Data.path[i] = i;
        }
        DataTRANS=true;
        }
        if(temp2==LOW){
        My_Data.ID=2;
        My_Data.path[0] = 0;
        My_Data.path[1] = 1;
        My_Data.path[2] = 2;
        DataTRANS=true;
        }
        if(temp==HIGH&&temp1==HIGH&&temp2==HIGH){
          return;
        }
        for(i=0;i<Max_Nodes;i++){
          Serial.print(My_Data.path[i]);
        }
        radio.write(&My_Data, sizeof(MsgData));
        delay(5);
        Serial.print("sending_request: ");Serial.println(My_Data.ID); Serial.println(My_Data.return_flag);
  }
}

