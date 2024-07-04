#include "stm32f10x.h"

void GPIO_Init(void);
void TIM2_Init(void);
void TIM2_IRQHandler(void);

int main(void) {
    GPIO_Init();
    TIM2_Init();

    while (1) {
    }
}

void GPIO_Init(void) {
    RCC->APB2ENR |= RCC_APB2ENR_IOPCEN;   // Enable GPIOC clock
    GPIOC->CRH &= ~GPIO_CRH_MODE13;
    GPIOC->CRH |= GPIO_CRH_MODE13_0 | GPIO_CRH_MODE13_1; // Output mode, max speed 50 MHz
    GPIOC->CRH &= ~GPIO_CRH_CNF13;       // General purpose output push-pull
}

void TIM2_Init(void) {
    RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;   // Enable Timer 2 clock
    TIM2->PSC = (uint16_t)(SystemCoreClock/10000) - 1;	// Prescaler (PSC) - Timer clock = 10 kHz
    TIM2->ARR = 5000 - 1;                 // Auto-reload register (ARR) - Overflow every 0.5 second (5000 / 10 kHz)
    TIM2->DIER |= TIM_DIER_UIE;           // Enable update interrupt
    TIM2->CR1 |= TIM_CR1_CEN;             // Enable Timer 2

    NVIC_EnableIRQ(TIM2_IRQn);            // Enable Timer 2 interrupt in NVIC
}

void TIM2_IRQHandler(void) {
    if (TIM2->SR & TIM_SR_UIF) {          // Check if update interrupt flag is set
        TIM2->SR &= ~TIM_SR_UIF;          // Clear the update interrupt flag
        GPIOC->ODR ^= GPIO_ODR_ODR13;     // Toggle the LED
    }
}
