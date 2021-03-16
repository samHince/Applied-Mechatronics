# include "serialport.h"
# include <sys/types.h>
# include <unistd.h>
# include <stdio.h>
# include <string.h>

#define nBytes 5
#define STD_OUT 1   // 0, 1 or 2
#define delayTime 1

void writeSer(int fd){
    // Create a read terminal Message bufferArray - this to read everything from terminal
    char Message[nBytes];
    char readBuf[100];
    // read 100 maximum chars from terminal and add it to the readBuf
    int sizeBuf = read(STD_OUT, readBuf, 100);
    // copy the number of read characters from readBuf to Message
    strncpy(Message,readBuf,sizeBuf);
    // fill the unused chars in Message with #
    int i;
    for( i = sizeBuf-1; i< nBytes; i++){
        Message[i] = '#';
    }

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
}

void readSer(int fd){
    printf("%s \n", "readSer: Something Recived");
    // Create a read serial Message bufferArray
    char Message[nBytes];
    // Read letters one by one from COM port and add to Message
    int i;
    for(i = 0; i < nBytes; i++){
        char currentChar;
        int charsRead;
        //check if characters are read correctly. If to little chars are available, print out error
        charsRead = read(fd, &currentChar,1);
        if (charsRead<0){
            printf("%s \n", "readSer: Issue With Read Serial");
        }
        //if no issues, continue with adding currentChar to Message
        else{
                strncpy(Message+i, &currentChar,1);
        }
    }
    // Write Message Buffer to Terminal
    write(STD_OUT, Message, nBytes);
    write(STD_OUT,"\n",1);

}

int main () {
    //open 
    int fd = serial_init("/dev/ttyS0", 0);
    if(fd<0){
        printf("%s\n", "Serial port not found");
    }
    else {
         printf("%s\n", "Serial port found");
    }
        // Constantly Polling 
    while(1){
        writeSer(fd);
        printf("\n%s\n", "Have Written Waiting to read");
        readSer(fd);
    }       
    serial_cleanup(fd);
}
