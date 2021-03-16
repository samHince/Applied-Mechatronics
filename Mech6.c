/*
* [Mech6.c] Encoder + PWM test (increment)
************************************
* THIS CODE WRITTEN FOR MECHATRONICS
* While studying abroad in Lund
* By Sam Hince
*/

// -------- INCLUDE SOME HEADERS ---------------
#include <avr/io.h>
#include <stdio.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdbool.h>

// -------- DEFINE CONSTANTS --------------------
unsigned char duty;
bool up;

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

int interupt_init(){
    //Allow interupts from C register
    PCICR = (1<<PCIE1);

    //Enable interupts from encoder pins
    PCMSK1 |= ((1<<PCINT11)|(1<<PCINT10)); //(Enc 1) | (Enc 2)

    //Set interupt flag
    sei();

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

int inc_duty(){
    //track direction changes
    if(duty == 255){
        up = false;
    }else if(duty == 0){
        up = true;
    }
    
    //increment duty
    if(up){
        duty++;
    }else{
        duty--;
    }
    
    // Return successful
    return 1;
}


// -------- INTERUPT ROUTINE --------------------

// Encoder interrupt
ISR(PCINT1_vect,ISR_BLOCK){ //With other interrupts disabled
    //swich led 
    PD7_switch();
    //increment duty
    inc_duty();
}

// -------- MAIN PROGRAM ------------------------

int main (){
    // initialize variables
    up = true; //begin by incresing power
    duty = 0; //start at 0 power
    OCR0A = duty; //initialize PWM at initial value of duty 
    
    // run initialization routines
    pin_init();
    PWM_init();
    interupt_init();

    PD7_on(); //tuen on test LED
    
    while (1){
        // set PWM to the calculated duty
        OCR0A = duty;
    }
}