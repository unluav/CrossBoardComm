#include "stdint.h"

#define GL_MESSAGE_MAX_LEN 150

typedef enum GLMessageCode {
  directions
} GLMessageCode;

typedef struct {
  int roll;
  int pitch;
  int yaw;
} DirectionsPayload;

typedef union GLPayload {
  DirectionsPayload directions;
  uint8_t data[GL_MESSAGE_MAX_LEN];
} GLPayload;

typedef struct GLMessage {
  GLMessageCode code;
  uint8_t len;
  GLPayload payload;
  uint8_t checksum;
} GLMessage;

// This populated the data array with the actual message to send
// it returns the length of the data array and set the checksum of
// the message passed in
int GLSendMessage(GLMessage* message, uint8_t* data);

// This will take an array of input bytes and map them into the message struct
// it returns 0 if the message is valid and -1 if it isn't
int GLRecieveMessage(uint8_t* data, GLMessage* message);

// This will check if the message is valid by running a checksum on the message
int GLValidMessage(GLMessage* message);
