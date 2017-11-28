#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "msgs.h"
#define MAX_CLIENTS 3
#define SERVER_PORT 0

#define SENDING_FILENAME          0
#define SENDING_FILECONTENT       1
#define FILE_SIZE_VIOLATION       2
#define CLIENT_LIMIT_VIOLATION    3
#define FILENAME_LIMIT_VIOLATION  4
#define SUCCESSFULLY_SENT         5

void server(char *charArr);
void client(char *fileLocation);

int clientId = 0;
int clientPort = 1;

// int activeClients = 0;

semaphore_t *globalAccess;

void clearBuffer(char *buffer)
{
  for ( int y = 0 ; y < MESSAGES ; y++)
  {
    buffer[y] = ' ';
  }
}

int isEmpty(char *buffer)
{
  int x = 0;
  for (int iter = 0 ; iter < 10 ; iter++)
  {
    if (buffer[iter] == ' ')
    {
      x++;
    }
  }

  if(x == MESSAGES)
  {
    return 1;
  }
  return 0;
}

void client(char *fileLocation)
{

  int Id,portVar;

  P(globalAccess);
    Id   = clientId++;
    portVar = clientPort++;
  V(globalAccess);
  char    dataBuffer[MESSAGES];
  FILE    *fd;
  int     content              = 0;
  short   fileSizeViolation    = 0;
  short   clientLimitViolation = 0;
  short   fileNameViolation    = 0;
  int     idx                  = 0;
  int     fileNameSize         = strlen(fileLocation);
  message *mess = (message *)malloc(sizeof(message));

  mess->metaData[0]           = portVar;
  mess->metaData[1]           = SENDING_FILENAME;

  int     dataLimit           = MESSAGES;

  while (idx < fileNameSize)
  {
    dataBuffer[idx%dataLimit] = fileLocation[idx];

    if (idx!=0 && (idx % (dataLimit-1))==0)
    {
      for (int nIter = 0 ; nIter < dataLimit ; nIter++)
      {
        mess->m[nIter]        = dataBuffer[nIter];
      }

      printf("\n Client # %d sending data on port %d",Id,SERVER_PORT);
      send(SERVER_PORT,*mess);
      message received       = receive(portVar);

      if (received.metaData[1] == CLIENT_LIMIT_VIOLATION)
      {
        printf("\n Client # %d: Server limit exceeded!",Id);
        clientLimitViolation    = 1;
        break;
      }
      if (received.metaData[1] == FILENAME_LIMIT_VIOLATION)
      {
        printf("\n Client # %d: File Name Limit Violation!",Id);
        fileNameViolation    = 1;
        break;
      }

      printf("\n Client # %d: File Name successfully sent",Id);
      clearBuffer(dataBuffer);
    }

    ///////////////////////////////////////
    if (idx == fileNameSize-1)
    {
      if (isEmpty(dataBuffer) == 0)
      {
        for ( int z = 0;z < ((idx+1)%dataLimit) ; z++)
        {
          mess->m[z] = dataBuffer[z];
        }
        mess->m[(idx+1)%dataLimit] = '\0';
        clearBuffer(dataBuffer);
      }
      else
      {
        mess->m[0] = '\0';
      }

      printf("\n Client # %d: sending file name",Id);
      send(SERVER_PORT,*mess);
      message response = receive(portVar);

      if (response.metaData[1] == CLIENT_LIMIT_VIOLATION)
      {
        printf("\n Client # %d: Client limit exceeded",Id);
        clientLimitViolation = 1;
        break;
      }
      if (response.metaData[1] == FILENAME_LIMIT_VIOLATION)
      {
        printf("\n Client # %d: File Name Violation",Id);
        fileNameViolation = 1;
        break;
      }

      printf("\n Client # %d: file name sent",Id);

    }
    idx++;
  }


  if (clientLimitViolation != 1 || fileNameViolation != 1)
  {
    FILE *fp = fopen(fileLocation,"rb");
    char c;
    mess->metaData[1] = SENDING_FILECONTENT;
    idx = 0;

    while ((c = fgetc(fp)) != EOF)
    {
      dataBuffer[idx] = c;
      if (idx==MESSAGES-1)
      {
        for ( int z = 0 ; z < MESSAGES ; z++)
        {
          mess->m[z] = dataBuffer[z];
        }
        printf("\n Client # %d: sending file contents on port %d",Id,SERVER_PORT);
        send(SERVER_PORT,*mess);
        message response = receive(portVar);

        if (response.metaData[1] == FILE_SIZE_VIOLATION)
        {
          printf("\n File size overshoot... \n");
          fileSizeViolation = 1;
          break;
        }
        clearBuffer(dataBuffer);
        idx = 0;
        continue;
      }
      idx++;
    }

    if(fileSizeViolation!=1)
    {
      printf("\n Client # %d sent file on port %d",Id,SERVER_PORT);
      dataBuffer[idx++] = '\0';
      for (int x = 0 ; x < idx ; x++)
      {
        mess->m[x] = dataBuffer[x];
      }
      // printf("\n Client # %d sent EOF")
      send(SERVER_PORT,*mess);
      message rMess = receive(portVar);
    }
    fclose(fp);
  }

  free(mess);

  while(1)
  {
    yield();
  }

}

