/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
 ******************************************************************************
 * This notice applies to any and all portions of this file
 * that are not between comment pairs USER CODE BEGIN and
 * USER CODE END. Other portions of this file, whether
 * inserted by the user or by software development tools
 * are owned by their respective copyright owners.
 *
 * Copyright (c) 2018 STMicroelectronics International N.V.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted, provided that the following conditions are met:
 *
 * 1. Redistribution of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. Neither the name of STMicroelectronics nor the names of other
 *    contributors to this software may be used to endorse or promote products
 *    derived from this software without specific written permission.
 * 4. This software, including modifications and/or derivative works of this
 *    software, must execute solely and exclusively on microcontroller or
 *    microprocessor devices manufactured by or for STMicroelectronics.
 * 5. Redistribution and use of this software other than as permitted under
 *    this license is void and will automatically terminate your rights under
 *    this license.
 *
 * THIS SOFTWARE IS PROVIDED BY STMICROELECTRONICS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS, IMPLIED OR STATUTORY WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
 * PARTICULAR PURPOSE AND NON-INFRINGEMENT OF THIRD PARTY INTELLECTUAL PROPERTY
 * RIGHTS ARE DISCLAIMED TO THE FULLEST EXTENT PERMITTED BY LAW. IN NO EVENT
 * SHALL STMICROELECTRONICS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
 * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 ******************************************************************************
 */
/* Includes ------------------------------------------------------------------*/
#include "stm32f1xx_hal.h"

/* USER CODE BEGIN Includes */
#ifdef BLUEPILL
#include "usb_device.h"
#include "usbd_cdc.h"
#include "port_usb.h"
#endif

#include "port_uart.h"
#include "app_access.h"
#include "app_typedef.h"
#include "app_common.h"
#include "app_defaults.h"

#include "app_shell_if.h"
#include "app_i2c.h"
#include "port_dut_uart.h"
#include "app.h"
#include "port.h"

#include "test.h"
/* USER CODE END Includes */

/* Private variables ---------------------------------------------------------*/
extern UART_HandleTypeDef huart_if;
extern UART_HandleTypeDef huart_dut;
extern I2C_HandleTypeDef hi2c_dut;
extern IWDG_HandleTypeDef hiwdg;

/* USER CODE BEGIN PV */
/* Private variables ---------------------------------------------------------*/

#define UART_BUF_SIZE	512
/* USER CODE END PV */


/**
 * @brief  The application entry point.
 *
 * @retval None
 */
int main(void) {
	/* USER CODE BEGIN 1 */
	int32_t led_tick = 0;

#ifdef BLUEPILL
	char if_usb_buf[UART_BUF_SIZE] = { 0 };
	PORT_USB_t usb_if = {.str = if_usb_buf,
			.size = sizeof(if_usb_buf),
			.access = IF_ACCESS, .index = 0,
			.mode = MODE_REG};
#endif
	char if_uart_buf[UART_BUF_SIZE] = { 0 };
	PORT_UART_t uart_if = {.huart = &huart_if,
			.str = if_uart_buf,
			.size = sizeof(if_uart_buf),
			.access = IF_ACCESS,
			.mode = MODE_REG};
	char dut_uart_buf[UART_BUF_SIZE] = { 0 };
	PORT_UART_t uart_dut = {.huart = &huart_dut,
			.str = dut_uart_buf,
			.size = sizeof(dut_uart_buf),
			.access = PERIPH_ACCESS,
			.mode = MODE_ECHO};
	/* USER CODE END 1 */

	/* MCU Configuration----------------------------------------------------------*/

	/* Reset of all peripherals, Initializes the Flash interface and the Systick. */
	HAL_Init();

	/* Configure the system clock */
	init_clock();
	run_test();
	/* Initialize all configured peripherals */
	init_periphs();

	/* USER CODE BEGIN 2 */
	port_uart_init(&uart_if);
	uart_dut_init(&uart_dut);
	app_i2c_init(&hi2c_dut);
	execute_reg_change();
	/* USER CODE END 2 */

	/* Infinite loop */
	/* USER CODE BEGIN WHILE */
	while (1) {

		if (led_tick < HAL_GetTick()) {
			if ((FW_REV & 0x03) == 0) {
				led_tick = HAL_GetTick() + 40;
			}
			else if ((FW_REV & 0x03) == 1){
				led_tick = HAL_GetTick() + 200;
			}
			else if ((FW_REV & 0x03) == 2){
				led_tick = HAL_GetTick() + 500;
			}
			else if ((FW_REV & 0x03) == 3){
				led_tick = HAL_GetTick() + 1000;
			}
			HAL_GPIO_TogglePin(LED0);
		}

		if (HAL_IWDG_Refresh(&hiwdg) != HAL_OK) {
		}
#ifdef BLUEPILL
		port_usb_poll(&usb_if);
#endif
		port_uart_poll(&uart_if);
		port_uart_poll(&uart_dut);
		/* USER CODE END WHILE */

		/* USER CODE BEGIN 3 */

	}
	/* USER CODE END 3 */

}

/**
 * @}
 */

/**
 * @}
 */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
