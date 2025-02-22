/*
 * main.cpp
 *
 *  Created on: Feb 20, 2025
 *      Author: greg
 */

/* heres a quote from Tim (Tim Stevens) original work.       
 *      "
 *      This code works. Its ghetto, but the BMS is happy with it. had to add that 57 for some weird reason. IDK
 *      Checksum is CRITICAL to getting BMS not to fault.
 * 
 *      This is specific to the 40x Temp Sensing Module made fr E17.
 * 
 *      Despite some of the terrible programming, the program cycles very fast, usually a bit less than a millisecond. Meaning all temps are checked in 8ms or less.
 *      "
 * you should note that the first paragraph is wrong (he should have read the data sheet more closely),
 * this code is better - allegedly (from me)
 */

#include <src/CAN1.h>
#include <src/CAN1.h>
#include <stdint.h>
#include <stdbool.h>
#include "tm4c123gh6pm.h"

#include "3rd party code/uart0.h"
#include "3rd party code/gpio.h"


// constants
#define F_CPU       50e6
#define UART0_BAUD  9600
#define NEWLLN      "\n\r"


int main(){
    // yap yap yap

    /*---init-----------------------------------------------------------------
     * 50Mhz , anything higher than 66Mhz involves the PLL (complicated :( )
     *      considering the CAN baud rate MUST be a integer multiple of the clock
     *      we just keep it simple and make the main clock a integer multiple.
     */

    // 66.66 Mhz
    SYSCTL_RCC_R = SYSCTL_RCC_XTAL_16MHZ | SYSCTL_RCC_OSCSRC_MAIN | SYSCTL_RCC_USESYSDIV | (3 << SYSCTL_RCC_SYSDIV_S);

    enablePort(PORTF);
    _delay_cycles(3);

    initUart0();
    setUart0BaudRate(UART0_BAUD, F_CPU);

    putsUart0("orion BMS thermistor expansion substitute v1");
    putsUart0(NEWLLN);
    putsUart0("github.com/Gregification/Orion-Thermistor-Expansion-Module-substitute");
    putsUart0(NEWLLN);

    /*---can0 config-----------------------------------------------------------
     * short on time. TODO use driver lib
     */

    initCAN1();

 }

