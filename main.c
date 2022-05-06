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

/* About Stack */
uint32_t red_stack[40];
uint32_t blue_stack[40];

uint32_t *stack_pointer_red = &red_stack[40];
uint32_t *stack_pointer_blue = &blue_stack[40];


volatile uint32_t tick;
volatile uint32_t _tick;


/* Prototype For Function */
void RCC_Config_HSE(void);
void GPIO_Config(void);
void SysTick_Handler(void);
uint32_t getTick(void);
void DelayS(uint32_t seconds);
void BLUE_MAIN(void);
void RED_MAIN(void);
void BLUE_ON(void);
void BLUE_OFF(void);
void RED_ON(void);
void RED_OFF(void);


int main()
{	
	RCC_Config_HSE();
	GPIO_Config();
	/* Stack For RED_MAIN thread */
	/* 24 is about Excetion Entry in pdf of Cortex-M4 Devices Generic User Guide Generic User Guide  */
	/* IRQ top of stack */
	*(--stack_pointer_red) = (1U << 24); /*xPSR*/
	*(--stack_pointer_red) = (uint32_t)&RED_MAIN; /* PC */
	*(--stack_pointer_red) = 0x0000000DU; /* LR */
	*(--stack_pointer_red) = 0x0000000EU; /* R12 */
	*(--stack_pointer_red) = 0x0000000AU; /* R3 */
	*(--stack_pointer_red) = 0x0000000EU; /* R2 */
	*(--stack_pointer_red) = 0x0000000AU; /* R1 */
	*(--stack_pointer_red) = 0x0000000DU; /* R0 */
	
	/* Stack for BLUE_MAIN thread */
		*(--stack_pointer_blue) = (1U << 24); /*xPSR*/
	*(--stack_pointer_blue) = (uint32_t)&BLUE_MAIN; /* PC */
	*(--stack_pointer_blue) = 0x0000000BU; /* LR */
	*(--stack_pointer_blue) = 0x0000000EU; /* R12 */
	*(--stack_pointer_blue) = 0x0000000EU; /* R3 */
	*(--stack_pointer_blue) = 0x0000000EU; /* R2 */
	*(--stack_pointer_blue) = 0x0000000EU; /* R1 */
	*(--stack_pointer_blue) = 0x0000000FU; /* R0 */
	
	while(1)
	{
		//GPIOD->ODR ^= RED| GREEN| BLUE| ORANGE;  -> For toggle
	}
}

void  BLUE_MAIN(void)
{
	for(;;)
	{
		BLUE_ON();
		DelayS(1);
		BLUE_OFF();
		DelayS(1);	
	}
}

void RED_MAIN(void)
{
	for(;;)
	{
		RED_ON();
		DelayS(1);
		RED_OFF();
		DelayS(1);	
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
	
	SystemCoreClockUpdate(); /* https://www.keil.com/pack/doc/cmsis/Core/html/group__system__init__gr.html */
	SysTick_Config(SystemCoreClock/100U); /* SysTick - RTOS Tick */
	__enable_irq(); /* Enable interrupt to allow Systick to be interrupting and counting the time */ 
	
	
	
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
	_tick = tick;/* This line is critical section. We don't want any interrupt to come and interrupt
	during execution of this line . For more information, go to section 2-12 6:30 */
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


