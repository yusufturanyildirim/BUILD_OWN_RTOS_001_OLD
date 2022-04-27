#include <stm32f407xx.h>

#define RED    (1U<<14)
#define GREEN  (1U<<12)
#define BLUE   (1U<<15)
#define ORANGE (1U<<13)

#define RED_BIT    (1U<<28)
#define GREEN_BIT  (1U<<24)
#define BLUE_BIT   (1U<<30)
#define ORANGE_BIT (1U<<26)

#define GPIOD_CLOCK (1<<3)


volatile uint32_t tick;
volatile uint32_t _tick;


/* Prototype For Function */
void RCC_Config_HSE(void);
void GPIO_Config(void);
void SysTick_Handler(void);
uint32_t getTick(void);
void DelayS(uint32_t seconds);
void BLUE_ON(void);
void BLUE_OFF(void);
void RED_ON(void);
void RED_OFF(void);

/* Function Mains */

int  BLUE_MAIN(void)
{
	for(;;)
	{
		BLUE_ON();
		DelayS(1);
		BLUE_OFF();
		DelayS(1);	
	}
}

int RED_MAIN(void)
{
	for(;;)
	{
		RED_ON();
		DelayS(1);
		RED_OFF();
		DelayS(1);	
	}
}




int main()
{
	uint32_t volatile start = 0U;
	
	RCC_Config_HSE();
	GPIO_Config();

	if(start)
	{
		BLUE_MAIN();
	}
	else
	{
		RED_MAIN();
	}
	
	
	while(1)
	{
		//GPIOD->ODR ^= RED| GREEN| BLUE| ORANGE;  -> For toggle
	}
}
/*
 * HSE ON and System Freq. 16MHz
 */

void RCC_Config_HSE()
{
	RCC->AHB1ENR |= GPIOD_CLOCK;
	RCC->CR |= 1 << 16;
	while(!((RCC->CR) & (1<<17)));
	
	SystemCoreClockUpdate();
	SysTick_Config(SystemCoreClock/100U);
	__enable_irq();
	
	
	
}
void GPIO_Config(void)
{
	GPIOD->MODER |= RED_BIT| GREEN_BIT| BLUE_BIT| ORANGE_BIT;
	
}

void SysTick_Handler(void)
{
	++tick;
}

uint32_t getTick(void)
{
	__disable_irq();
	_tick = tick;/* This Commond line is so important for us. So in this moment we dont want to any interrupt . 
	So we did disable all irqs */
	__enable_irq();
	
	return _tick;
}

void DelayS(uint32_t seconds)
{
	seconds *= 100;
	uint32_t temp = getTick();
	while((getTick() - temp)< seconds){}

}

void BLUE_ON(void)
{
	GPIOD->ODR |=BLUE;
}

void BLUE_OFF(void)
{
	GPIOD->ODR &=~BLUE;
}

void RED_ON(void)
{
	GPIOD->ODR |= RED;
}

void RED_OFF(void)
{
	GPIOD->ODR &=~RED;
}
