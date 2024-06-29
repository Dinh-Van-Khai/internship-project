#include "stm32f10x.h"

void I2C1_Init(void);
void I2C1_Transmit(uint8_t deviceAddr, uint8_t* data, uint8_t length);
void I2C1_Receive(uint8_t deviceAddr, uint8_t* data, uint8_t length);

int main(void) {
	char* data = "Hello!";
	I2C1_Init();
	I2C1_Transmit(0x1,(uint8_t*)data, sizeof(data));
	while(1) {
	}
}

void I2C1_Init(void) {
	RCC->APB2ENR |= RCC_APB2ENR_IOPBEN;
	RCC->APB1ENR |= RCC_APB1ENR_I2C1EN;

	GPIOB->CRL &= ~GPIO_CRL_CNF6;
	GPIOB->CRL |= GPIO_CRL_CNF6_0 | GPIO_CRL_CNF6_1;
	GPIOB->CRL &= ~GPIO_CRL_MODE6;
	GPIOB->CRL |= GPIO_CRL_MODE6_0 | GPIO_CRL_MODE6_1;

	GPIOB->CRL &= ~GPIO_CRL_CNF7;
	GPIOB->CRL |= GPIO_CRL_CNF7_0 | GPIO_CRL_CNF7_1;
	GPIOB->CRL &= ~GPIO_CRL_MODE7;
	GPIOB->CRL |= GPIO_CRL_MODE7_0 | GPIO_CRL_MODE7_1;

	// Thiết lập thanh ghi CR2 cho tốc độ 100kHz
	I2C1->CR2 &= ~I2C_CR2_FREQ;
	I2C1->CR2 |= 8; // 36: tần số tín hiệu APB1 = 36 MHz

	// Thiết lập thanh ghi CCR và CR1 cho I2C1
	I2C1->CCR &= ~I2C_CCR_FS;
	I2C1->CCR |= I2C_CCR_FS; // Chế độ chuẩn
	I2C1->CCR &= ~I2C_CCR_DUTY;
	I2C1->CCR |= 0x28; // Tốc độ baudrate 100kHz
	I2C1->CR1 |= I2C_CR1_PE; // Bật I2C1
}

void I2C1_Transmit(uint8_t deviceAddr, uint8_t* data, uint8_t length) {
	// Đợi cho tới khi Bus I2C sẵn sàng để truyền
	while(I2C1->SR2 & I2C_SR2_BUSY);

	// Gửi START condition
	I2C1->CR1 |= I2C_CR1_START;
	while(!(I2C1->SR1 & I2C_SR1_SB));

	// Gửi địa chỉ thiết bị và bit ghi
	I2C1->DR = (deviceAddr << 1) & 0xFF;
	while(!(I2C1->SR1 & I2C_SR1_ADDR));
	(void) I2C1->SR2;

	// Gửi dữ liệu
	for (uint8_t i = 0; i < length; i++) {
		I2C1->DR = data[i];
		while (!(I2C1->SR1 & I2C_SR1_TXE));
	}

	// Gửi STOP condition
	I2C1->CR1 |= I2C_CR1_STOP;
}

void I2C1_Receive(uint8_t deviceAddr, uint8_t* data, uint8_t length) {
	// Đợi cho tới khi Bus I2C sẵn sàng để truyền
	while(I2C1->SR2 & I2C_SR2_BUSY);

	// Gửi START condition
	I2C1->CR1 |= I2C_CR1_START;
	while(!(I2C1->SR1 & I2C_SR1_SB));

	// Gửi địa chỉ thiết bị và bit đọc
	I2C1->DR = ((deviceAddr << 1) | 0x01) & 0xFF;
	while(!(I2C1->SR1 & I2C_SR1_ADDR));
	(void) I2C1->SR2;

	// Nhận dữ liệu
	for (uint8_t i = 0; i < length; i++) {
		if (i == length - 1) {
			I2C1->CR1 &= ~I2C_CR1_ACK;
			I2C1->CR1 |= I2C_CR1_STOP;
		}

		while (!(I2C1->SR1 & I2C_SR1_RXNE));
		data[i] = I2C1->DR & 0xFF;
	}

	I2C1->CR1 |= I2C_CR1_ACK;
}
