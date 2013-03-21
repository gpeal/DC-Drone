#ifndef COMMUNICATION_H
#define COMMUNICATION_H

#include <SoftwareSerial.h>

#define XBEE_RX 8
#define XBEE_TX 9
#define MAX_PAYLOAD_LENGTH 256
#define DELIMITER ';'

typedef struct Message
{
  int to;
  int from;
  int type;
  char payload[MAX_PAYLOAD_LENGTH];
};

class Xbee
{
  static SoftwareSerial serial(XBEE_RX, XBEE_TX);
  // we can only read one character at a time from the xbee so
  // we incrementally save the results to a buffer
  static int input_buffer_index;
  // include enough space for the payload plus the to id, from id, message type, their delimiters
  // plus another 7 just to be safe :)
  static char input_buffer[MAX_PAYLOAD_LENGTH + 7 + 7];
  // keep track of what part of the message we are currently waiting for/reading
  enum
  {
    TO,
    FROM,
    TYPE,
    PAYLOAD
  } MessageReadState;

  static void setup(void);
  static Message loop(void);
  static Message parse_message(char *input);
};

#endif