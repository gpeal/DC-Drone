#ifndef COMMUNICATION_H
#define COMMUNICATION_H
#include "SoftwareSerial.h"
#include <XBee.h>

#define MAX_PAYLOAD_LENGTH 32
// message parse errors
#define PARSE_ERROR 0
#define INCORRECT_ID_ERROR 1
// include enough space for the payload plus the to id, from id, message type, their delimiters
#define MAX_MESSAGE_LENGTH MAX_PAYLOAD_LENGTH + 7
#define DELIMITER ';'
#define PAYLOAD_DELIMITER ','
#define END_DELIMITER '\n'
#define DUMMY_PAYLOAD "_"

// message types
#define MT_INITIALIZE 0
#define MT_INITIALIZE_RESPONSE 1
#define MT_HEARTBEAT 2
#define MT_HEARTBEAT_RESPONSE_IDLE 3
#define MT_HEARTBEAT_RESPONSE_DEPLOYING 4
#define MT_HEARTBEAT_RESPONSE_SEARCHING 5
#define MT_HEARTBEAT_RESPONSE_RELOCATING 6
#define MT_HEARTBEAT_RESPONSE_ATTACKING 7
#define MT_HEARTBEAT_RESPONSE_SEARCHING_NEST 8
#define MT_HEARTBEAT_RESPONSE_RETURNING 9
#define MT_HEARTBEAT_RESPONSE_DELIVERING 10
#define MT_HEARTBEAT_RESPONSE_DISCONNECTED 11
#define MT_SWITCH_STATE 12


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
  void send(int type, char *msg);
private:
  SoftwareSerial *comm;
  XBee xbee;
  Message_t *message;
  Rx16Response xbee_response;
  Tx16Request xbee_request;
  char xbee_message[MAX_MESSAGE_LENGTH];
  void parse_message(char *input);
  TxStatusResponse xbee_tx_response;
};

#endif