/*
 * Copyright 2020 Kevin Weiss for HAW Hamburg
 *
 * This file is subject to the terms and conditions of the MIT License. See the
 * file LICENSE in the top level directory for more details.
 * SPDX-License-Identifier:    MIT
 */

/*
 ******************************************************************************
 * @file			port.h
 * @author			Kevin Weiss
 * @date			20.03.2019
 * @brief			Board/cpu specific porting.
 ******************************************************************************
 */

#ifndef PORT_H_
#define PORT_H_

#ifdef BLUEPILL
#include "port_bluepill.h"
#endif

#ifdef NUCLEOF103RB
#include "port_nucleo_f103rb.h"
#endif

/******************************************************************************/
/* <TEMPLATE defines> *********************************************************/
/******************************************************************************/
/*
 * <MODULE>_INST
 *
 * <MODULE>_CLK_EN
 * <MODULE>_CLK_DIS
 * <MODULE>_GPIO_CLK_EN
 *
 * <MODULE>_GPIO_AF_REMAP
 *
 * <MODULE>_INT
 * <MODULE>_IRQ
 *
 * <MODULE>_DMA_INST
 * <MODULE>_DMA_INT
 * <MODULE>_DMA_IRQ
 */

/******************************************************************************/
/* Pin and ports defines ******************************************************/
/******************************************************************************/
#define LED0 LED0_GPIO_Port, LED0_Pin

#define DEBUG0 DEBUG0_GPIO_Port, DEBUG0_Pin
#define DEBUG1 DEBUG1_GPIO_Port, DEBUG1_Pin
#define DEBUG2 DEBUG2_GPIO_Port, DEBUG2_Pin

#define TEST_PASS TEST_PASS_GPIO_Port, TEST_PASS_Pin
#define TEST_WARN TEST_WARN_GPIO_Port, TEST_WARN_Pin
#define TEST_FAIL TEST_FAIL_GPIO_Port, TEST_FAIL_Pin

#define USER_BTN USER_BTN_GPIO_Port, USER_BTN_Pin
#define DUT_RST DUT_RST_GPIO_Port, DUT_RST_Pin

#define DUT_ADC DUT_ADC_GPIO_Port, DUT_ADC_Pin

#define PM_LO_ADC PM_LO_ADC_GPIO_Port, PM_LO_ADC_Pin
#define PM_HI_ADC PM_HI_ADC_GPIO_Port, PM_HI_ADC_Pin
#define PM_V_ADC PM_V_ADC_GPIO_Port, PM_V_ADC_Pin

#define DUT_TX DUT_TX_GPIO_Port, DUT_TX_Pin
#define DUT_RX DUT_RX_GPIO_Port, DUT_RX_Pin
#define DUT_CTS DUT_CTS_GPIO_Port, DUT_CTS_Pin
#define DUT_RTS DUT_RTS_GPIO_Port, DUT_RTS_Pin

#define DUT_IC DUT_IC_GPIO_Port, DUT_IC_Pin

#define IF_TX IF_TX_GPIO_Port, IF_TX_Pin
#define IF_RX IF_RX_GPIO_Port, IF_RX_Pin

#define DUT_NSS DUT_NSS_GPIO_Port, DUT_NSS_Pin
#define DUT_SCK DUT_SCK_GPIO_Port, DUT_SCK_Pin
#define DUT_MISO DUT_MISO_GPIO_Port, DUT_MISO_Pin
#define DUT_MOSI DUT_MOSI_GPIO_Port, DUT_MOSI_Pin

#define DUT_SCL DUT_SCL_GPIO_Port, DUT_SCL_Pin
#define DUT_SDA DUT_SDA_GPIO_Port, DUT_SDA_Pin

#define DUT_PWM DUT_PWM_GPIO_Port, DUT_PWM_Pin
#define DUT_DAC DUT_DAC_GPIO_Port, DUT_DAC_Pin

/******************************************************************************/
/* CPU Macros *****************************************************************/
/******************************************************************************/
#define DUT_IC_REMAINING_BUF(x)	(x.Instance->CNDTR)
#define DUT_IC_OV_OCCURED(x)	(x->Instance->SR & TIM_SR_UIF)
#define DUT_IC_OV_CLEAR(x)		x->Instance->SR &= ~(TIM_SR_UIF)

#define ADC_DATA_READY(x)	(x.Instance->SR & ADC_FLAG_EOC)

#define DEFAULT_INT_PRIO	1

/******************************************************************************/
void init_periphs(void);
void init_clock(void);
void _Error_Handler(char *file, int line);

#endif /* PORT_H_ */
