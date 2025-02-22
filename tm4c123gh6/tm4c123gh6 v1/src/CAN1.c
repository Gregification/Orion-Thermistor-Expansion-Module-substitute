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
#include "tm4c123gh6pm.h"
#include "common.h"

#define C1_RX_P     BV(0)
#define C1_TX_P     BV(1)

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

void CAN1setBaud(uint32_t baudrate, uint32_t clock_freq){

    //---calc values-----------------------------------------

    // see pg.1076,pg.1063-1064
    // calculation demonstrated here https://www.ti.com/lit/an/sprac35/sprac35.pdf?ts=1740127898516
    // tldr: each bit transmitted split into smaller tim units called "tq" (pg.1063)
    //      we need to find a clk division so that the module clk divides the closest
    //      into the target baud rate.
    // calculated for loosest oscillator tolerance

    uint16_t brp        = clock_freq / baudrate; // 1tq in clock cycles, the mcu effectively adds 1 to the value
    uint16_t btime_us   = ;


    //---write values----------------------------------------

    CAN1_CTL_R |= CAN_CTL_INIT;     // start init
    CAN1_CTL_R |= CAN_CTL_CCE;      // enable write access to CANBIT when in init mode

    CAN1_BIT_R  &= ~(0x3F);
    CAN1_BIT_R  |= 0x3F & brp;
    CAN1_BRPE_R &= ~(0xF);
    CAN1_BRPE_R |= 0xF & (brp >> 6);

    CAN1_CTL_R &= ~(CAN_CTL_INIT);      // stop init
}

void CAN1setMsgData(uint8_t messageBlock, uint64_t * data){
    while(CAN1_IF2CRQ_R | CAN_IF2CRQ_BUSY){
        // wait
    }

    // set target message block pg.1081
    CAN1_IF2CRQ_R &= ~(0x1F);
    CAN1_IF2CRQ_R |= 0x1F & messageBlock;

    // set values to write pg.1094
    *(volatile uint16_t *)CAN1_IF2DA1_R = htons(((uint16_t *)data)[0]);
    *(volatile uint16_t *)CAN1_IF2DA2_R = htons(((uint16_t *)data)[1]);
    *(volatile uint16_t *)CAN1_IF2DB1_R = htons(((uint16_t *)data)[2]);
    *(volatile uint16_t *)CAN1_IF2DB2_R = htons(((uint16_t *)data)[3]);

    // write message pg.1082
    CAN1_IF2CMSK_R |= CAN_IF1CMSK_WRNRD;
}

void CAN1readMsg(uint8_t messageBlock, uint64_t * data){

}
