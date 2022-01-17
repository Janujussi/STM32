#include "stm32l476xx.h"

//PA.5: Green LED
//PC.13: User Button

#define LED_PIN		5
#define BUTTON_PIN	13


void configure_button_pin() {
	//Enable clock to GPIO Port C
	RCC->AHB2ENR |= RCC_AHB2ENR_GPIOCEN;

	//GPIO Mode
	GPIOC->MODER &= ~(3UL << (2 * BUTTON_PIN)); //Clear the pin, input

	//Speed
	//GPIOC->OSPEEDR &= ~(3 << (2 * BUTTON_PIN)); //Low speed

	//Output Type
	//GPIOA->OTYPER &= ~(1 << LED_PIN); //Push-pull
	GPIOC->PUPDR &= ~(3U<<(2*BUTTON_PIN)); //No pull-up, no pull down

	//EXTI Interrupt Enable
	NVIC_EnableIRQ(EXTI15_10_IRQn);
	NVIC_SetPriority(EXTI15_10_IRQn, 0);

	//Connect External Line to the GPIO
	RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;
	SYSCFG->EXTICR[3] &= ~SYSCFG_EXTICR4_EXTI13;     // SYSCFG external interrupt configuration registers
	SYSCFG->EXTICR[3] |=  SYSCFG_EXTICR4_EXTI13_PC;

	//Interrupt Mask Register
	EXTI->IMR1 |= EXTI_IMR1_IM13;

	//Software interrupt event register
	EXTI->SWIER1 |= EXTI_SWIER1_SWI0;

	//Falling trigger selection register
	EXTI->FTSR1 |= EXTI_FTSR1_FT13;
}

void EXTI15_10_IRQHandler(void) {
	if ((EXTI->PR1 & EXTI_PR1_PIF13) == EXTI_PR1_PIF13) {
		EXTI->PR1 |= EXTI_PR1_PIF13;
		toggle_LED();
	}
}

void configure_LED_pin() {
	// Enable the clock to GPIO Port A
	RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN;

	// GPIO Mode: Input(00), Output(01), AlterFunc(10), Analog(11, reset)
	GPIOA->MODER &= ~(3UL << (2*LED_PIN));
	GPIOA->MODER |=   1UL << (2*LED_PIN);      // Output(01)

	// GPIO Speed: Low speed (00), Medium speed (01), Fast speed (10), High speed (11)
	GPIOA->OSPEEDR &= ~(3 << (2*LED_PIN)); // Low speed (00)

	// GPIO Output Type: Output push-pull (0, reset), Output open drain (1)
	GPIOA->OTYPER &= ~(1 << LED_PIN);      // Push-pull

	// GPIO Push-Pull: No pull-up, pull-down (00), Pull-up (01), Pull-down (10), Reserved (11)
	GPIOA->PUPDR  &= ~(3 << (2*LED_PIN));  // No pull-up, no pull-down
}


void turn_on_LED() {
	GPIOA->ODR |= 1 << LED_PIN;
}

void turn_off_LED() {
	GPIOA->ODR &= ~(1 << LED_PIN);
}

void toggle_LED() {
	GPIOA->ODR ^= (1 << LED_PIN);
}

int main(void){
	//Initializing the LED and button
	configure_LED_pin();
	/*turn_on_LED();
	for (int i = 0; i < 1500000; i++);
	turn_off_LED();
	for (int i = 0; i < 1500000; i++);
	for (int i = 0; i < 20; i++) {
		toggle_LED();
		for (int i = 0; i < 100000; i++);
	}
	turn_off_LED();*/
	configure_button_pin();
}