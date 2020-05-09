/* 
 * First Written : 2020 05 08
 * DATA_STRUCTURE HEADER
 * Written by    : Ammar Faizi, Aviezab
 * MIT License
 * Clang-format
 * Convention    : using memset instead of bzero, bzero is not standard for C lib
*/
#ifndef DATA_STRUCTURE_H
#define DATA_STRUCTURE_H

#include <stdint.h>

// Preparing data_arena as max 16 bit allocation for packet length
char data_arena[65538];

// Packet is our data structure with maximum 16 bit message length
typedef struct
{
    uint16_t len; // the length of data to be sent
    char data[1]; // must be "struct hack"
} packet;

#endif