#include "stm32l476xx.h"
#include <stdlib.h>

void init_LED();
void init_SERVO();
void initialize_timers();
void SERVO_LEFT(int* brightness);
void SERVO_MIDDLE(int* brightness);
void SERVO_RIGHT(int* brightness);

////////////////////////////// MAIN /////////////////////////////////

int main() {
	int brightness = 500; // value LED first brightens to

	init_LED();
	init_SERVO();
	initialize_timers();

	SERVO_MIDDLE(&brightness);
	for (int i = 0; i < 500000; i++);
	SERVO_LEFT(&brightness);
	for (int i = 0; i < 500000; i++);
	SERVO_MIDDLE(&brightness);
	for (int i = 0; i < 500000; i++);
	SERVO_RIGHT(&brightness);
	for (int i = 0; i < 500000; i++);
	SERVO_MIDDLE(&brightness);
}

////////////////////////////// FUNCTIONS /////////////////////////////////

void init_LED() {
	// GPIOA ENABLE
	RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN;

	// AF MODE
	GPIOA->MODER &= ~((1 << 10 | 1 << 11));
	GPIOA->MODER |= (1 << 11);

	// AF1
	GPIOA->AFR[0] &= ~((1 << 20) | (1 << 21) | (1 << 22) | (1 << 23));
	GPIOA->AFR[0] |=  (1 << 20);

	// NO PU PD
	GPIOA->PUPDR &= ~((1 << 10) | (1 << 11));

	// HIGH SPEED
	GPIOA->OSPEEDR &= ~((1 << 10) | (1 << 11));
	GPIOA->OSPEEDR |= (1 << 11);
}

void init_SERVO() {
	// AF MODE
	GPIOA->MODER &= ~((1 << 0) | (1 << 1));
	GPIOA->MODER |= (1 << 1);

	// AF2
	GPIOA->AFR[0] &= ~((1 << 0) | (1 << 1) | (1 << 2) | (1 << 3));
	GPIOA->AFR[0] |= (1 << 1);

	// NO PU PD
	GPIOA->PUPDR &= ~((1 << 0) | (1 << 1));

	// Medium Speed
	GPIOA->OSPEEDR &= ~((1 << 0) | (1 << 1));
	GPIOA->OSPEEDR |= (1 << 0);
}

void initialize_timers() {
	// ENABLE TIM2 | ENABLE TIM5
	RCC->APB1ENR1 |= RCC_APB1ENR1_TIM2EN | RCC_APB1ENR1_TIM5EN;

	// CONFIGURE TIMER 2 CHANNEL 1 ////////////////////////////////////////////////////////////////////////////////
	TIM2->PSC = 99;
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

	// CONFIGURE TIMER 5 CHANNEL 1 /////////////////////////////////////////////////////////////////////////////////
	TIM5->PSC = 71;
	TIM5->ARR = 999;

	// UP COUNTING MODE
	TIM5->CR1 &= ~TIM_CR1_DIR;

	// PWM MODE 2 | ENABLE OUTPUT 1 PRELOAD
	TIM5->CCMR1 &= ~TIM_CCMR1_OC1M;
	TIM5->CCMR1 |= TIM_CCMR1_OC1M_0 | TIM_CCMR1_OC1M_1 | TIM_CCMR1_OC1M_2;
	TIM5->CCMR1 |= TIM_CCMR1_OC1PE;

	// ACTIVE LOW
	TIM5->CCER &= ~TIM_CCER_CC1NP;
	TIM5->CCER |= TIM_CCER_CC1P;

	// ENABLE CHANNEL 1 OUTPUT | ENABLE MAIN OUTPUT | ENABLE COUNTER
	TIM5->CCER |= TIM_CCER_CC1E;
	TIM5->BDTR |= TIM_BDTR_MOE;
	TIM5->CR1 |= TIM_CR1_CEN;
}

void SERVO_LEFT(int* brightness) {
	int j = *brightness;
	TIM5->CCR1 = 130;
	for (int i = j; i > 0; i--) {
		*brightness = i;
		TIM2->CCR1 = i;
		for (int y = 0; y < 500; y++);
	}
}

void SERVO_MIDDLE(int* brightness) {
	int j = *brightness;
	TIM5->CCR1 = 75;
	if (*brightness < 400) {
		for (int i = j; i < 401; i++) {
			*brightness = i;
			TIM2->CCR1 = i;
			for (int y = 0; y < 500; y++);
		}
	} else {
		for (int i = j; i > 399; i--) {
			*brightness = i;
			TIM2->CCR1 = i;
			for (int y = 0; y < 500; y++);
		}
	}
}

void SERVO_RIGHT(int* brightness) {
	int j = *brightness;
	TIM5->CCR1 = 20;
	for (int i = j; i < 1000; i++) {
		*brightness = i;
		TIM2->CCR1 = i;
		for (int y = 0; y < 500; y++);
	}
}