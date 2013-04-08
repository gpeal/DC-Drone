#ifndef COMMUNICATION_H
#define COMMUNICATION_H
#include "SoftwareSerial.h"

#define XBEE_RX 9
#define XBEE_TX 10
#define MAX_PAYLOAD_LENGTH 32
// message parse errors
#define PARSE_ERROR 0
#define INCORRECT_ID_ERROR 1
// include enough space for the payload plus the to id, from id, message type, their delimiters
// plus another 7 just to be safe :)
#define MAX_MESSAGE_LENGTH MAX_PAYLOAD_LENGTH + 7 + 7
#define DELIMITER ';'
#define PAYLOAD_DELIMITER ','
#define END_DELIMITER '\n'

// message types
#define MT_HEARTBEAT_QUEEN 0
#define MT_HEARTBEAT 1


//TODO: make this a class with inheritance
typedef struct Message_t
{
  int to;
  int from;
  int type;
  char payload[MAX_PAYLOAD_LENGTH];
};

class Comm
{
public:
  Comm(int rx, int tx);
  void setup(int rx, int tx);
  Message_t *loop(void);
  void send(Message_t *message);
private:
  SoftwareSerial *comm;
  // we can only read one character at a time from the xbee so
  // we incrementally save the results to a buffer
  char input_buffer[MAX_MESSAGE_LENGTH];

  Message_t *parse_message(char *input);
};

#endif