/*
 * CAN0.cpp
 *
 *  Created on: Feb 21, 2025
 *      Author: greg
 */
/*
 * message interface usage
 *  1 : rx/get
 *  2 : tx/set
 */

#include <src/CAN1.h>
#include <stdlib.h>
#include "tm4c123gh6pm.h"
#include "common.h"

#define C1_RX_P     BV(0)
#define C1_TX_P     BV(1)

#define DECP    100     // decimal scler for percision. % * DECP = <int val used>
// specified by the can A/B standard (the one tm4c targets)
#define CAN_BIT_MIN_TQ 8
#define CAN_BIT_MAX_TQ 25

void initCAN1(){
    // enable clocks
    SYSCTL_RCGCGPIO_R |= SYSCTL_DC4_GPIOA;      // clk to PA
    _delay_cycles(3);
    SYSCTL_RCGC0_R |= SYSCTL_DC1_CAN1;          // PA clk to CAN1
    _delay_cycles(3);

    GPIO_PORTA_AFSEL_R |= C1_RX_P | C1_TX_P;    // use alt func
    // set can as alt functions
    GPIO_PORTA_PCTL_R &= ~(GPIO_PCTL_PA0_M | GPIO_PCTL_PA1_M);
    GPIO_PORTA_PCTL_R |= GPIO_PCTL_PA0_CAN1RX | GPIO_PCTL_PA1_CAN1TX;

    // init can controller
    CAN1_CTL_R |= CAN_CTL_INIT;     // start init
    CAN1_CTL_R &= ~(CAN_CTL_IE | CAN_CTL_SIE | CAN_CTL_EIE); // disable interrupts
    CAN1_CTL_R &= ~(CAN_CTL_TEST);  // disable test mode
    CAN1_CTL_R &= ~(CAN_CTL_INIT);  // stop init

    // can 1 as rx

}

void CAN1txMsg(uint8_t msgBlk){

}

