/*
 * Copyright 2020 Kevin Weiss for HAW Hamburg
 *
 * This file is subject to the terms and conditions of the MIT License. See the
 * file LICENSE in the top level directory for more details.
 * SPDX-License-Identifier:    MIT
 */

/*
 ******************************************************************************
 * @file			port_nucleo_f103rb.h
 * @author			Kevin Weiss
 * @date			12.03.2020
 * @brief			nucleo_f103rb specific porting.
 ******************************************************************************
 */

#ifndef PORT_NUCLEO_F103RB_H_
#define PORT_NUCLEO_F103RB_H_

/******************************************************************************/
/* Pin and Ports defines ******************************************************/
/******************************************************************************/
#define LED0_Pin GPIO_PIN_5
#define LED0_GPIO_Port GPIOA

#define DEBUG0_Pin GPIO_PIN_3
#define DEBUG0_GPIO_Port GPIOB
#define DEBUG1_Pin GPIO_PIN_4
#define DEBUG1_GPIO_Port GPIOB
#define DEBUG2_Pin GPIO_PIN_2
#define DEBUG2_GPIO_Port GPIOD

/******************************************************************************/
#define TEST_PASS_Pin GPIO_PIN_4
#define TEST_PASS_GPIO_Port GPIOA
#define TEST_WARN_Pin GPIO_PIN_0
#define TEST_WARN_GPIO_Port GPIOA
#define TEST_FAIL_Pin GPIO_PIN_1
#define TEST_FAIL_GPIO_Port GPIOA

#define USER_BTN_Pin GPIO_PIN_13
#define USER_BTN_GPIO_Port GPIOC
#define DUT_RST_Pin GPIO_PIN_2
#define DUT_RST_GPIO_Port GPIOB

/******************************************************************************/
#define DUT_ADC_Pin GPIO_PIN_0
#define DUT_ADC_GPIO_Port GPIOB

#define PM_LO_ADC_Pin GPIO_PIN_1
#define PM_LO_ADC_GPIO_Port GPIOC
#define PM_HI_ADC_Pin GPIO_PIN_2
#define PM_HI_ADC_GPIO_Port GPIOC
#define PM_V_ADC_Pin GPIO_PIN_0
#define PM_V_ADC_GPIO_Port GPIOC

/******************************************************************************/
#define DUT_TX_Pin GPIO_PIN_9
#define DUT_TX_GPIO_Port GPIOA
#define DUT_RX_Pin GPIO_PIN_10
#define DUT_RX_GPIO_Port GPIOA
#define DUT_CTS_Pin GPIO_PIN_11
#define DUT_CTS_GPIO_Port GPIOA
#define DUT_RTS_Pin GPIO_PIN_12
#define DUT_RTS_GPIO_Port GPIOA

/******************************************************************************/
#define DUT_IC_Pin GPIO_PIN_8
#define DUT_IC_GPIO_Port GPIOA

/******************************************************************************/
#define IF_TX_Pin GPIO_PIN_2
#define IF_TX_GPIO_Port GPIOA
#define IF_RX_Pin GPIO_PIN_3
#define IF_RX_GPIO_Port GPIOA

/******************************************************************************/
#define DUT_NSS_Pin GPIO_PIN_12
#define DUT_NSS_GPIO_Port GPIOB
#define DUT_SCK_Pin GPIO_PIN_13
#define DUT_SCK_GPIO_Port GPIOB
#define DUT_MISO_Pin GPIO_PIN_14
#define DUT_MISO_GPIO_Port GPIOB
#define DUT_MOSI_Pin GPIO_PIN_15
#define DUT_MOSI_GPIO_Port GPIOB

/******************************************************************************/
#define DUT_SCL_Pin GPIO_PIN_8
#define DUT_SCL_GPIO_Port GPIOB
#define DUT_SDA_Pin GPIO_PIN_9
#define DUT_SDA_GPIO_Port GPIOB

/******************************************************************************/
#define DUT_PWM_Pin GPIO_PIN_8
#define DUT_PWM_GPIO_Port GPIOC
#define DUT_DAC_Pin GPIO_PIN_9
#define DUT_DAC_GPIO_Port GPIOC

/******************************************************************************/
/* GPIO defines ***************************************************************/
/******************************************************************************/
#define LED_OFF 				GPIO_PIN_RESET

#define GPIO_DEBUG0_INT			EXTI3_IRQHandler
#define GPIO_DEBUG0_IRQ			EXTI3_IRQn
#define GPIO_DEBUG1_INT			EXTI4_IRQHandler
#define GPIO_DEBUG1_IRQ			EXTI4_IRQn
#define GPIO_DEBUG2_INT			EXTI2_IRQHandler
#define GPIO_DEBUG2_IRQ			EXTI2_IRQn

#define GPIO_NSS_CTS_INT		EXTI15_10_IRQHandler
#define GPIO_NSS_CTS_IRQ		EXTI15_10_IRQn

/******************************************************************************/
/* BOARD defines **************************************************************/
/******************************************************************************/
#define RCC_HSE_STATE			RCC_HSE_BYPASS

#define BOARD_ID				0

/******************************************************************************/
/* SPI defines ****************************************************************/
/******************************************************************************/
#define DUT_SPI_INST			SPI2

#define DUT_SPI_CLK_EN()		__HAL_RCC_SPI2_CLK_ENABLE()
#define DUT_SPI_CLK_DIS()		__HAL_RCC_SPI2_CLK_DISABLE()
#define DUT_SPI_GPIO_CLK_EN()	__HAL_RCC_GPIOB_CLK_ENABLE()