void server(char *charArr)
{

  char files[MAX_CLIENTS][50];
  int ports[MAX_CLIENTS];

  // voiding variables
  for (int i=0 ; i<MAX_CLIENTS ; i++){
    for (int j=0; j<30 ; j++){
      files[i][j] = ' ';
    }
    // voiding ports initially
    ports[i] = -1;
  }

  // Violation checking variables
  short fileSizeViolation    = 0;
  short clientLimitViolation = 0;
  short fileNameViolation    = 0;

  int iterator      = 0;
  int fileIdx       = 0;
  short isPortFound = 0;

  while(1){
    // resetting violation variables
    fileSizeViolation    = 0;
    clientLimitViolation = 0;
    fileNameViolation    = 0;

    //printf(" Server # %d Listening on port # %d ",currentThreadId(),SERVER_PORT);
    message receivedMessage = receive(SERVER_PORT);

    clientPort              = receivedMessage.metaData[0];
    printf("\n Client Port # %d \n",clientPort);

    // reset variables
    iterator    = 0;
    fileIdx     = 0;
    isPortFound = 0;
    int i       = 0;

    for (i=0; i<MAX_CLIENTS ; i++){
      if ( ports[i] == clientPort ){
        fileIdx     = i;
        isPortFound = 1;
        break;
      }
    }

    if(isPortFound == 0){
      for ( i=0 ; i<MAX_CLIENTS ; i++){
        if(ports[i] == -1){
          fileIdx = i;
          ports[i] = clientPort;
          break;
        }
      }
    }

    if (i==MAX_CLIENTS)
    {
      clientLimitViolation = 1;
    }
    else if(receivedMessage.metaData[1] == 0)
    {
      // when file name is sent
      int z = 0;

      while(files[fileIdx][z] != ' ')
      {
        z++;
      }
      int x=0;
      for( x=0 ; x<10 && receivedMessage.m[x] != '\0' ; x++)
      {
        if(z > 14)
        {
          fileNameViolation = 1;
          break;
        }
        files[fileIdx][z++] = receivedMessage.m[x];
      }

      if(fileNameViolation!=1)
      {
        if(x!=10 && receivedMessage.m[x] == '\0')
        {
          files[fileIdx][z] = '\0';
          strcat(files[fileIdx],".server");
        }
      }

    }
    else if(receivedMessage.metaData[1] == 1)
    {
      // when file data is sent
      char name[50];
      FILE *fp = fopen(files[fileIdx],"ab");
      for(int iter=0 ; iter<10 ; iter++)
      {
        if(receivedMessage.m[iter] == '\0')
        {
          for(int z1=0 ; z1<50 ; z1++)
          {
            files[fileIdx][z1] = ' ';
          }
          ports[fileIdx] = -1;
          break;
        }
        else
        {
          if(ftell(fp) > 1000000)
          {
            fileSizeViolation = 1;
            break;
          }
          fputc((char)receivedMessage.m[iter],fp);
        }
      }
      fclose(fp);
      name[0] = '\0';
    }

  message *sendMessage = (message *)malloc(sizeof(message));
  char *response = "sent";
  sendMessage->metaData[0]  = SERVER_PORT;
  if(fileSizeViolation == 1)
  {
    sendMessage->metaData[1] = FILE_SIZE_VIOLATION;
  }
  else if(clientLimitViolation == 1)
  {
    sendMessage->metaData[1] = CLIENT_LIMIT_VIOLATION;
  }
  else if(fileNameViolation == 1)
  {
    sendMessage->metaData[1] = FILENAME_LIMIT_VIOLATION;
  }
  else
  {
    sendMessage->metaData[1] = SUCCESSFULLY_SENT;
  }

  int newIter = 0;
  for(newIter = 0; response[newIter] != '\0' ; newIter++)
  {
    sendMessage->m[newIter] = response[newIter];
  }
  sendMessage->m[newIter] = '\0';
  printf("\n Server sending data on port # %d \n",receivedMessage.metaData[0]);
  send(receivedMessage.metaData[0],*sendMessage);
  free(sendMessage);

  }
}

int main(int argc, char *argv[])
{
  globalAccess = CreateSem(1);
  int clients = 0;
  createQueue();

  if (argc>1){
    initPort();
    clients = atoi(argv[1]);
    printf("\n # of Clients: %d \n",clients);
    for( int i=2 ; i<argc ; i++)
    {
      printf("%s \t",argv[i]);
      start_thread(&client,argv[i]);
    }
    start_thread(&server,NULL);
    run();
  }
  return 0;
}
