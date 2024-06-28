#include "stm32f10x.h"

void USART1_Init(uint32_t baudrate);
void USART1_Transmit(uint8_t* buffer, uint32_t len);
void USART1_Receive(uint8_t* buffer, uint32_t len);

int main(void) {
	char* data = "Hello!";
	USART1_Init(9600);
	USART1_Transmit((uint8_t*)data, sizeof(data));
	while(1) {
	}
}

void USART1_Init(uint32_t baudrate) {
	float USARTDIV = ((float)SystemCoreClock) / (16 * (float)baudrate);
	uint32_t DIV_Mantissa = (uint32_t)USARTDIV;
	uint32_t DIV_Fraction = (uint32_t)(16 * (USARTDIV - (float)DIV_Mantissa));
	
	RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;
	RCC->APB2ENR |= RCC_APB2ENR_USART1EN;

	GPIOA->CRH &= ~GPIO_CRH_CNF9;
	GPIOA->CRH |= GPIO_CRH_CNF9_1;
	GPIOA->CRH &= ~GPIO_CRH_MODE9;
	GPIOA->CRH |= GPIO_CRH_MODE9_0 | GPIO_CRH_MODE9_1;

	GPIOA->CRH &= ~GPIO_CRH_CNF10;
	GPIOA->CRH |= GPIO_CRH_CNF10_0;
	GPIOA->CRH &= ~GPIO_CRH_MODE10;

	USART1->BRR = (uint16_t)((DIV_Mantissa << 4) | (DIV_Fraction & 0xF));
	USART1->CR1 |= USART_CR1_TE;
	USART1->CR1 |= USART_CR1_RE;
	USART1->CR1 |= USART_CR1_UE;
}

void USART1_Transmit(uint8_t* buffer, uint32_t len) {
	for (uint32_t i = 0; i < len; i++) {
		while (!(USART1->SR & USART_SR_TXE));
		USART1->DR = *buffer;
		buffer++;
	}
	while (!(USART1->SR & USART_SR_TC));
}

void USART1_Receive(uint8_t* buffer, uint32_t len) {
	for (uint32_t i = 0; i < len; i++) {
		while (!(USART1->SR & USART_SR_RXNE));
		*buffer = USART1->DR & 0xFF;
		buffer++;
	}
}