#define DUT_SPI_GPIO_AF_REMAP()

#define DUT_SPI_INT				SPI2_IRQHandler
#define DUT_SPI_IRQ				SPI2_IRQn

#define GPIO_NSS_IRQ			GPIO_NSS_CTS_IRQ

/******************************************************************************/
/* I2C defines ****************************************************************/
/******************************************************************************/
#define DUT_I2C_INST			I2C1

#define DUT_I2C_CLK_EN()		__HAL_RCC_I2C1_CLK_ENABLE()
#define DUT_I2C_CLK_DIS()		__HAL_RCC_I2C1_CLK_DISABLE()
#define DUT_I2C_GPIO_CLK_EN()	__HAL_RCC_GPIOB_CLK_ENABLE()

#define DUT_I2C_GPIO_AF_REMAP()	__HAL_AFIO_REMAP_I2C1_ENABLE()

#define DUT_I2C_EV_INT			I2C1_EV_IRQHandler
#define DUT_I2C_EV_IRQ			I2C1_EV_IRQn
#define DUT_I2C_ERR_INT			I2C1_ER_IRQHandler
#define DUT_I2C_ERR_IRQ			I2C1_ER_IRQn

/******************************************************************************/
/* UART defines ***************************************************************/
/******************************************************************************/
#define DUT_UART_INST			USART1

#define DUT_UART_CLK_EN()		__HAL_RCC_USART1_CLK_ENABLE()
#define DUT_UART_CLK_DIS()		__HAL_RCC_USART1_CLK_DISABLE()
#define DUT_UART_GPIO_CLK_EN()	__HAL_RCC_GPIOA_CLK_ENABLE()

#define DUT_UART_INT			USART1_IRQHandler
#define DUT_UART_IRQ			USART1_IRQn

#define GPIO_CTS_IRQ			GPIO_NSS_CTS_IRQ

#define DUT_UART_RX_DMA_INST	DMA1_Channel5
#define DUT_UART_DMA_RX_INT		DMA1_Channel5_IRQHandler
#define DUT_UART_DMA_RX_IRQ		DMA1_Channel5_IRQn

/******************************************************************************/
#define IF_UART_INST			USART2

#define IF_UART_CLK_EN()		__HAL_RCC_USART2_CLK_ENABLE()
#define IF_UART_CLK_DIS()		__HAL_RCC_USART2_CLK_DISABLE()
#define IF_UART_CLK_GPIO_EN()	__HAL_RCC_GPIOA_CLK_ENABLE()

#define IF_UART_INT				USART2_IRQHandler
#define IF_UART_IRQ				USART2_IRQn

#define IF_UART_DMA_RX_INST		DMA1_Channel6
#define IF_UART_DMA_RX_INT		DMA1_Channel6_IRQHandler
#define IF_UART_DMA_RX_IRQ		DMA1_Channel6_IRQn

#define IF_UART_DMA_TX_INST		DMA1_Channel7
#define IF_UART_DMA_TX_INT		DMA1_Channel7_IRQHandler
#define IF_UART_DMA_TX_IRQ		DMA1_Channel7_IRQn

/******************************************************************************/
/* PWM_DAC defines ************************************************************/
/******************************************************************************/
#define DUT_PWM_DAC_INST			TIM3
#define DUT_PWM_TMR					DUT_PWM_DAC_INST
#define DUT_DAC_TMR					DUT_PWM_DAC_INST

#define DUT_PWM_DAC_CLK_EN()		__HAL_RCC_TIM3_CLK_ENABLE()
#define DUT_PWM_DAC_CLK_DIS()		__HAL_RCC_TIM3_CLK_DISABLE()
#define DUT_PWM_DAC_GPIO_CLK_EN()	__HAL_RCC_GPIOC_CLK_ENABLE()

#define DUT_PWM_DAC_GPIO_AF_REMAP()	__HAL_AFIO_REMAP_TIM3_ENABLE();

/******************************************************************************/
/* IC defines *****************************************************************/
/******************************************************************************/
#define DUT_IC_INST				TIM1
#define DUT_IC_CHANNEL			TIM_CHANNEL_1

#define DUT_IC_CLK_EN()			__HAL_RCC_TIM1_CLK_ENABLE()
#define DUT_IC_GPIO_CLK_EN()	__HAL_RCC_GPIOA_CLK_ENABLE()

#define DUT_IC_CLK_DIS()		__HAL_RCC_TIM1_CLK_DISABLE()

#define DUT_IC_DMA_INST			DMA1_Channel2
#define DUT_IC_DMA_ID			TIM_DMA_ID_CC1

#define DUT_IC_DMA_INT			DMA1_Channel2_IRQHandler
#define DUT_IC_DMA_IRQ			DMA1_Channel2_IRQn
#define DUT_IC_GPIO_INT			EXTI9_5_IRQHandler
#define DUT_IC_GPIO_IRQ			EXTI9_5_IRQn

/******************************************************************************/
/* ADC defines ****************************************************************/
/******************************************************************************/
#define DUT_ADC_INST			ADC2
#define ADC_CHANNEL				ADC_CHANNEL_8

#define DUT_ADC_CLK_EN()		__HAL_RCC_ADC2_CLK_ENABLE()
#define DUT_ADC_GPIO_CLK_EN()	__HAL_RCC_GPIOA_CLK_ENABLE()

#define DUT_ADC_CLK_DIS()		__HAL_RCC_ADC2_CLK_DISABLE()

#define ADC_INT					ADC1_2_IRQHandler
#define ADC_IRQ					ADC1_2_IRQn

#endif /* PORT_NUCLEO_F103RB_H_ */
