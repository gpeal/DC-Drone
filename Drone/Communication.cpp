#include <MemoryFree.h>
#include "Communication.h"
#include "Debug.h"
#include "Drone.h"

Comm::Comm(int rx, int tx)
{
  if (comm->isListening())
  {
    debug->logl(ERROR, "Attempting to initialize comm port twice");
  }
  else
  {
    debug->log("Initializing Serial Rx: %d, Tx: %d", rx, tx);
  }
  comm = new SoftwareSerial(rx, tx);
  input_buffer[0] = '\0';
  comm->begin(9600);
  xbee = XBee();
  xbee.begin(*comm);
  xbee_response = Rx16Response();
  xbee_request = Tx16Request();
}

/**
 * Comm::loop checks the Comm serial port for new characters. If there is a character waiting
 * on the bus, it adds it to the current input buffer. It has to do this because there is no equivalent
 * for serial.readln. It also increments the read state when it receives a delimiter.
 *
 * @return Message_t returns a parsed message after receiving a complete message. NULL_MESSAGE otherwise.
 */
Message_t *Comm::loop(void)
{
  char input_char;
  Message_t *message;

  xbee.readPacket();
  if (xbee.getResponse().isAvailable() && xbee.getResponse().getApiId() == RX_16_RESPONSE)
  {
    xbee.getResponse().getRx16Response(xbee_response);
    // copy the message to our own char array
    for (int i = 0; i < xbee_response.getDataLength(); i++)
    {
      xbee_message[i] = (char)xbee_response.getData()[i];
    }
    debug->log("Rcv: %s", xbee_message);
    if(xbee_message[strlen(xbee_message) - 2] == DELIMITER)
    {
      sprintf(xbee_message, "%s%s\n", xbee_message, DUMMY_PAYLOAD);
    }
    message = parse_message(x);
    if (message->type == -1)
    {
      debug->logl(ERROR, "%s", message->payload);
      delete message;
      return NULL;
    }
    return message;
  }
  return NULL;
}

/**
 * Comm::parse_message takes an input buffer and returns a parsed message object
 *
 * @param char * input should be in the form of
 *   TO_ID DELIMITER FROM_ID DELIMITER MESSAGE_TYPE DELIMITER PAYLOAD delimiter
 *
 * @return Message_t the parsed message object
 */
Message_t *Comm::parse_message(char *input)
{
  Message_t *message;
  message = new Message_t;

  char temp[MAX_PAYLOAD_LENGTH];
  // string formatter for sscanf to decode the message
  char formatter[32];
  int num_matched;
  sprintf(formatter, "%%d%c%%d%c%%d%c%%s%c", DELIMITER, DELIMITER, DELIMITER, END_DELIMITER);
  num_matched = sscanf(input, formatter, &(message->to), &(message->from), &(message->type), message->payload);
  if (strcmp(message->payload, DUMMY_PAYLOAD) == 0)
  {
    strcpy(message->payload, "");
  }

  if (num_matched != 4)
  {
    message->type = -1;
    sprintf(message->payload, "MSG Parse Error (%d/4)", num_matched);
  }
  else if(message->to != DRONE_ID && message->to != 0)
  {
    message->type = -1;
    strcpy(message->payload, "MSG for wrong drone");
  }
  return message;
}

/**
 * Comm::send takes a Message_t pointer and sends it over serial
 *
 * @param message the message to be sent
 */
void Comm::send(Message_t *message)
{
  char buffer[MAX_MESSAGE_LENGTH];
  uint8_t payload[MAX_MESSAGE_LENGTH];
  int ret;
  sprintf(buffer, "%d%c%d%c%d%c%s%c", message->to, DELIMITER, message->from, DELIMITER, message->type, DELIMITER, message->payload, END_DELIMITER);
  for(int i = 0; i < strlen(buffer); i++)
  {
    payload[i] = (uint8_t)buffer[i];
  }
  xbee_request = Tx16Request(QUEEN_ID, payload, sizeof(payload));
  xbee.send(xbee_request);
}