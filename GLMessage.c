#include "GLMessage.h"

#define START_CHAR 0
#define END_CHAR 1
#define REPLACE_CHAR 2
#define START_REPLACE_CHAR 3
#define END_REPLACE_CHAR 4
#define REPLACE_REPLACE_CHAR 5

#define MAX_PACKET_LENGTH 300

void addToMessage(uint8_t* message, uint8_t c, int* message_index);
int receiveChar(uint8_t* data, int index, uint8_t* destination);

int GLSendMessage(GLMessage* message, uint8_t* data){
  uint8_t* payload = message->payload.data;
  uint8_t checksum = message->code + message->len;
  int index = 0;

  data[index++] = START_CHAR;
  addToMessage(data, message->code, &index);
  addToMessage(data, message->len, &index);

  for(int i = 0; i < message->len; i++){
    checksum += payload[i];
    addToMessage(data, payload[i], &index);
  }

  message->checksum = 256 - checksum;
  addToMessage(data, message->checksum, &index);
  data[index++] = END_CHAR;
  return index;
}

int GLRecieveMessage(uint8_t* data, GLMessage* message){
  int data_index = 1;
  data_index += receiveChar(data, data_index, (uint8_t*) &message->code);
  data_index += receiveChar(data, data_index, &message->len);

  int message_index = 0;
  while(message_index < message->len){
    data_index += receiveChar(data, data_index, message->payload.data + message_index++);
  }
  data_index += receiveChar(data, data_index, &message->checksum);

  return GLValidMessage(message);
}

int GLValidMessage(GLMessage* message){
  uint8_t checksum = message->code + message->len + message->checksum;
  for(int i = 0; i < message->len; i++){
    checksum += message->payload.data[i];
  }
  if(checksum !=0)
    return -1;
  else
    return 0;
}

int receiveChar(uint8_t* data, int index, uint8_t* destination){
  if(data[index] == REPLACE_CHAR){
    switch (data[++index]) {
      case START_REPLACE_CHAR:
        *destination = START_CHAR;
        break;
      case END_REPLACE_CHAR:
        *destination = END_CHAR;
        break;
      case REPLACE_REPLACE_CHAR:
        *destination = REPLACE_CHAR;
        break;
    }

    return 2;
  } else {
    *destination = data[index];
    return 1;
  }
}

void addToMessage(uint8_t* message, uint8_t c, int* message_index){
  switch (c) {
    case START_CHAR:
      message[*message_index++] = REPLACE_CHAR;
      message[*message_index++] = START_REPLACE_CHAR;
      break;
    case END_CHAR:
      message[*message_index++] = REPLACE_CHAR;
      message[*message_index++] = END_REPLACE_CHAR;
      break;
    case REPLACE_CHAR:
      message[*message_index++] = REPLACE_CHAR;
      message[*message_index++] = REPLACE_REPLACE_CHAR;
      break;
    default:
      message[*message_index++] = c;
  }
}
