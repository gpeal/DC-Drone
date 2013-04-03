#ifndef COMMUNICATION_H
#define COMMUNICATION_H

#include "SoftwareSerial.h"

#define XBEE_RX 8
#define XBEE_TX 9
#define MAX_PAYLOAD_LENGTH 256
// message parse errors
#define PARSE_ERROR 0
#define INCORRECT_ID_ERROR 1
// include enough space for the payload plus the to id, from id, message type, their delimiters
// plus another 7 just to be safe :)
#define MAX_MESSAGE_LENGTH MAX_PAYLOAD_LENGTH + 7 + 7
#define DELIMITER ';'


//TODO: make this a class with inheritance
typedef struct Message_t
{
  int to;
  int from;
  int type;
  char payload[MAX_PAYLOAD_LENGTH];
};

class Xbee
{
  static SoftwareSerial comm;
  // we can only read one character at a time from the xbee so
  // we incrementally save the results to a buffer
  static int input_buffer_index;
  static char input_buffer[MAX_MESSAGE_LENGTH];
  // keep track of what part of the message we are currently waiting for/reading
  enum MessageReadState
  {
    TO,
    FROM,
    TYPE,
    PAYLOAD
  };
  static MessageReadState message_read_state;

  static void setup(void);
  static Message_t *loop(void);
  static Message_t *parse_message(char *input);
};

#endif