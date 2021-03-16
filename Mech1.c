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
//  [Mech4_5.c] AVR bounceback                  [x]
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

//TO DO 
// overflow clock + interrupt + couter / reset
// send / recieve interupt 
// pid 

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
//#define MAXCHG 5000; //approx 1000 per rpm, so 5 rpm above avg
#define OVERFLOWS 10 //totally arbitrary number
#define MOTOR OCR0A

//initialize global variables
int max_ticks = 120000; //not my origional numbers - verify
int min_ticks = 400;
int overflow_counter;
int acel_limit = 101;
unsigned int target_speed = 0;
unsigned int tick_array[AVGLEN] = {0};
unsigned int ticks;
unsigned int rev_per_min = 0;
unsigned int conversion = 625000;//(1000000 * 60) / 96;

//initalize PID variables
int error_prior = 0;
int integral = 0;
int derivative;
int PWM;

//tuning variables
int iteration_time = 10; //time in miliseconds

int Kp = 1;
int Ki = 1;
int Kd = 1;

// -------- INITIALIZATIONS ---------------------
//initialize clock / counter
int clock_init(){
    // Setup clock in the TCCR1B register Set CS11 to 1 for /8 prescaling
    TCCR1B |= (1<<CS11);
    TCCR1A = 0b00000000; //normal mode
    // TIMSK1= (1<<TOIE1);//Overflow Interrupt Enable
    return 1;
}

int loop_clock_init(){ ////WORK TO BE DONEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEE
    // Setup clock in the TCCR1B register Set CS10 to 1 for no prescaling
    TCCR2B |= (1<<CS10);
    TCCR2A = 0b00000000; //"Normal mode", the counter is just counting.
    TIMSK2= (1<<TOIE2);//Interrupt Enable
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
    // Enable Usart Interrupt
    UCSR0B |= (1<<RXCIE0);
    UCSR0A |= (1<<RXC0);
}

int interupt_init(){
    //Allow interupts from C register
    PCICR = (1<<PCIE1);
    
    //Enable interupts from encoder pins
    PCMSK1 |= ((1<<PCINT11)|(1<<PCINT10)); //(Enc 1) | (Enc 2)
    
    //Set interupt flag
    //sei();
    
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

unsigned char USART_Receive(void){
    // Wait for data to be received
    while ( !(UCSR0A & (1<<RXC0)) );
    // Get and return received data from buffer
    return UDR0;
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

int pid(int desired_value, int actual_value){
    
    //note: iteration_time removed because synchronous contorl
    int error = desired_value - actual_value;
    integral = integral + error; //integral + (error*iteration_time); 
    derivative = error - error_prior; //(error – error_prior)/iteration_time;
    
    PWM = Kp*error + Ki*integral + Kd*derivative; // + bias;
    
    //store the old error for next iteration
    error_prior = error;
    
    //return sucess
    return PWM;
}

int timer_overflow(){
    // Save global interrupt flag
    unsigned char sreg = SREG;
    // Disable interrupts
    cli();
    
    if(overflow_counter < OVERFLOWS){
        //initialize counter
        overflow_counter = overflow_counter + 1;
    } else {
        //reset counter 
        overflow_counter = 0;
        //update current speed 
        rev_per_min = ticks_to_speed(mean_ticks());
        //set motor output
        MOTOR = pid(target_speed, rev_per_min);
        
        //flash LED
        PD7_switch();
    }
    
    // Restore global interrupt flag
    SREG = sreg;
    
    //return sucessful
    return 1;    
}

// -------- INTERUPT ROUTINE --------------------

// Encoder interrupt
ISR(PCINT1_vect,ISR_BLOCK){
    speed_check();
}

//UART interrupt
ISR(USART_RX_vect)
{
    //deal with messages
    ////testing this idea///
    target_speed = USART_Receive();
    //return character
    USART_Transmit((unsigned char)target_speed);
}

//timer overflow 
ISR(TIMER2_OVF_vect){
    //run PID or just increment overflow_counter
    timer_overflow();
}

// -------- MAIN PROGRAM ------------------------

int main (){
    overflow_counter = 0;
    
    // run initialization routines
    pin_init();
    PWM_init();
    interupt_init();
    clock_init();
    loop_clock_init();
    USART_Init(MYUBRR);
    
    //Set interupt flag
    sei();
    
    while(1){
        ///HEATBEAT///
        //Set PB2 pin to high
        PORTB = ( 1 << PB2 );
        //switch output of PD7
        //PD7_switch();
        //pause for a sec
        _delay_ms(1000);
        
        
        //Set PB2 pin back to low
        PORTB = ( 0 << PB2 );
        //switch output of PD7
        //PD7_switch();
        //pause for a sec
        _delay_ms(1000);
    }
    //return sucess
    return 1;
}
