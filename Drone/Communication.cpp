#include "Communication.h"

void Xbee::setup(void)
{
  input_buffer_index = 0;
  serial.begin(57600);
}

/**
 * Xbee::loop checks the xbee serial port for new characters. If there is a character waiting
 * on the bus, it adds it to the current input buffer. It has to do this because there is no equivalent
 * for serial.readln. It also increments the read state when it receives a delimiter.
 *
 * @return Message returns a parsed message after receiving a complete message. NULL otherwise.
 */
Message Xbee::loop(void)
{
  char input_char;
  Message message;

  if (serial.available())
  {
    input_char = serial.read();
    input_buffer[input_buffer_index++] = input_char;
    if (input_char == DELIMITER)
    {
      // message end
      if (MessageReadState == PAYLOAD)
      {
        message = parse_message(input_buffer);
        if (message)
        {
          return message;
        }
        // there was an error parsing the message
        else
        {
          debug->error("There was an error parsing the message: %s", input_buffer);
          return NULL;
        }
      }
    }
  }

  return NULL;
}

/**
 * Xbee::parse_message takes an input buffer and returns a parsed message object
 *
 * @param char * input should be in the form of
 *   TO_ID DELIMITER FROM_ID DELIMITER MESSAGE_TYPE DELIMITER PAYLOAD delimiter
 *
 * @return Message the parsed message object
 */
Message Xbee::parse_message(char *input)
{

}