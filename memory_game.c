#include "stm32l476xx.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

void wait(int n);
void init_LED();
void init_BUTTON();
void init_SERVO1();
void init_SERVO2();
void rand_num(int* array, int len);
void output(int* array, int len);
void input(int* array, int len);
void timer_setup();
int check_correct_answer(int* array1, int* array2, int len);
void winner();
void loser();
void EXTI15_10_IRQHandler();
void TIM4_IRQHandler();
void SysTick_Handler();

int clicks = 0;
int Ticks;

//////////////////////////////MAIN/////////////////////////////////////////////////////////

int main() {
	int len = 5;
	int numbers[len];									// The array used to hold the numbers to output
	int user[len];										// The user's input of numbers
	srand(time(NULL));
	init_LED();											// Sets up the LED
	init_BUTTON();										// Sets up the button
	init_SERVO1();										// Sets up the servo
	init_SERVO2();
	timer_setup();										// Sets up the timer for checking if input is done

	while(1) {
		TIM5->CCR2 &= ~TIM_CCR2_CCR2;
		TIM5->CCR2 |= 40;								// "New Game"
		wait(50000);
		while (clicks > 0) {
			TIM5->CCR2 &= ~TIM_CCR2_CCR2;
			TIM5->CCR2 |= 65;							// "Playing"
			wait(50000);
			clicks = 0;
			rand_num(numbers, len);
			output(numbers, len);
			input(user, len);
			check_correct_answer(user, numbers, len) ? winner() : loser();
			clicks = 0;
		}
	}

	return 0;
}

//////////////////////////////FUNCTIONS////////////////////////////////////////////////////

// Delay function
void wait(int n) {
	for (int i = 0; i < n; i++);
}

// Initialize LED (GPIO A pin 5).
void init_LED() {
	// Enable GPIOA clock
	RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN;

	// General purpose output
	GPIOA->MODER &= ~((1 << 10) | (1 << 11));
	GPIOA->MODER |= GPIO_MODER_MODE5_0;

	// Output push-pull default

	// Low output speed
	GPIOA->OSPEEDR &= ~((1 << 10) | (1 << 11));

	// No PU, PD
	GPIOA->PUPDR &= ~((1 << 10) | (1 << 11));
}

// Initialize button (GPIO C pin 13).
void init_BUTTON() {
	// Enable GPIOC clock
	RCC->AHB2ENR |= RCC_AHB2ENR_GPIOCEN;

	// General purpose input
	GPIOC->MODER &= ~((1 << 26) | (1 << 27));

	// No PU, PD
	GPIOC->PUPDR &= ~((1 << 26) | (1 << 27));

	// Connect EXTI to GPIO
	RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;
	SYSCFG->EXTICR[3] &= ~SYSCFG_EXTICR4_EXTI13;
	SYSCFG->EXTICR[3] |= SYSCFG_EXTICR4_EXTI13_PC;

	// Interrupt mask bit set as unmasked
	EXTI->IMR1 &= ~(1 << 13);
	EXTI->IMR1 |= EXTI_IMR1_IM13;

	// Falling edge trigger
	EXTI->FTSR1 &= ~(1 << 13);
	EXTI->FTSR1 |= EXTI_FTSR1_FT13;

	// Enable NVIC lines
	NVIC_EnableIRQ(EXTI15_10_IRQn);
	NVIC_SetPriority(EXTI15_10_IRQn, 0);
}

// Initialize servo (GPIO A pin 0).
void init_SERVO1() {
	// Alternate Function Mode
	GPIOA->MODER &= ~((1 << 0) | (1 << 1));
	GPIOA->MODER |= (1 << 1);

	// AF1
	GPIOA->AFR[0] &= ~((1 << 0) | (1 << 1) | (1 << 2) | (1 << 3));
	GPIOA->AFR[0] |= (1 << 0);

	// NO PU PD
	GPIOA->PUPDR &= ~((1 << 0) | (1 << 1));

	// Fast Speed
	GPIOA->OSPEEDR &= ~((1 << 0) | (1 << 1));
	GPIOA->OSPEEDR |= (1 << 1);

	// Enable Timer 2
	RCC->APB1ENR1 |= RCC_APB1ENR1_TIM2EN;

	TIM2->PSC = 71;
	TIM2->ARR = 999;

	// UP COUNTING MODE
	TIM2->CR1 &= ~TIM_CR1_DIR;

	// PWM MODE 2 | ENABLE OUTPUT 1 PRELOAD
	TIM2->CCMR1 &= ~TIM_CCMR1_OC1M;
	TIM2->CCMR1 |= TIM_CCMR1_OC1M_0 | TIM_CCMR1_OC1M_1 | TIM_CCMR1_OC1M_2;
	TIM2->CCMR1 |= TIM_CCMR1_OC1PE;

	// ACTIVE LOW
	TIM2->CCER &= ~TIM_CCER_CC1NP;
	TIM2->CCER |= TIM_CCER_CC1P;

	// ENABLE CHANNEL 1 OUTPUT | ENABLE MAIN OUTPUT | ENABLE COUNTER
	TIM2->CCER |= TIM_CCER_CC1E;
	TIM2->BDTR |= TIM_BDTR_MOE;
	TIM2->CR1 |= TIM_CR1_CEN;
}

