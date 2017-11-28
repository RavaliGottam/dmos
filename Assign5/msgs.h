#include "sem.h"
#define PORTS 10
#define MESSAGES 15

typedef struct message{
  int m[15];
  int metaData[2];
  // metaData[0] -> port Number
  // metaData[1] -> communication codes
  // Codes for metaData
  // 0 -> fileName
  // 1 -> fileContent
  // 2 -> file size Violation
  // 3 -> client limit Violation
  // 4 -> filename limit Violation
  // 5 -> successfully sent
}message;

typedef struct port{
  semaphore_t *full,*empty,*accesslock;
  message mess[MESSAGES];
  int inputPointer,outputPointer;
}port;

port portObject[PORTS];

// Intializes all the ports with appropriate values
void initPort();

// Used to send message to the intended port
void send(int intendedPort,message givenMessage);

// Used to receive message for the intended port
message receive(int intendedPort);


void initPort(){
  for (int i=0 ; i < PORTS ; i++){
      portObject[i].empty = CreateSem(MESSAGES);
      portObject[i].full = CreateSem(0);
      portObject[i].accesslock = CreateSem(1);
      portObject[i].inputPointer = 0;
      portObject[i].outputPointer = 0;
  }
}

message receive(int intendedPort){
  port *aport = &portObject[intendedPort];
  message temp;
  P(aport->full);
  P(aport->accesslock);
   if(aport->inputPointer == aport->outputPointer){
     printf("No Data \n");
   }else{
     temp = aport->mess[aport->outputPointer];
     //printf("\n nnnnnnnnnnnnnnnnnnnnnnnnn %d \n",temp.metaData[0]);
     aport->outputPointer =  (aport->outputPointer+1)%MESSAGES;
   }
  V(aport->accesslock);
  V(aport->empty);
  return temp;
}

void send(int intendedPort,message givenMessage){

  port *aport = &portObject[intendedPort];

  P(aport->empty);
  P(aport->accesslock);
  //printf("\n %d - PORT %d %d \n",intendedPort,givenMessage.metaData[0],aport->inputPointer);
    aport->mess[aport->inputPointer] = givenMessage;
    aport->inputPointer = (aport->inputPointer+1)%MESSAGES;
  V(aport->accesslock);
  V(aport->full);
}
