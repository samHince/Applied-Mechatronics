/*
* [Mech7.c] Encoder test (speed)
************************************
* THIS CODE WRITTEN FOR MECHATRONICS
* While studying abroad in Lund
* By Sam Hince
*/

//include some useful libraries
#include <avr/io.h>
#include <stdio.h>
#include <avr/interrupt.h>
#include <util/delay.h>

//define constants for future use
#define FOSC 1000000UL //clock speed
#define BAUD 2400 //9600
#define MYUBRR (FOSC/ (16UL * BAUD)) - 1
#define AVGLEN 64
#define AVGPWR 6 //change with above
#define MAXCHG = 5000; //approx 1000 per rpm, so 5 rpm above avg

//initialize global variables
int max_ticks = 120000; //not my origional numbers - verify
int min_ticks = 400;
int acel_limit = 101;
unsigned int tick_array[AVGLEN] = {0};
unsigned int ticks;
unsigned int conversion = 625000;//(1000000 * 60) / 96;

// -------- INITIALIZATIONS ---------------------
//initialize clock / counter (/8 maybe?)
int clock_init(){
    // Setup clock in the TCCR1B register Set CS11 to 1 for /8 prescaling
    TCCR1B |= (1<<CS11);
    TCCR1A = 0b00000000; //"Normal mode", the counter is just counting.
    // TIMSK1= (1<<TOIE1);//Overflow Interrupt Enable
    return 1;
}

void USART_Init(unsigned int ubrr){
    //Set baud rate
    UBRR0H = (unsigned char)(ubrr>>8);
    UBRR0L = (unsigned char)ubrr;
    //Enable receiver and transmitter
    UCSR0B = (1<<RXEN0)|(1<<TXEN0);
    // Set frame format: 8data, 2stop bit
    UCSR0C = (1<<USBS0)|(3<<UCSZ00);
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

int ticks_to_speed(unsigned int conv_ticks){
    unsigned int RPM = conversion / conv_ticks;
    return RPM;
}

void USART_Transmit(unsigned char data){
    // Wait for empty transmit buffer
    while ( !( UCSR0A & (1<<UDRE0)) );
    // Put data into buffer, sends the data
    UDR0 = data;
}

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

int set_clock(unsigned int t){
    //Save global interrupt flag
    unsigned char sreg = SREG;
    //Disable interrupts
    cli();
    
    //Set clock (TCNT1) to t
    TCNT1 = t;
    
    //Restore global interrupt flag
    SREG = sreg;
    
    //return successful
    return 1;
}

int get_tick(){
    //read ticks (TCNT1) into count
    unsigned int count = TCNT1;
    
    //return count
    return count;
}

int speed_check(){
    // Save global interrupt flag
    unsigned char sreg = SREG;
    // Disable interrupts
    cli();
    
    //get number of ticks
    unsigned int temp_ticks = get_tick();
    //reset clock
    set_clock(0);
    
    //pre filter
    if ((temp_ticks < max_ticks) && (temp_ticks > min_ticks)){
        //store in global array
        for(int i = (AVGLEN - 1); i > 0; i = i - 1){
            tick_array[i] = tick_array[i - 1];
        }
        tick_array[0] = temp_ticks;
    } else {
        //do nothing
    }
    
    // Restore global interrupt flag
    SREG = sreg;
    
    //return successful
    return 1;
}

int mean_ticks(){
    unsigned int sum = 0;
    for(int i = 0; i < AVGLEN; i = i + 1){
        sum = sum + tick_array[i];
    }
    
    unsigned int avg = sum >> AVGPWR;
    return avg;
}
// -------- INTERUPT ROUTINE --------------------

// Encoder interrupt
ISR(PCINT1_vect,ISR_BLOCK){ //With other interrupts disabled
    speed_check();
}

// -------- MAIN PROGRAM ------------------------

int main (){
    // run initialization routines
    pin_init();
    PWM_init();
    interupt_init();
    clock_init();
    USART_Init(MYUBRR);
    
    while(1){
        //set some PWM output
        OCR0A = 100;
        
        //get speed
        unsigned int rev_per_min = ticks_to_speed(mean_ticks());
    
        //send RPM back
        USART_Transmit((unsigned char)rev_per_min); 
        
        //set of 0.5 second loop
        _delay_ms(500);
    }
    //return sucess
    return 1;
}
