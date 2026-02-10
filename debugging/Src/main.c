#include "stm32f4xx.h"

int total;

int main (void)
{
	for(int i=0; i<10; i++)
	{
		total += i;
	}
	while(1)
	{

	}
}
