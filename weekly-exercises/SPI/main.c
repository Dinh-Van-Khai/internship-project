#include "stm32f10x.h"

void SPI1_Init(void);
void SPI1_Transmit(uint8_t data);
uint8_t SPI1_Receive(void);

int main(void) {
		uint8_t receivedData;
    SPI1_Init();
	
    // Transmit and receive data
    SPI1_Transmit(0x55);
    receivedData = SPI1_Receive();

    while (1) {
        // Main loop
    }
}

void SPI1_Init(void) {
    // Enable clock for GPIOA and SPI1
    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN | RCC_APB2ENR_SPI1EN;

    // Configure PA5 (SCK), PA6 (MISO), and PA7 (MOSI) as alternate function push-pull
    GPIOA->CRL &= ~(GPIO_CRL_MODE5 | GPIO_CRL_CNF5 |
                    GPIO_CRL_MODE6 | GPIO_CRL_CNF6 |
                    GPIO_CRL_MODE7 | GPIO_CRL_CNF7);
    GPIOA->CRL |= (GPIO_CRL_MODE5_1 | GPIO_CRL_MODE5_0 | GPIO_CRL_CNF5_1 |
                   GPIO_CRL_MODE7_1 | GPIO_CRL_MODE7_0 | GPIO_CRL_CNF7_1);
    GPIOA->CRL |= GPIO_CRL_CNF6_0;  // MISO as input floating

    // Configure PA4 as general purpose output push-pull (for NSS)
    GPIOA->CRL &= ~(GPIO_CRL_MODE4 | GPIO_CRL_CNF4);
    GPIOA->CRL |= GPIO_CRL_MODE4_1 | GPIO_CRL_MODE4_0 | GPIO_CRL_CNF4_0;

    // Configure SPI1 with CPOL = 1, CPHA = 1 (Mode 3)
    SPI1->CR1 = SPI_CR1_MSTR |   // Master mode
                SPI_CR1_BR_0 |   // Baud rate control (fPCLK/4)
                SPI_CR1_SSI |    // Internal slave select (SSI = 1)
                SPI_CR1_SSM |    // Software slave management (SSM = 1)
                SPI_CR1_CPOL |   // Clock polarity (CPOL = 1)
                SPI_CR1_CPHA;    // Clock phase (CPHA = 1)

    SPI1->CR1 |= SPI_CR1_SPE;    // Enable SPI
}

void SPI1_Transmit(uint8_t data) {
    while (!(SPI1->SR & SPI_SR_TXE));
    SPI1->DR = data;
    while (!(SPI1->SR & SPI_SR_RXNE));
}

uint8_t SPI1_Receive(void) {
    while (!(SPI1->SR & SPI_SR_RXNE));
    return SPI1->DR & 0xFF;
}
