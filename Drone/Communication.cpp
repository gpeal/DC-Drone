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
  input_buffer_index = 0;
  comm->begin(9600);
  comm->println("Starting UP");
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

  if (comm->available())
  {
    input_char = comm->read();
    debug->log("Received: %c", input_char);
    input_buffer[input_buffer_index++] = input_char;
    if (input_char == DELIMITER)
    {
      if (message_read_state == PAYLOAD)
      {
        message_read_state = TO;
        message = parse_message(input_buffer);
        if (message == NULL)
        {
          debug->log("Message was discarded: %s", input_buffer);
          return NULL;
        }
        return message;
      }
    }
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
  message = (Message_t *)malloc(sizeof(message));

  MessageReadState state;
  char temp[MAX_PAYLOAD_LENGTH];
  // string formatter for sscanf to decode the message
  char formatter[100];
  int num_matched;

  sprintf(formatter, "%%d%c%%d%c%%d%c%%s%c", DELIMITER, DELIMITER, DELIMITER, DELIMITER);
  num_matched = sscanf(input, formatter, &(message->to), &(message->from), &(message->type), message->payload);
  if (num_matched != 4)
  {
    delete(message);
    return (Message_t *)INCORRECT_ID_ERROR;
  }
  else if(message->to != DRONE_ID && message->to != 0)
  {
    delete(message);
    return (Message_t *)INCORRECT_ID_ERROR;
  }
  return message;
}