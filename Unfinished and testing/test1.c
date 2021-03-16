# include <avr/io.h>
main () {
    DDRB = 0xF0 ;
    while(1){
        unsigned char tmp = (PINB & 0x0F);
        PORTB = (tmp << 4);
    }
}