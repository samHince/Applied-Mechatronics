/*
* [Mech3.c] Blinking LED test
************************************
* THIS CODE WRITTEN FOR MECHATRONICS
* While studying abroad in Lund
* By Sam Hince
*/

/*
* This code should flash two LEDS alternatly useing two different routines
* in order to check AVR board fundamental functionality
*/

// -------- INCLUDE SOME HEADERS ---------------
#include <avr/io.h>
#include <stdio.h>
#include <util/delay.h>
//#include <avr/interrupt.h>

// -------- DEFINE CONSTANTS & VARIABLES --------

// -------- INITIALIZATIONS ---------------------
int pin_init(){
    // Set D as output
    DDRD = 0xFF;

    // Set encoder input
    DDRC = 0x00;

    // Return successful
    return 1;
}

// -------- FUNCTIONS -------------------------
int PD7_on(){
    PORTD = ( 1 << PD7 );

    // Return successful
    return 1;
}

int PD7_switch(){
    //switch PD7 to other output
    if(PORTD == ( 1 << PD7 )){
        PORTD = ( 0 << PD7 );
    }else{
        PORTD = ( 1 << PD7 );
    }
    
    //return sucessful 
    return 1;
}

// -------- INTERUPT ROUTINE --------------------

// -------- MAIN PROGRAM ------------------------

int main (){
    // initialize variables //
    
    // run initialization routines //
    pin_init();

    // pre loop setup //
    PD7_on(); //tuen on test LED

    while (1){ //flash test LEDs
        //Set PB2 pin to high
        PORTB = ( 1 << PB2 );
        //switch output of PD7
        PD7_switch();
        //pause for a sec
        _delay_ms(1000);
        
        
        //Set PB2 pin back to low
        PORTB = ( 0 << PB2 );
        //switch output of PD7
        PD7_switch();
        //pause for a sec
        _delay_ms(1000);
    }
    
    //return successful
    return 1;
}