/*
 * common.c
 *
 *  Created on: Feb 21, 2025
 *      Author: greg
 */

#include "common.h"

uint16_t htons(uint16_t num){
    return (num & 0x00FF) << 8 + (num & 0xFF00) >> 8;
}
