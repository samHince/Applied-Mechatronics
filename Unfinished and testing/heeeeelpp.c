#define SHIFT 4
	
int32_t error; //P

int16_t pwm_temp = 0;
uint8_t pwm_val = 0;

uint16_t kp = (1<<(SHIFT-1));
uint16_t ki = (1<<(SHIFT));
	
			

    //T = loops
	//time_stamp = avg
snipp = UINT16_MAX - (uint32_t)avg;
td = (((uint32_t)UINT16_MAX*(OVERFLOWS-1) + (uint32_t)avg + (uint32_t)snipp) << SHIFT)/1000000;
			

speed = speed_temp >> SHIFT;
fp_target = (target << SHIFT);
		
		
static int16_t mult(int16_t x, int16_t y)
{ 
	int32_t temp;
	int16_t result;
	temp = x*y;
	temp = temp + (1 << (SHIFT-1));
	temp = (temp >> SHIFT);
	
	if(temp > INT16_MAX){
		temp = INT16_MAX;
	}
	else if (temp < INT16_MIN)
	{ 
		temp = INT16_MIN;
	}
	
	result = temp;
	return result;
}


		
error = (fp_target - speed_temp);
p_part_temp = fp_multiply(error, kp);
i_part_temp = fp_multiply((uint16_t)e, ki);
i_part_temp = fp_multiply(i_part_temp, (uint16_t)td);
if ((i_part_acc + i_part_temp) < (0xff << N))
{
	i_part_acc += i_part_temp;
}else{
	//i_part_acc = (0xff << N);
}
	
pwm_temp = p_part_temp + i_part_acc + (1 << (N-1));
pwm_temp = pwm_temp >> N;
			
if (pwm_temp > UINT8_MAX){
	pwm_temp = UINT8_MAX;
}else if (pwm_temp < 0){
	pwm_temp = 0;
}
			
pwm_val = pwm_temp;
	OCR0A = pwm_val;
}
		