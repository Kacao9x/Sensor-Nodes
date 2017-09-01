#include <SPI.h>
#include "RF24.h"

const int Max_Nodes = 30;
byte Received_ID_Tags[Max_Nodes]; //write max number of Nodes. 

RF24 radio(7,8);
const uint64_t pipe = 0xE8E8F0F0E1LL; //channel to receive
byte addresses[][6] = {"1Node","2Node"};
//unsigned long msg;
int time_packet = 0;

typedef struct {
  byte ID;
  float sensor1;
  byte GoToSleep;   //SET TO 1324 TO TELL LEAFNODES TO GO TO SLEEP
}MsgData;

//Initializing the Data in Structs.
//These can be altered Later by using Struct_name.Struct_access 
MsgData Received_Data = {Max_Nodes + 1, 0, 0};                     //Set ID number to more than the number of nodes utilized. 
MsgData My_Data = {0, 0, 22};
MsgData Leaf_Node_Data[Max_Nodes] = {0, 0, 0};

int inPin = 9;   // choose the input pin (for a pushbutton)
int val = 0;     // variable for reading the pin status
bool flag = 0;
int k = 0;
int j = 0;
bool Once = false;
void setup(void){
    Serial.begin(9600);
    radio.begin();
    radio.setAutoAck(false);
    radio.openReadingPipe(1,pipe);
    radio.startListening();
    pinMode(inPin, INPUT);
}


void loop(void){
  if(Once == false){                                   //Run one time
    for(j = 0; j <= Max_Nodes; j++){                   //Preset all known IDs to 0 to begin checking for IDs. 
        Received_ID_Tags[j] = 0;
      }
      Once = true;
  }
      
  val = digitalRead(inPin);                            // read input value
  if (val == HIGH) {                                   // check if the input is HIGH (button released)
      reset_auto();                                    //Reset the Nodes and Home node settings.
      delay(50);                                       //Pause before requesting
      transmit_time_packet(1000);
      Serial.println("****Button Pressed****");
  } 
  else {}

  receive();
        for(j = 0; j <= Max_Nodes; j++){
          if(Received_Data.ID == Received_ID_Tags[j] || Received_Data.ID > Max_Nodes){ // if we have seen it, set flag to 1.
            flag = 1;
            break;
          }
        }
        if(flag !=1){                                                            //if we have not seen it, set flag to 0.
           Leaf_Node_Data[k] = Received_Data;
           Received_ID_Tags[k]= Received_Data.ID;
           Serial.print("Node ID: ");
           Serial.println(Leaf_Node_Data[k].ID);
           Serial.print("Sensor Data: ");
           Serial.println(Leaf_Node_Data[k].sensor1);
           Serial.println();
           k++;
        }
        
      
    j = 0;
    flag = 0;
    
    reset();
}

void receive(){
    radio.openWritingPipe(addresses[1]);
    radio.openReadingPipe(1,addresses[0]);
    radio.startListening();  
       if(radio.available()){
        while(radio.available()){   
            radio.read(&Received_Data, sizeof(MsgData));  //byte value
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
        radio.write(&Transmit_Msg, sizeof(MsgData));
        delay(5);
  }
}

void transmit_time_packet(int Time){
    radio.openWritingPipe(addresses[0]);
    radio.openReadingPipe(1,addresses[1]);
    radio.stopListening();
    //unsigned long msg = value;
  for(byte i=0; i<15; i++){  
        radio.write(&Time, sizeof(time_packet));
        delay(5);
  }
}

void receive_time_packet(){
    radio.openWritingPipe(addresses[1]);
    radio.openReadingPipe(1,addresses[0]);
    radio.startListening();  
       if(radio.available()){
        while(radio.available()){   
            radio.read(&time_packet, sizeof(time_packet));  
            delay(5);
         }
      }
    return;
}

void reset(){
  if ( Serial.available() )
  {
    char c = toupper(Serial.read());
     Serial.println(F("*****SYSTEM RESET*****"));
     if(c == 'R'){
      transmit(My_Data);
       val = 0;     // variable for reading the pin status
       flag = 0;
       k = 0;
       j = 0;
       c = 0;
       Once = false;
       Received_Data = {Max_Nodes + 1, 0, 0};
//           for(j = 0; j <= Max_Nodes; j++){                   //Preset all known IDs to 0 to begin checking for IDs. 
//        Received_ID_Tags[j] = 0;
//      }
      for(j = 0; j < Max_Nodes; j++){                   //Preset all known IDs to 0 to begin checking for IDs. 
        Leaf_Node_Data[j] = {0,0,0};
      }
     }
  }
}

void reset_auto(){
       transmit(My_Data);
       val = 0;     // variable for reading the pin status
       flag = 0;
       k = 0;
       j = 0;
       Once = false;
       Received_Data = {Max_Nodes + 1, 0, 0};
//           for(j = 0; j <= Max_Nodes; j++){                   //Preset all known IDs to 0 to begin checking for IDs. 
//        Received_ID_Tags[j] = 0;
//      }
      for(j = 0; j < Max_Nodes; j++){                   //Preset all known IDs to 0 to begin checking for IDs. 
        Leaf_Node_Data[j] = {0,0,0};
      }
}


