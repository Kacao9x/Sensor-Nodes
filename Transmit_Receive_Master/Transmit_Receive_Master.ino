#include <SPI.h>
#include "RF24.h"
#include "DHT.h"


const int Max_Nodes = 30;
byte Received_ID_Tags[Max_Nodes]; //write max number of Nodes. 
#define DHTTYPE DHT11   // DHT 11
#define DHTPIN 5
DHT dht(DHTPIN, DHTTYPE);

RF24 radio(7,8);
const uint64_t pipe = 0xE8E8F0F0E1LL; //channel to receive
byte addresses[][6] = {"1Node","2Node"};
//unsigned long msg;

typedef struct {
  byte ID;
  float sensor1;
  byte GoToSleep;
}MsgData;

//Initializing the Data in Structs.
//These can be altered Later by using Struct_name.Struct_access 
MsgData Received_Data = {Max_Nodes + 1, 0, 0};
MsgData My_Data = {1, 33, 0};
//MsgData My_Data = {2, 98, 0};

  int k = 0;                                      //Initialize
  int j = 0;
  unsigned long long first_time = 0;
bool Once = false;
bool flag = 0;
int time_packet = 0;

void setup(void){
    Serial.begin(9600);
    radio.begin();
    radio.setAutoAck(false);
    radio.openReadingPipe(1,pipe);
    radio.startListening();
    dht.begin();
}

void loop(void){
   // My_Data.sensor1 = dht.readHumidity();
  // Read temperature as Celsius (the default)
  //float t = dht.readTemperature();
  // Read temperature as Fahrenheit (isFahrenheit = true)


  while(Once == false){                                   //Run one time
    for(j = 0; j <= Max_Nodes; j++){                   //Preset all known IDs to 0 to begin checking for IDs. 
        Received_ID_Tags[j] = 0;
      } 
    receive_time_packet();
    if(time_packet > 0){                            //if we have a time_packet from the home node
      Serial.print("Time Packet:");
      Serial.println(time_packet);
      first_time = millis();                        //Save the time of Package Receive
      time_packet = time_packet - 1;                //subtract 1ms from time_packet to make accurate for future nodes
      transmit_time_packet(time_packet);            //transmit packet for other leafnodes
        Once = true;
          My_Data.sensor1 = dht.readTemperature(true);
      }
  }


    while(time_packet + first_time > millis()){   //wait until time_packet is completed
      //wait until we hit 'synchronization time'
    }

      
      //we are now in sync, begin program
      if((millis() >= ((My_Data.ID*2000) + first_time + 500)) && (millis() < (My_Data.ID*2000) + first_time + 1000)){ //amount of time to wait until we switch out of receiving mode. 1 seconds
        //A buffer of 500 is added     right here      ^---^
        //Transmit My_Data
        transmit(My_Data);
        Serial.print("Transmitting My_Data: ");
        Serial.println(My_Data.ID);
      }
      else{
        //Receive_Data
        receive();
      }
      for(j = 0; j <= Max_Nodes; j++){ 
        if(Received_Data.ID == Received_ID_Tags[j] || Received_Data.ID > Max_Nodes){ // if we have seen it, set flag to 1.
            flag = 1;
            break;
          }
      }
        
        if(flag !=1){                                                            //New Node, Transmit and record Node ID
           transmit(Received_Data);
           Received_ID_Tags[k]= Received_Data.ID;
           Serial.println("New Node");
           Serial.print("ID: ");
           Serial.println(Received_Data.ID);
           Serial.print("Sensor Data: ");
           Serial.println(Received_Data.sensor1);
           Serial.println("Node Echo Complete");
           Serial.println();
           k++;
        }
        j = 0;                                                                    //Reset Variables for each iteration
        flag = 0;
    
        reset();
              
              
  }



void reset(){
     if(Received_Data.ID == 0 && Received_Data.GoToSleep == 22){               //Reset everything & go to sleep
        transmit(Received_Data);
       for(j = 0; j <= Max_Nodes; j++){                   //Preset all known IDs to 0 to begin checking for IDs. 
         Received_ID_Tags[j] = 0;
       } 
        Once = false;
        flag = 0; 
        j = 0; 
        k = 0;
        first_time = 0;
        time_packet = 0;
        Received_Data = {Max_Nodes + 1, 0, 0};
        Serial.println("Reset Node");        
      }

  
}
        
void receive(){                                                             //Recieve Data from another node
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

void transmit(MsgData Transmit_Msg){                                        //Transmit Data to Another Node
    radio.openWritingPipe(addresses[0]);
    radio.openReadingPipe(1,addresses[1]);
    radio.stopListening();
    //unsigned long msg = value;
  for(byte i=0; i<15; i++){  
        radio.write(&Transmit_Msg, sizeof(MsgData));
        delay(5);
  }
}

void transmit_time_packet(int Time){                                      //Transmit a Time Packet
    radio.openWritingPipe(addresses[0]);
    radio.openReadingPipe(1,addresses[1]);
    radio.stopListening();
    //unsigned long msg = value;
  for(byte i=0; i<15; i++){  
        radio.write(&Time, sizeof(time_packet));
        delay(5);
  }
}

void receive_time_packet(){                                             //Recieve a Time Packet
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
