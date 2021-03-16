/*
* THIS CODE WRITTEN FOR MECHATRONICS
* While studying abroad in Lund
* By Sam Hince
*/

// -------- INCLUDE SOME HEADERS ---------------

#include <avr/io.h>
#include <stdio.h>
#include <avr/interrupt.h>
//#include <stdlib.h>
//#include <string.h>
//#include <util/delay.h>

// -------- DEFINE CONSTANTS --------------------





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
    PCMSK1 = (1<<PCINT11); //Enc 1
    PCMSK1 = (1<<PCINT10); //Enc 2
    //Set interupt flag
    sei();
    
    // Return successful
    return 1; 
}

//rethink maybe
int counter_init(){
    //Setup clock in the TCCR1B register ///CS11 to 1 for /8 prescaling?
    TCCR1B |= (1<<CS11);
    //"Normal mode", the counter is just counting.
    TCCR1A = 0b00000000; 
    
    // Return successful
    return 1;
}

// -------- FUNCTIONS -------------------------

//store speed

//calculate RPM // or reverse

int encoder(){
    //sudo code 
    if (one goes hight)
        if(two goes high)
            forward
        else
            backward
    //end sudo code
    
    //Return successful
    return 1
}

// -------- TEST LED ON -------------------------

int PD7_on(){
    PORTD = ( 1 << PD7 );
    
    // Return successful
    return 1;
}

// -------- INTERUPT ROUTINE --------------------

// Encoder interrupt
ISR(PCINT1_vect,ISR_BLOCK){ //With other interupts disabled
    //Set PD7 to the input from enc1
    PORTD = (( PINB & 0x03 ) << 4 )
}

// -------- MAIN PROGRAM ------------------------

int main (){
    // run initialization routines
    pin_init();
    PWM_init();
    counter_init();
    interupt_init();
    
    PD7_on(); //tuen on test LED 
    
    // initialize variables
    unsigned char duty;
    
    while (1){
        
        ////Flash PB2////
        // Set just one pin to high
        PORTB = ( 1 << PB2 ); 
        //PORTD = ( 0 << PD7 );
        _delay_ms(1000);
        // Set all pins back to low
        PORTB = ( 0 << PB2 ); 
        //PORTD = ( 1 << PD7 );
        _delay_ms(1000);
    }
}

        //for(duty=0; duty<255; duty++){
        //    OCR0A = duty;  /*increase the LED light intensity*/
        //    _delay_ms(8);
        //}
        //for(duty=255; duty>1; duty--){
        //    OCR0A = duty;  /*decrease the LED light intensity*/
        //    _delay_ms(8);
