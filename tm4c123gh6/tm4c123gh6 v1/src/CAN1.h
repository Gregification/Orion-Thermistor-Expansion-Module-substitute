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
#define CAN1_PROPDELAY_NOM_NS       50      // <delay> = <prop speed> * <length>
#define CAN1_PROPDELAY_EXTRA_NS     10      // over head safety

void initCAN1();
void CAN1setBaud(uint32_t baudrate, uint32_t clock_freq);

/*
 * The message objects are numbered from 1 to 32. 0 also maps to 32.
 */
void CAN1setMsg(uint8_t messageBlockNum, uint64_t * data);

/*
 * The message objects are numbered from 1 to 32. 0 also maps to 32.
 */
void CAN1readMsg(uint8_t messageBlockNum, uint64_t * data);

#endif /* SRC_CAN1_H_ */
