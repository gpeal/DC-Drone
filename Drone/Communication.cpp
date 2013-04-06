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
    // input_buffer[input_buffer_index++] = input_char;
    strncat(input_buffer, &input_char, 1);
    debug->log("Input buffer: %s (FM: )", input_buffer);
    if (input_char == END_DELIMITER)
    {
    //   strncat(input_buffer, "\0", 1);
      message = parse_message(input_buffer);
      input_buffer[0] = '\0';
      if (message->type == -1)
      {
        debug->logl(ERROR, "Message returned with error: %s", message->payload);
        return NULL;
      }
      return message;
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
  message = new Message_t;

  char temp[MAX_PAYLOAD_LENGTH];
  // string formatter for sscanf to decode the message
  char formatter[100];
  int num_matched;
  debug->log("Parsing message: %s", input);
  sprintf(formatter, "%%d%c%%d%c%%d%c%%s%c", DELIMITER, DELIMITER, DELIMITER, END_DELIMITER);
  debug->log("Format string: %s", formatter);
  num_matched = sscanf(input, formatter, &(message->to), &(message->from), &(message->type), message->payload);
  debug->log("Parsed message (%d arguments): %s", num_matched, input);
  debug->log("To: %d\tFrom: %d\tType: %d\tPayload: %s", message->to, message->from, message->type, message->payload);
  if (num_matched != 4)
  {
    debug->logl(ERROR, "Error parsing message (parsed %d of 4 inputs)", num_matched);
    message->type = -1;
    strcpy(message->payload, "Error parsing message");
  }
  else if(message->to != DRONE_ID && message->to != 0)
  {
    debug->logl(INFO, "Received a message for drone %d (I am %d)", message->to, DRONE_ID);
    message->type = -1;
    strcpy(message->payload, "Wrong drone");
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
  int ret;
  ret = sprintf(buffer, "%d%c%d%c%d%c%s%c", message->to, DELIMITER, message->from, DELIMITER, message->type, DELIMITER, message->payload, END_DELIMITER);
  if (ret != 8)
  {
    debug->logl(ERROR, "Only parsed %d arguments from message (out of 8)", ret);
  }
  comm->print(buffer);
}