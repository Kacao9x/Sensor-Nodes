#include <SPI.h>
#include "RF24.h"

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

//Tim's defined variables
int TransAMOUNT=5;
int awake=true; 
int node_name=5;
int sleep_amount=2000;
int history=NULL;
//Reserved: Node 0, Data ID -1,


void setup(void){
    Serial.begin(9600);
    radio.begin();
    radio.setAutoAck(false);
    radio.openReadingPipe(1,pipe);
    radio.startListening();
}





void loop(void){
  delay(10000);     
  //My_Data=sensorREAD();                   //take sensor reading
  recieve();                                //wait for incoming data to enter loop
  
  for(;;){
    awake=true;
    //My_Data=sensorREAD();                   //take sensor reading
    
    while(awake){
      recieve();
      //if recieved data
      if(Recieved_Data.sensor1==node_name){        //if you => send your data
        transmit(My_Data);
        Serial.println("sending_msg");
      }
      
      else if(Recieved_Data.sensor1!=history){     //if not you => forward data
          MsgData fwd_data={1, 2, 5};
          transmit(fwd_data);            
          history=Recieved_Data.sensor1;
          Serial.println("forwarding_msg");
      }
      
      if(Recieved_Data.ID==-1){                //sleep command
          awake=false;
          sleep_amount=Recieved_Data.sensor1; //set timmer for sleep amount
       }        
    }
    history=NULL;                             //reset history for the day
    delay(sleep_amount);                      //enter low power sleep mode   
  }
}









void recieve(){
    radio.openWritingPipe(addresses[1]);
    radio.openReadingPipe(1,addresses[0]);
    radio.startListening();  
       if(radio.available()){ 
            radio.read(&Recieved_Data, sizeof(MsgData));  //byte value
            delay(5);
            Serial.println(Recieved_Data.sensor1);
      }
      else{
      Serial.println("NULL");
      }
    return;
}

void transmit(MsgData Transmit_Msg){
    radio.openWritingPipe(addresses[0]);
    radio.openReadingPipe(1,addresses[1]);
    radio.stopListening();
    //unsigned long msg = value;
  for(byte i=0; i<TransAMOUNT; i++){  
        radio.write(&Transmit_Msg, sizeof(MsgData));
        delay(5);
  }
}

