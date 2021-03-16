 /*
 * AVR___Controller.c
 *
 * Created: 2019-11-14 10:50:10
 *  Author: tmk19wh
 */

//-------------------------------------------------------Includes

#include <avr/io.h>
#include <stdio.h>
#include <stdlib.h>
#include <avr/interrupt.h>

#include <string.h>



//-------------------------------------------------------Defines
#define PWMpwr OCR0B
#define BAUD 2400 //Used for USART
#define FOSC 1000000 // 6 0's
#define MYUBRR FOSC/16/BAUD-1
#define nBytes 5
#define Binaryprescaler 3

//------------------------------------------------------Filter Defines
#define filterSizeExp 5
#define filterSize 32
#define minCountsPerInterrupt 400 //Defined By matlab FOR HIGH SPEEDS
#define maxCountsPerInterrupt 120000 //Defined By matlab

//------------------------------------------------------PID Defines
#define Kp 1
#define Ki 0.01




//-------------------------------------------------------Variables
volatile int counter = 100;
int lastA = 0;
// the timestamp array gets looped with the latest readings continually.
unsigned int ClockStampArray[filterSize];
int ClockStampCounter = 0;
char TargetSpeedBuffer[nBytes-1];
int writingSpeedAtPos = 5;

int AvgSpeed = 0;
int TargetSpeed = 60;
int u = 0;
int Integral = 0;



//-------------------------------------------------------Setups
int setupClock(){
// Setup clock in the TCCR1B register Set CS11 to 1 for /8 prescaling
TCCR1B |= (1<<CS11);
TCCR1A = 0b00000000; //"Normal mode", the counter is just counting.
// TIMSK1= (1<<TOIE1);//Overflow Interrupt Enable
return 1;
}

int setupLED()
{
DDRD |= ((1<<PD2)|(1<<PD3));
DDRB |= (1<<PB7);
return 1;
}

int setupPWM()
{
//PWM PINS are (PWM POWER PD5 pin11 - OCR0A) and (PWM POS PD6 pin 12 OCR0B)

DDRD |= ((1<<PD5)|(1<<PD6)); // Defining the pins as ouputs
TCCR0A |= ((1<<COM0A1)|(1<<COM0A0)|(1<<COM0B1)|(1<<COM0B0)|(1<<WGM01)|(1<<WGM00));
TCCR0B |= (1<<CS00); //Setting clock no prescaler
return 1;
}

void setupUSART( unsigned int ubrr){
// Setup Baud Rate
UBRR0H = (unsigned char)(ubrr>>8);
UBRR0L = (unsigned char)ubrr;

//Setup TXEN (transmit enable) in the UCSRnB
//Enable recieve RXEN and transmit TXEN in the UCSR0B
UCSR0B = (1<<RXEN0) | (1<<TXEN0);

// Set frame format to 8data, 2 stop bits
UCSR0C = (1<<USBS0) | (3<<UCSZ00);

// Enable Usart Recieve Complete Interrupt USART_RX
UCSR0B |= (1<<RXCIE0);
UCSR0A |= (1<<RXC0);

}

int setupINTERRUPT(){
//PCINT9 PCINT10 for encoder
// enable PCIE1
PCICR |= (1<<PCIE1);
// enable PCINT9 and PCINT10
PCMSK1 |= ((1<<PCINT9)|(1<<PCINT10));
// enable interrupts
sei();
return 1;
}



//-------------------------------------------------------Functions
unsigned int readClock( void ){
unsigned char sreg;
unsigned int i;
/* Save global interrupt flag */
sreg = SREG;
/* Disable interrupts */
cli();
/* Read TCNT1 into i */
i = TCNT1;
/* Restore global interrupt flag */
SREG = sreg;
return i;
}
int resetClock(){
//TIM16_WriteTCNT1
unsigned char sreg;
// set i as the time to write to the clock
unsigned int i = 0;
/* Save global interrupt flag */
sreg = SREG;
/* Disable interrupts */
cli();
/* Set TCNT1 to i */
TCNT1 = i;
/* Restore global interrupt flag */
SREG = sreg;
return 1;
}
int turnOnLED(int i)
{
if (i == 1){
PORTD |= (1<<PD2);
}
else if (i == 2){
PORTD |= (1<<PD3);

}
else if (i == 3){
PORTB |= (1<<PD7);
}
return 1;
}
int turnOffLED(int i)
{
if (i == 1){
PORTD &= ~(1<<PD2);
}
else if (i == 2){
PORTD &= ~(1<<PD3);

}
else if (i == 3){
PORTB &= ~(1<<PD7);
}
return 1;
}
void USART_Transmit (char data){
// wait for empty transmit buffer
while( !( UCSR0A & (1<<UDRE0)));
//put data into buffer, sends the data
UDR0 = data;
}
unsigned char USART_Recieve (void){
//wait for data to be recieved
while( !(UCSR0A & (1<<RXC0)));
//Get and return recieved data from buffer
return UDR0;
}

