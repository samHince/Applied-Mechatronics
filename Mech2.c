//// UNDER CONSTRUCTION ////

/*
* [Mech2.c] PC coms
************************************
* THIS CODE WRITTEN FOR MECHATRONICS
* While studying abroad in Lund
* By Sam Hince
*/

#include "serialport.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>



/* baudrate settings are defined in <asm/termbits.h>, which is
 * included by <termios.h> */
#ifndef BAUDRATE
#define BAUDRATE B2400
#endif

#define _POSIX_SOURCE 1		/* POSIX compliant source */

static int fd;
static struct termios oldtio, newtio;
static char *device;

int serial_init(char *modemdevice, int canonical)
{
    /*
     * Open modem device for reading and writing and not as controlling tty
     * because we don't want to get killed if linenoise sends CTRL-C.
     **/
    device = modemdevice;
    fd = open (device, O_RDWR | O_NOCTTY );
    if (fd < 0)
      {
	  perror (device);
	  exit(-1);
      }

    tcgetattr (fd, &oldtio);	/* save current serial port settings */
    memcpy(&newtio, &oldtio, sizeof(newtio));

    cfsetispeed(&newtio, BAUDRATE);
    cfsetospeed(&newtio, BAUDRATE);

    /*
     *CRTSCTS : output hardware flow control (only used if the cable has
     *all necessary lines. )
     *CS8     : 8n1 (8bit,no parity,1 stopbit)
     *CLOCAL  : local connection, no modem contol
     *CREAD   : enable receiving characters
     **/
    newtio.c_cflag &= ~CSIZE;
    newtio.c_cflag &= ~PARENB;
    newtio.c_cflag &= ~CSTOPB;
    newtio.c_cflag &= ~CRTSCTS;
    newtio.c_cflag |=  CS8 | CLOCAL | CREAD;

    /*
     *ICRNL   : map CR to NL (otherwise a CR input on the other computer
     *          may not terminate input)
     *          otherwise make device raw (no other input processing)
     **/
    newtio.c_iflag |=  ICRNL;

#ifndef  SEND_RAW_NEWLINES
    /*
     * Map NL to CR NL in output.
     *                  */
    newtio.c_oflag |= ONLCR;
#else
    newtio.c_oflag &= ~ONLCR;
#endif

    /*
     * ICANON  : enable canonical input, read line-by-line
     **/
    if(canonical) {
	newtio.c_lflag |= ICANON;
#ifdef ECHO
    /* If you use canonical mode to use line editing, you may
     * want to turn on echo of characters to make the edits to show
     * in the sending terminal */
    newtio.c_lflag |= ECHO | ECHOE;
#endif
    } else {
	newtio.c_lflag &= ~ICANON;
    }

    /*
     * ISIG : enable SIGINTR, SIGSUSP, etc..
     **/
    newtio.c_lflag &= ~ISIG;

    /*
     * now clean the modem line and activate the settings for the port
     **/
    tcflush (fd, TCIFLUSH);
    tcsetattr (fd, TCSANOW, &newtio);

    /*
     * terminal settings done, return file descriptor
     **/

    return fd;
}

void serial_cleanup(int ifd){
    if(ifd != fd) {
	    fprintf(stderr, "WARNING! file descriptor != the one returned by serial_init()\n");
    }
#ifdef DRAIN_BEFORE_CLOSE
    /* wait until all chars in buffer have been sent */
    tcdrain(ifd);
#endif
    /* restore the old port settings */
    tcsetattr (ifd, TCSANOW, &oldtio);
    close(ifd);
}

int main(){	
	int fd = serial_init ("/dev/ttyS0" , 0);
	
	if(fd<0){
		printf("%s\n", "Serial port not found");
	}
	else{
		printf("%s\n", "Serial port found");
	}
	
	int e = 0;
	int i = 0;
	int j = 0;
	
	printf("Input a character");
//	scanf("%u", &ts_read);
//	target_speed = (unsigned char)ts_read;
	
//	gettimeofday(&tm1, NULL);
//	while(1){
	    unsigned char msg_send = 'a';
	    unsigned char msg_recieve = '0';
	//	e = serialWrite(fd, &cw, 1);
	//	i = serialWrite(fd, &transmit_speed, 1);
		j = serialWrite(fd, &msg_send, 1);
		if(e < 0 || i <  0 || j < 0)
		{
			printf("\n Error while writing values \n");
			//return -1;
		}
		
	//	e = serialRead(fd, &sw, 1);
	//	i = serialRead(fd, &recieve_speed, 1);
		j = serialRead(fd, &msg_recieve, 1);
		if(e < 0 || i < 0 || j < 0)
		{
			printf("\n Error while reading values \n");
			return -1;
		}
		
//		gettimeofday(&tm2, NULL);
//		transmit_speed = control_loop(target_speed - recieve_speed, P, I, tm2.tv_usec - tm1.tv_usec); // +transmit_speed;
		printf("in: %d \n", msg_recieve);
//		gettimeofday(&tm1, NULL);
		
//	}
	serial_cleanup ( fd );
	
	return 1;
}

