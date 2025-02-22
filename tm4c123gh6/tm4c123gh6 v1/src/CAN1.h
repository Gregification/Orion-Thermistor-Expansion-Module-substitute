/*
 * CAN0.hpp
 *
 *  Created on: Feb 21, 2025
 *      Author: greg
 *
 * TODO make this not can 0/1 specific, driver lib has a example approach
 */

#ifndef SRC_CAN1_H_
#define SRC_CAN1_H_

#include <stdint.h>

// adjustable vals
#define CAN1_PROPTIME_NS   120      // <delay> = <prop speed> * <length> + <hardware overhead>  ; eg: 5<ns/m> * 3<m> + 100<ns> = 105ns prop delay

void initCAN1();
void CAN1setBaud(uint32_t baudrate, uint32_t clock_freq);

/*
 * The message objects are numbered from 1 to 32. 0 also maps to 32.
 */
void CAN1txMsg(uint8_t msgBlk);

/*
 * The message objects are numbered from 1 to 32. 0 also maps to 32.
 */
void CAN1setMsg(uint8_t msgBlk, uint64_t * data);

/*
 * The message objects are numbered from 1 to 32. 0 also maps to 32.
 */
void CAN1getMsg(uint8_t msgBlk, uint64_t * data);

#endif /* SRC_CAN1_H_ */
