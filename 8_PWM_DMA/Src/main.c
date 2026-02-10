
#include "tim.h"


int main(void)
{
	tim2_pa0_pa1_pwm_init();


	while(1)
	{
		for (int i = 1500; i < 2500; i++)
		{
			tim2_pa0_pa1_pwm_dutycycle(1,i);
			pseudo_delay();
		}
		for (int i = 0; i < 1000; i++)
		{
			tim2_pa0_pa1_pwm_dutycycle(1,2500-i);
			pseudo_delay();
		}
	}
}