void CAN1setBaud(uint32_t baudrate, uint32_t clock_freq){

    /*---calc values-----------------------------------------
     * calculation explanation https://www.ti.com/lit/an/sprac35/sprac35.pdf?ts=1740127898516
     *      credit to Adam Haun of TI for that document.
     * the calculations use are based on that.
     *
     * see pg.1076,pg.1063-1064
     * tldr: each bit transmitted split into smaller time units called "tq" (pg.1063)
     *      we need to find a clk division so that the module clk divides the closest
     *      into the target baud rate while also having a reasonable sample stage.
     * - calculated for loosest oscillator tolerance
     * - the can standard guarentees a compatiable tq/bit ratio in the range of 8 to 25, so
     *      we just loop over all possibilities and use the one that gives the greatest
     *      oscillator tolerance.
     * - note that on some systems the timing is self corrected, this is not the case for the tm4c123gh
     *      so this tq calculation is quite important
     * - this is by no means a compact way to calculate this. made for simplicity. though i try to stay away from floats.
     */

    // condition values
    uint32_t bt_ns      = 1e9 / baudrate;           // bit time in ns
    uint32_t clkspb     = clock_freq / baudrate;    // cpu cycles per bit

    /*
     * - note that everything in this struct can be calculated given the baudprescaler only. its just
     *      simpiler to store the values
     * - see table at figure 17-4 ~pg.1063 for timing breakdown
    */
    typedef struct {
        uint16_t baudprescaler; // 10b as { <3:0 BRPE (MSBs)> , <5:0 BRP (LSBs)> }
        uint8_t prop_tq;
        uint8_t ph1_tq;         // phase
        uint8_t ph2_tq;
        uint8_t maxSyncJmp_tq;
        int16_t osci_tolerance; // % * 100
    } Condition;

    //vals
    Condition bstcs; // best case

    // calculation
    {
        uint8_t tqpb;
        for(tqpb = CAN_BIT_MIN_TQ; tqpb <= CAN_BIT_MAX_TQ; tqpb++){
            // tqpb : tq per bit

            Condition cond;
            cond.baudprescaler  = CAN_BIT_MAX_TQ + CAN_BIT_MIN_TQ - tqpb;

            int16_t error   = abs(
                    ((tqpb * cond.baudprescaler - clkspb) * DECP +clkspb/2)
                    / clkspb);
            cond.prop_tq = (CAN1_PROPTIME_NS * tqpb +bt_ns/2) / bt_ns;
            cond.ph2_tq = tqpb - (1 + cond.prop_tq +1) / 2;
            cond.ph1_tq = tqpb - (1 + cond.prop_tq + cond.ph2_tq);
            cond.maxSyncJmp_tq = cond.ph1_tq > cond.ph2_tq ? cond.ph1_tq : cond.ph2_tq; // min of { ph1, ph2, #4 }
            if(cond.maxSyncJmp_tq > 4)
                cond.maxSyncJmp_tq = 4;

            int16_t df1 = 2 * (13 * tqpb - cond.ph2_tq); // the 13 is from 12+1 divisions of the bit time. (12 base, +1 from hardware properties)
            int16_t df2 = 20 * tqpb;
            int16_t osctol = (cond.maxSyncJmp_tq * DECP) / (df1 > df2 ? df1 : df2); // max variation means min tolerance

            if( // if the tq values are valid
                    cond.ph1_tq < tqpb && cond.ph2_tq < tqpb                            // phases are greater than 0
                    && cond.ph1_tq <= CAN_BIT_MAX_TQ && cond.ph2_tq <= CAN_BIT_MAX_TQ   // phases are individually in expected range
                    && cond.prop_tq <= CAN_BIT_MAX_TQ                                   // prop is in expected range
                    && (1 + cond.prop_tq + cond.ph1_tq + cond.ph2_tq == tqpb)           // total tq is the target tq per bit
              )
                cond.osci_tolerance = error - (cond.maxSyncJmp_tq * DECP) / osctol;
            else
                cond.osci_tolerance = -1;

            // if is better
            if(cond.osci_tolerance > bstcs.osci_tolerance || tqpb == CAN_BIT_MIN_TQ) {
                bstcs = cond;
            }
        }
    }

    //---write values----------------------------------------

    CAN1_CTL_R |= CAN_CTL_INIT;     // start init
    CAN1_CTL_R |= CAN_CTL_CCE;      // enable write access to CANBIT when in init mode

    // pg.1076
    CAN1_BIT_R &= ~(CAN_BIT_BRP_M | CAN_BIT_SJW_M| CAN_BIT_TSEG1_M | CAN_BIT_TSEG2_M);
    CAN1_BIT_R |= CAN_BIT_BRP_M & ((bstcs.baudprescaler & 0x3F) << CAN_BIT_BRP_S);
    CAN1_BIT_R |= CAN_BIT_SJW_M & ((bstcs.maxSyncJmp_tq & 0x03) << CAN_BIT_SJW_S);
    CAN1_BIT_R |= CAN_BIT_TSEG1_M & ((bstcs.ph1_tq      & 0x0F) << CAN_BIT_TSEG1_S);
    CAN1_BIT_R |= CAN_BIT_TSEG2_M & ((bstcs.ph2_tq      & 0x07) << CAN_BIT_TSEG2_S);

    CAN1_CTL_R &= ~(CAN_CTL_INIT);  // stop init
}

void CAN1setMsgData(uint8_t messageBlock, uint64_t * data){
    while(CAN1_IF2CRQ_R | CAN_IF2CRQ_BUSY){
        // wait
    }

    // set target message block pg.1081
    CAN1_IF2CRQ_R &= ~(0x1F);
    CAN1_IF2CRQ_R |= 0x1F & messageBlock;

    // set values to write pg.1094 . note : little endian
    *(volatile uint16_t *)CAN1_IF2DA1_R = htons(((uint16_t *)data)[0]);
    *(volatile uint16_t *)CAN1_IF2DA2_R = htons(((uint16_t *)data)[1]);
    *(volatile uint16_t *)CAN1_IF2DB1_R = htons(((uint16_t *)data)[2]);
    *(volatile uint16_t *)CAN1_IF2DB2_R = htons(((uint16_t *)data)[3]);

    // write message pg.1082
    CAN1_IF2CMSK_R |= CAN_IF1CMSK_WRNRD;
}

void CAN1readMsg(uint8_t messageBlock, uint64_t * data){

}
