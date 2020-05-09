#ifndef DATA_STRUCTURE_H
#define DATA_STRUCTURE_H

#include <stdint.h>

typedef struct
{
    uint16_t len; // the length of data to be sent
    char data[1]; // must be "struct hack"
} packet;

#endif