// Initialize servo (GPIO A pin 1).
void init_SERVO2() {
	// Alternate Function Mode
	GPIOA->MODER &= ~((1 << 2) | (1 << 3));
	GPIOA->MODER |= (1 << 3);

	// AF2
	GPIOA->AFR[0] &= ~((1 << 4) | (1 << 5) | (1 << 6) | (1 << 7));
	GPIOA->AFR[0] |= (1 << 5);

	// NO PU PD
	GPIOA->PUPDR &= ~((1 << 2) | (1 << 3));

	// Fast Speed
	GPIOA->OSPEEDR &= ~((1 << 2) | (1 << 3));
	GPIOA->OSPEEDR |= (1 << 3);

	// Enable Timer 5
	RCC->APB1ENR1 |= RCC_APB1ENR1_TIM5EN;

	TIM5->PSC = 71;
	TIM5->ARR = 999;

	// UP COUNTING MODE
	TIM5->CR1 &= ~TIM_CR1_DIR;

	// PWM MODE 2 | ENABLE OUTPUT 1 PRELOAD
	TIM5->CCMR1 &= ~TIM_CCMR1_OC2M;
	TIM5->CCMR1 |= TIM_CCMR1_OC2M_0 | TIM_CCMR1_OC2M_1 | TIM_CCMR1_OC2M_2;
	TIM5->CCMR1 |= TIM_CCMR1_OC2PE;

	// ACTIVE LOW
	TIM5->CCER &= ~TIM_CCER_CC2NP;
	TIM5->CCER |= TIM_CCER_CC2P;

	// ENABLE CHANNEL 2 OUTPUT | ENABLE MAIN OUTPUT | ENABLE COUNTER
	TIM5->CCER |= TIM_CCER_CC2E;
	TIM5->BDTR |= TIM_BDTR_MOE;
	TIM5->CR1 |= TIM_CR1_CEN;
}

// Generate random numbers.
void rand_num(int* array, int len) {
	for (int i = 0; i < len; i++){
		do {
			array[i] = rand() % 7 + 1;
		} while (array[i] == array[i - 1]);
	}
}

// Servo shows the numbers that are generated
void output(int* array, int len) {
	for (int i = 0; i < len; i++) {
		wait(500000);
		switch (array[i]) {
		case 1:
			TIM2->CCR1 &= ~TIM_CCR1_CCR1;
			TIM2->CCR1 |= 133;
			continue;
		case 2:
			TIM2->CCR1 &= ~TIM_CCR1_CCR1;
			TIM2->CCR1 |= 121;
			continue;
		case 3:
			TIM2->CCR1 &= ~TIM_CCR1_CCR1;
			TIM2->CCR1 |= 107;
			continue;
		case 4:
			TIM2->CCR1 &= ~TIM_CCR1_CCR1;
			TIM2->CCR1 |= 94;
			continue;
		case 5:
			TIM2->CCR1 &= ~TIM_CCR1_CCR1;
			TIM2->CCR1 |= 81;
			continue;
		case 6:
			TIM2->CCR1 &= ~TIM_CCR1_CCR1;
			TIM2->CCR1 |= 67;
			continue;
		case 7:
			TIM2->CCR1 &= ~TIM_CCR1_CCR1;
			TIM2->CCR1 |= 53;
			continue;
		}
	}
	wait(500000);
}

// The number of clicks being put into the user array. Includes timing to index the array
void input(int* array, int len) {
	TIM2->CCR1 &= ~TIM_CCR1_CCR1;								// "Your Turn"
	TIM2->CCR1 |= 41;
	GPIOA->ODR |= (1 << 5);
	wait(100000);
	GPIOA->ODR &= ~(1 << 5);
	wait(100000);
	GPIOA->ODR |= (1 << 5);
	wait(100000);
	GPIOA->ODR &= ~(1 << 5);

	TIM4->CR1 |= TIM_CR1_CEN;									// Enabling the timer to check for last button push
	for (int i = 0; i < len; i++) {
		GPIOA->ODR |= (1 << 5);
		wait(500000);
		GPIOA->ODR &= ~(1 << 5);
		Ticks = 0;
		clicks = 0;
		while (Ticks < 100000) {
			array[i] = clicks;									// The number of clicks gets put into the array
		}
	}
	TIM4->CR1 &= ~TIM_CR1_CEN;
}

// Comparing the generated numbers and the user's input
int check_correct_answer(int* array1, int* array2, int len) {
	for (int i = 0; i < len; i++) {
		if (array1[i] != array2[i]) {
			return 0;
		}
	}


	return 1;
}

void winner() {
	TIM5->CCR2 &= ~TIM_CCR2_CCR2;
	TIM5->CCR2 |= 95;
	for (int i = 0; i < 30; i++){
		GPIOA->ODR ^= (1 << 5);
		wait(50000);
	}
}

void loser() {
	TIM5->CCR2 &= ~TIM_CCR2_CCR2;
	TIM5->CCR2 |= 120;
	wait(1500000);
}

// Setting up the timer checking for last button press
void timer_setup() {
	RCC->APB1ENR1 |= RCC_APB1ENR1_TIM4EN;

	TIM4->PSC = 0;
	TIM4->ARR = 80;

	TIM4->CR1 |= TIM_CR1_URS;
	TIM4->DIER |= TIM_DIER_UIE;
	TIM4->EGR |= TIM_EGR_UG;

	NVIC_EnableIRQ(TIM4_IRQn);
}

void EXTI15_10_IRQHandler() {
	if ((EXTI->PR1 & EXTI_PR1_PIF13) == EXTI_PR1_PIF13) {
		EXTI->PR1 |= EXTI_PR1_PIF13;
		GPIOA->ODR |= (1 << 5);
		clicks++;													// Clicks goes up for every button press
		Ticks = 0;													// Timer gets reset every button press
		wait(5000);
		GPIOA->ODR &= ~(1 << 5);
	}
}

void TIM4_IRQHandler() {
		Ticks++;													// Every interrupt increments this
		TIM4->SR &= ~TIM_SR_UIF;
}