int calcAvgSpeed(){
int i;
long sumClockTicks = 0;
int avgClockTicks;

// Sum up all the number of ClockTicks from the array over the filtersize
for(i=0; i<filterSize ; i++){
sumClockTicks = sumClockTicks + ClockStampArray[i];
}

// CAlculate average clock ticks divide by Filtersize by bitshifting;
avgClockTicks = sumClockTicks >> filterSizeExp;

// The whole calculation from AvgClockTicks/interrupts to RPM in a factor K Divide with prescale 8 --> >>3
long k = 625000;
AvgSpeed = (k / avgClockTicks) >> Binaryprescaler;

return 1;
}

void sendSpeed(){
// calc avg speed when requested

calcAvgSpeed();

char buf[nBytes+1]; //+1 for later function terminating null
// Sends the speed as a nBytes long message over the COM port
int i;


// puts AvgSpeed Into buf as characters One extra space for terminating null
int length = snprintf(buf,nBytes+1,"%d",AvgSpeed);

// Fills the last spots of the buffer with #
for( i = length ; i < nBytes  ; i++ ){
buf[i] = '#';
}

for(i = 0; i<nBytes; i++ ){
USART_Transmit(buf[i]);
}
}
void calcTargetSpeed(){
int i;
char buf[nBytes-1];
int counter = 0;

// Checking how many numbers are in the TargetSpeedBuffer in a row
for(i = 0; i<nBytes-1; i++){

// if its a NUMBER-character, add 1 to counter.
if(TargetSpeedBuffer[i] > 47 && TargetSpeedBuffer[i] < 58){
counter++;
}

// if a # is encountered, stop counting
if(TargetSpeedBuffer[i] == '#'){
break;
}
}

// Taking the numbers from the TargetSpeedBuffer at the length of COUNTER and converting them to Int
strncpy(buf,TargetSpeedBuffer,counter);
TargetSpeed = atoi(buf);


}
void sendTargetSpeed(){

calcTargetSpeed();

char buf[nBytes+1]; //+1 for later function terminating null
// Sends the targetSpeed as a nBytes long message over the COM port
int i;

// puts TargetSpeed Into buf as characters One extra space for terminating null
int length = snprintf(buf,nBytes+1,"%d",TargetSpeed);

// Fills the last spots of the buffer with #
for( i = length ; i < nBytes  ; i++ ){
buf[i] = '#';
}

for(i = 0; i<nBytes; i++ ){
USART_Transmit(buf[i]);
}
}
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
void setPWR(int PWR){
OCR0A = PWR;
OCR0B = PWR;
}

void PI(){
calcAvgSpeed();


// P

int e = TargetSpeed-AvgSpeed;


Integral = Integral + e ;
if (abs(e)>10)
{
turnOnLED(1);
}
if (abs(e)<10)
{
turnOffLED(1);
}

if(abs(e) < 0.5){
e=0;
turnOnLED(3);
}
if(abs(e) > 0.5){
turnOffLED(3);
}

u = Kp * e + Ki * Integral;

// Saturation filter
if(u>255){
u=255;
}
if (u<0){
u=0;
}
setPWR(u);
};
//-------------------------------------------------------Interrupts+

// Encoder Interrupts TODOO
ISR(PCINT1_vect,ISR_BLOCK){
addClockTicks();
}

// USART READ interrupt
ISR(USART_RX_vect){
unsigned char  RecievedByte = USART_Recieve();

if(RecievedByte == 'r'){
sendSpeed();
writingSpeedAtPos = nBytes;
}

else if(RecievedByte == 't'){
sendTargetSpeed();
writingSpeedAtPos = nBytes;
}


// if Write new target speed - Set WritingSpeed to 1
else if(RecievedByte == 'w'){
writingSpeedAtPos = 0;
}
else if(RecievedByte == '#'){

if(writingSpeedAtPos<nBytes){
TargetSpeedBuffer[writingSpeedAtPos] = '#';
}
if(writingSpeedAtPos == nBytes-2){
sendTargetSpeed();
}
writingSpeedAtPos++;

// Recalculate target Speed always
calcTargetSpeed();
}

// if RecievedByte is a number (ASCII - char) and we should be writing it, put it at the current position.
else if(RecievedByte > 47 && RecievedByte < 58 && writingSpeedAtPos < nBytes-1){
TargetSpeedBuffer[writingSpeedAtPos] = RecievedByte;

if(writingSpeedAtPos == nBytes-2){
sendSpeed();
}

// increasing the position to write on one step up
writingSpeedAtPos++;


}



}

//-------------------------------------------------------Main----------------------------------------------
int main(void)
{
//------Run Setups------
setupClock();
setupLED();
setupUSART(MYUBRR); // Setup before INTERRUPT init!
setupINTERRUPT();
setupPWM();


//PWMpwr = 0;


while(1)
{
PI();
//OCR0A = TargetSpeed;
//OCR0B = TargetSpeed;
}
}