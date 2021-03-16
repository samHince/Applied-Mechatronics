/*
* [Mech4_5.c] bounce back coms test
************************************
* THIS CODE WRITTEN FOR MECHATRONICS
* While studying abroad in Lund
* By Sam Hince
*/

//intended for use with hyper term for send recieve
//2400 
//two stop bits
//no parity or hardware checking

//test from
#include <avr/io.h>
#include <stdio.h>
#include <util/delay.h>

//define constants for future use
#define FOSC 1000000UL //clock speed
#define BAUD 2400 //9600
#define MYUBRR (FOSC/ (16UL * BAUD)) - 1

// -------- INITIALIZATIONS ---------------------
void USART_Init(unsigned int ubrr){
    //Set baud rate
    UBRR0H = (unsigned char)(ubrr>>8);
    UBRR0L = (unsigned char)ubrr;
    //Enable receiver and transmitter
    UCSR0B = (1<<RXEN0)|(1<<TXEN0);
    // Set frame format: 8data, 2stop bit
    UCSR0C = (1<<USBS0)|(3<<UCSZ00);
}

int pin_init(){
    // Set D as output
    DDRD = 0xFF;
    
    // Set encoder input
    DDRC = 0x00;
    
    // Return successful
    return 1;
}

// -------- FUNCTIONS -------------------------
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

int PD7_on(){
    PORTD = ( 1 << PD7 );
    
    // Return successful
    return 1;
}

// -------- MAIN PROGRAM ------------------------
int main (void){
    pin_init();
    USART_Init(MYUBRR);
    PD7_on();
    while(1) {
        //recieve character
        unsigned char a = USART_Receive();
        //return character
        USART_Transmit(a);
    } //end while
}  // end main
