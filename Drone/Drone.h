#ifndef DRONE_H
#define DRONE_H

#define QUEEN_ID 1
#define DRONE_ID_EEPROM 0
extern int DRONE_ID;

void delegate_message(Message_t *message);
void free_memory_check(void);
void search(void);
void attack(void);

#endif