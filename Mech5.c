/*
* [Mech5.c] PWM test (fade up and down)
************************************
* THIS CODE WRITTEN FOR MECHATRONICS
* While studying abroad in Lund
* By Sam Hince
*/

// -------- INCLUDE SOME HEADERS ---------------
#include <avr/io.h>
#include <stdio.h>
#include <util/delay.h>
//#include <avr/interrupt.h>

// -------- DEFINE CONSTANTS --------------------
unsigned char duty;

// -------- INITIALIZATIONS ---------------------
int pin_init(){
    // Set D as output
    DDRD = 0xFF;

    // Set encoder input
    DDRC = 0x00;

    // Return successful
    return 1;
}

int PWM_init(){
////Set timer registers ////
    //set fast PWM mode with non-inverted output
    TCCR0A |= ((1<<COM0A1)|(1<<COM0A0)|(1<<COM0B1)|(1<<COM0B0)|(1<<WGM01)|(1<<WGM00));

    //Setting clock no pre-scaler
    TCCR0B |= (1<<CS00);

    // Return successful
    return 1;
}

// -------- FUNCTIONS -------------------------
int PD7_on(){
    PORTD = ( 1 << PD7 );

    // Return successful
    return 1;
}

// -------- INTERUPT ROUTINE --------------------

// -------- MAIN PROGRAM ------------------------

int main(){
    // initialize variables
    duty = 0;
    OCR0A = duty; //set PWM register
        
    // run initialization routines
    pin_init();
    PWM_init();
    
    //pre loop setup
    PD7_on(); //tuen on test LED
    
    while (1){
        // slowly increase PWM to 100%
        for(duty=0; duty<255; duty++){
            OCR0A = duty; /*increase the LED light intensity*/
            _delay_ms(8);
        }
        // slowly reduce PWM to 0
        for(duty=255; duty>1; duty--){
            OCR0A = duty; /*decrease the LED light intensity*/
            _delay_ms(8);
        }
    }
    //return successful
    return 1;
}