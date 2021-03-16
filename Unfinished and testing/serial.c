// Serial Comunication Code
// Written by Sam Hince
// For Intro to Mecatronics at Lund Univerity
// 12/14/2019

//ORIGIONAL CODE TO SEND DATA OVER SERIAL. COPPIED TO MECH2

//help with serial: https://github.com/xanthium-enterprises/Serial-Port-Programming-on-Linux/blob/master/USB2SERIAL_Write/Transmitter%20(PC%20Side)/SerialPort_write.c


//# include <termios.h> /* POSIX Terminal Control Definitions */
# include <unistd.h>  /* UNIX Standard Definitions 	   */ 
# include <errno.h>   /* ERROR Number Definitions           */

//include useful header files
# include <fcntl.h>
# include <stdio.h>

//make a few definitions for ease later
# define size 4

//declare global variables
int ser;
char term_buff[size]; //make buffer to hold terminal input

//function to open up serial comunication
int UART_init(){
	//open communication with the serial port through some whithcraft
	ser = open("/dev/ttyS0", 0);
	
	//check for sucess
	if(ser<0){
		printf("%s\n", "Serial port not found");
	}
	else{
		printf("%s\n", "Serial port found");
	}

	//return complete
	return 1;
}

//read message from term -- yes, this is overly simple, but it works
int termRead(){
        //read input from term
        for(int i = 0; i < size; i++){
                char term = getchar();
                term_buff[i] = term;
        }
        return 1;
}

//write meg over port
int sendMsg(){
	int  bytes_written  = 0;  	/* Value for storing the number of bytes written to the port */ 

	bytes_written = write(ser, term_buff, sizeof(term_buff));		/* use write() to send data to port                                            */
										/* "fd"                   - file descriptor pointing to the opened serial port */
									     	/*	"write_buffer"         - address of the buffer containing data	            */
									     	/* "sizeof(write_buffer)" - No of bytes to write                               */	
	//printf("\n  %s written to serial port", term_buff);
	//printf("\n  %d Bytes written to serial port", bytes_written);
	//printf("\n +----------------------------------+\n\n");

	return 1;
}

/*void sendMsg(int fd){
	char Message_buff[size];
	
	// copy the number of read characters from readBuf to Message
        strncpy(message_buff,readBuf,sizeBuf);

	
	//send message over UART
	
	
	
	printf("%s \n", "writeSer: Confirming the message to be sent to be: ");
	write(STD_OUT, Message, nBytes);
	printf(" \n %s \n", "writeSer: Sending the characters letter by letter as:");
	// Write the message to the COM port letter by letter

	for (i = 0; i<nBytes; i++){

        // Sending text to terminal as what being sent to serial
		// printf("%s \n", "writeSer: writing following to serial -> ");
        	write(STD_OUT,&Message[i],1);

        	// Actually writing to serial
        	write(fd,&Message[i],1);
        	//sleep(delayTime);
	}
}*/

int main() {
	int toSend = 100; //rpm
	UART_init();

//	while(1){
		//sudo code
//		clear buffer
		
		// 
		termRead();

//		itoa(n, buffer, 10);
//		UART_sendstring(buffer);
//	}
	close(ser);/* Close the Serial port */
//	serial_cleanup();
}
