#include "stm32f10x.h"

void GPIO_Init(void);
void TIM2_PWM_Init(void);

int main(void) {
    GPIO_Init();
    TIM2_PWM_Init();

    while (1) {
    }
}

void GPIO_Init(void) {
    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;   // Enable GPIOA clock
    GPIOA->CRL &= ~GPIO_CRL_MODE0;
    GPIOA->CRL |= GPIO_CRL_MODE0_0 | GPIO_CRL_MODE0_1; // Output mode, max speed 50 MHz
    GPIOA->CRL &= ~GPIO_CRL_CNF0;
    GPIOA->CRL |= GPIO_CRL_CNF0_1;       // Alternate function push-pull
}

void TIM2_PWM_Init(void) {
    RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;   // Enable Timer 2 clock

    TIM2->PSC = 1 - 1;                    // frequency = timer frequency
    TIM2->ARR = 100 - 1;                  // Auto-reload register (ARR)
    TIM2->CCR1 = 50;                      // 50% duty cycle

    TIM2->CCMR1 &= ~TIM_CCMR1_OC1M;
    TIM2->CCMR1 |= TIM_CCMR1_OC1M_1 | TIM_CCMR1_OC1M_2; // PWM mode 1
    TIM2->CCMR1 |= TIM_CCMR1_OC1PE;       // Preload enable
    TIM2->CCER |= TIM_CCER_CC1E;          // Enable capture/compare 1 output
    TIM2->CR1 |= TIM_CR1_ARPE;            // Auto-reload preload enable
    TIM2->EGR |= TIM_EGR_UG;              // Update generation
    TIM2->CR1 |= TIM_CR1_CEN;             // Enable Timer 2
}
