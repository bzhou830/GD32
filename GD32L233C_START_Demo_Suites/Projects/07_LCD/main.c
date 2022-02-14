#include "gd32l23x.h"
#include "systick.h"
#include "9235.h"
#include "nes_main.h"


int main(void)
{
    systick_config();

    /* enable the  GPIO clock */
    rcu_periph_clock_enable(RCU_GPIOA);
    rcu_periph_clock_enable(RCU_GPIOB);
	
    /* configure  GPIO pin */
    gpio_mode_set(GPIOA, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, BITS(1, 5));
    gpio_output_options_set(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, BITS(1, 5));
    gpio_mode_set(GPIOB, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, BITS(8, 15));
    gpio_output_options_set(GPIOB, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, BITS(8, 15));
    

	  Init_ILI9325();
		nes_main();
    while(1) 
	  {
			Show_RGB(0,240,0,320,Red);
			delay_1ms(100);
			Show_RGB(0,240,0,320,Green);
			delay_1ms(100);
			Show_RGB(0,240,0,320,Blue);
      delay_1ms(100);  
    }
}
