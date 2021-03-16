// Serial Comunication Code
// Written by Sam Hince
// For Intro to Mecatronics at Lund Univerity
// 12/14/2019

// ORIGIONAL SERIAL COMUNICATION CODE COPPIED TO MECH 4

//test from 
#include <avr/io.h>
#include <stdio.h>

#define FOSC 1843200 // Clock Speed
#define BAUD 9600
#define MYUBRR FOSC/16/BAUD-1

void USART_Init(unsigned int ubrr){
    //Set baud rate
    UBRR0H = (unsigned char)(ubrr>>8);
    UBRR0L = (unsigned char)ubrr;
    //Enable receiver and transmitter
    UCSR0B = (1<<RXEN0)|(1<<TXEN0);
    // Set frame format: 8data, 2stop bit
    UCSR0C = (1<<USBS0)|(3<<UCSZ00);
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

//probably dont need
void USART_Flush(void){
    unsigned char dummy;
    while ( UCSR0A & (1<<RXC0) ) dummy = UDR0;
}

//main function cuz why not
int main(){
    USART_Init(MYUBRR);
    
    while(1){
        USART_Transmit("a");
        _delay_ms(1000);
    }
    
    //return sucess
    return 1;
}