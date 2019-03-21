/*
 * MIT License
 *
 * Copyright 2018 Kevin Weiss for HAW Hamburg
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

/**
 ******************************************************************************
 * @file			main.c
 * @author			Kevin Weiss
 * @author			Yegor Yefremov
 * @date			13.02.2019
 * @brief			Controls the uart peripherals.
 ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include <errno.h>
#include <stdint.h>

#include "stm32f1xx_hal.h"

#include "PHiLIP_typedef.h"
#include "PHiLIP_defaults.h"
#include "app_access.h"
#include "app_common.h"
#include "app_defaults.h"

#include "app_reg.h"
#include "app_shell_if.h"
#include "trace.h"
#include "gpio.h"
#include "i2c.h"
#include "uart.h"
#include "spi.h"
#include "pwm_dac.h"
#include "rtc.h"
#include "adc.h"
#include "sys.h"
#include "port.h"

#include "test.h"
/* Private variables ---------------------------------------------------------*/
/** @brief  Watchdog timer handle. */
extern IWDG_HandleTypeDef hiwdg;

/** @brief  The application entry point. */
int main(void) {
	int32_t led_tick = 0;
	map_t reg = { 0 };
	map_t saved_reg = { 0 };

	/* Reset of all peripherals, Initializes the Flash interface and Systick. */
	HAL_Init();

	/* Configure the system clock */
	init_clock();

	init_defaults_map_t(&reg);
	/* Initialize all configured peripherals */
	DIS_INT;
	init_periphs();
	init_gpio(&reg, &saved_reg);
	init_app_reg(&reg, &saved_reg);
	init_trace(&reg);
	init_dut_pwm_dac(&reg, &saved_reg);
	init_dut_uart(&reg, &saved_reg);
	init_if_uart();
	init_dut_i2c(&reg, &saved_reg);
	init_dut_spi(&reg, &saved_reg);
	init_rtc(&reg);
	init_dut_adc(&reg, &saved_reg);
	init_sys(&reg, &saved_reg);
	EN_INT;

	while (1) {
		if (led_tick < HAL_GetTick()) {
			led_tick = HAL_GetTick() + 20;
			HAL_GPIO_TogglePin(LED0);
			HAL_IWDG_Refresh(&hiwdg);
			update_tick();
			update_debug_inputs();
			update_dut_spi_inputs();
			update_rtc();
		}
		poll_dut_uart();
		poll_if_uart();
	}
}
