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
  comm->begin(9600);
  xbee = XBee();
  xbee.begin(*comm);
  xbee_response = Rx16Response();
  xbee_tx_response = TxStatusResponse();
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
    xbee_message[xbee_response.getDataLength()] = '\0';
    // debug->log("Rcv (%X, %d): %s", xbee_response.getRemoteAddress16(), xbee_response.getDataLength(), xbee_message);
    // if the payload is empty, fill it with a dummy payload.
    // this is only necessary to help with the sscanf string parsing
    // which breaks on empty payloads otherwise
    if(xbee_message[strlen(xbee_message) - 2] == DELIMITER)
    {
      sprintf(xbee_message, "%s%s\n", xbee_message, DUMMY_PAYLOAD);
    }
    message = parse_message(xbee_message);
    if (message->type == -1)
    {
      debug->logl(ERROR, "%s", message->payload);
      delete message;
      return NULL;
    }
    return message;
  }
  else if (xbee.getResponse().isAvailable() && xbee.getResponse().getApiId() == TX_STATUS_RESPONSE) {
    xbee.getResponse().getZBTxStatusResponse(xbee_tx_response);
    // get the delivery status, the fifth byte
    if (xbee_tx_response.getStatus() == SUCCESS)
    {
       // debug->log("TX success");
    }
    else
    {
      debug->logl(ERROR, "TX error");
    }
  }
  else if (xbee.getResponse().isAvailable() && xbee.getResponse().isError()) {
    debug->logl(ERROR, "XBEE error");
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
  // debug->log("Parsing %s with %s and got %d", input, formatter, num_matched);
  // if the payload is DUMMY_PAYLOAD it was just added to make the string parsing easier
  // remove it now so it doesn't actually get seen by the user
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
void Comm::send(int type, char *msg)
{
  // TODO merge buffer and payload
  char buffer[MAX_MESSAGE_LENGTH];
  sprintf(buffer, "%d%c%d%c%d%c%s%c", QUEEN_ID, DELIMITER, DRONE_ID, DELIMITER, type, DELIMITER, msg, END_DELIMITER);
  debug->log("Sending (%d) %s", strlen(buffer), buffer);
  xbee_request = Tx16Request(QUEEN_ID, (uint8_t *)buffer, strlen(buffer));
  xbee.send(xbee_request);
}