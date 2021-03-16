/*
* THIS CODE WRITTEN FOR MECHATRONICS
* While studying abroad in Lund
* By Sam Hince
*/

//PID control code

//initalize global variables
int error_prior = 0;
int integral = 0;
int derivative;
int PWM;

//tuning variables
int iteration_time = 10; //time in miliseconds

Kp = 1;
Ki = 1;
Kd = 1;

int pid(){
    error = desired_value – actual_value;
    integral = integral + (error*iteration_time);
    derivative = (error – error_prior)/iteration_time;
    
    PWM = KP*error + KI*integral + KD*derivative; // + bias;
    
    //store the old error for next iteration
    error_prior = error;
    
    //return sucess
    return 1;
}


int main(){
    while(i){
        //calculate new PID values
        pid();
        //pause for iteration time
        sleep(iteration_time);
    }
    //return sucess
    return 1;
}