/*
* [Mech1.c] Master code
************************************
* THIS CODE WRITTEN FOR MECHATRONICS
* While studying abroad in Lund
* By Sam Hince
*/

//Files to be created:
//  [Mech1.c] Master code                       [o]
//  [Mech2.c] PC coms                           [o]
//  [Mech3.c] Blinking LED test                 [x]
//  [Mech4.c] AVR coms test                     [x]
//  [Mech4_1.c] AVR bounceback                  [x]
//  [Mech5.c] PWM test (fade up and down)       [x]
//  [Mech6.c] Encoder + PWM test (increment)    [x]
//  [Mech7.c] Encoder test (speed)              [o]
//  [Mech8.c] Detail adjust                     [o]

// ability to scroll light up an down?

// Notes on coms
// Initialze buffers
// Clear serial buffer
// Send same data size every time
// Use a checksum

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
    // Set timer registers
    
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
int Clock_init(){
    //Setup clock in the TCCR1B register Set CS11 to 1 for /8 prescaling?
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

//coppies file 
void addClockTicks(){
    // Taking timestamp of interrupt & reseting the clock
    unsigned int ClockTicks = readClock();
    resetClock();

    // Checking if the timestamp is Valid to add to the TimeStampArray - Else Nothing
    if ( ClockTicks < maxCountsPerInterrupt && ClockTicks > minCountsPerInterrupt){
        
        // Adding the ClockTicks since previous reset to the ClockStampArray at counter position
        ClockStampArray[ClockStampCounter] = ClockTicks;

        // Adding 1 to the ClockStampCounter
        ClockStampCounter++;

        // if ClockStampCounter goes higher than filterSize, Reset to 0.
    if(ClockStampCounter > filterSize-1){
        ClockStampCounter = 0;
    }
}
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
    
    // Store speed
}

void do_something_atomic ()
{
unsigned char sreg = SREG ; // remember status register
cli (); // disable interrupts
do_stuff (); // without being interrupted
SREG = sreg ; // restore status register
}

// -------- MAIN PROGRAM ------------------------

int main (){
    // run initialization routines
    pin_init();
    PWM_init();
    interupt_init();
    
    PD7_on(); //tuen on test LED 
    
    // initialize variables
    unsigned char duty;
    
    while (1){
        if(target < speed){
            if(duty > 0){
                // decrease PWM
                duty--
                // pause for motor to adjust speed 
                _delay_ms(8);
            }
        }
        else if(target > speed){
            if(duty < 255){
                // increase PWM
                duty++
                // pause for motor to adjust speed
                _delay_ms(8);
            }
        }
        else{
            // do nothing at all
        }
        // motor to PWM
        OCR0A = duty;
        }
    }
}

        //for(duty=0; duty<255; duty++){
        //    OCR0A = duty;  /*increase the LED light intensity*/
        //    _delay_ms(8);
        //}
        //for(duty=255; duty>1; duty--){
        //    OCR0A = duty;  /*decrease the LED light intensity*/
        //    _delay_ms(8);
