/*
* [Mech4.c] AVR coms test - send only
************************************
* THIS CODE WRITTEN FOR MECHATRONICS
* While studying abroad in Lund
* By Sam Hince
*/

//test from
#include <avr/io.h>
#include <stdio.h>
#include <util/delay.h>

// #define FOSC 1843200// Clock Speed
#define FOSC 1000000UL
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
    
    //send continuously
    while(1){
        // Wait for empty transmit buffer
        while ( !( UCSR0A & (1<<UDRE0)) );
        // Put data into buffer, sends the data
        unsigned char a[14] = "Test! ";
        for(int i = 0; i <= 13; i++){
            USART_Transmit(a[i]);
            PD7_switch();
            _delay_ms(1000);
        }
    } //end while
}  // end main
