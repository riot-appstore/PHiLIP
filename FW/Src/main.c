/*
 * Copyright 2020 Kevin Weiss for HAW Hamburg
 *
 * This file is subject to the terms and conditions of the MIT License. See the
 * file LICENSE in the top level directory for more details.
 * SPDX-License-Identifier:    MIT
 */

/**
 ******************************************************************************
 * @file			main.c
 * @author			Kevin Weiss
 * @author			Yegor Yefremov
 * @date			21.03.2019
 * @brief			Main function that initializes and controls super loop and
 * 					polling based functions.
 ******************************************************************************
 */

/** @mainpage
 *
 * @section Description
 * PHiLIP is qualified open-source firmware for nucleo-f103rb or bluepill
 * boards used for testing peripherals of other embedded devices. PHiLIP is
 * a low-cost solution to allow detailed, corner case peripheral testing for
 * both developers and CI systems. PHiLIP is aimed at getting salient
 * information that would be gathered from an oscilloscope or logic analyzer
 * as well as injecting specific peripheral behaviors. PHiLIP is designed for
 * testing peripheral APIs for embedded operating systems and hardware
 * abstraction layers but was built with an architecture that allows for easy
 * extensions to other applications such as product qualification or
 * simulation. PHiLIP can be used with a raw serial connection but also comes
 * with a python interface that simplifies writing test scripts as well as a
 * shell for developers to run manual tests.
 *
 * @section Architecture
 * PHiLIP firmware is designed to easily add peripheral functionality. It
 * separates out the peripherals from the communication and application logic
 * and the memory map.
 *
 *  \image html PHiLIP_firmware_arch.png
 */

/* Includes *******************************************************************/
#include <errno.h>
#include <stdint.h>

#include "stm32f1xx_hal.h"

#include "PHiLIP_typedef.h"
#include "PHiLIP_defaults.h"
#include "app_common.h"

#include "app_reg.h"
#include "trace.h"
#include "gpio.h"
#include "i2c.h"
#include "uart.h"
#include "spi.h"
#include "pwm_dac.h"
#include "rtc.h"
#include "adc.h"
#include "sys.h"
#include "led_flash.h"
#include "wdt.h"
#include "tmr.h"
#include "port.h"

/* Private function prototypes ************************************************/
static uint32_t _is_tick();
static void _super_loop();

/******************************************************************************/
/*           Functions                                                        */
/******************************************************************************/
/** @brief  The application entry point. */
int main(void) {
	map_t reg = { 0 };

	/* Reset of all peripherals, Initializes the Flash interface and Systick. */
	HAL_Init();

	/* Configure the system clock */
	init_clock();

	init_defaults_map_t(&reg);
	/* Initialize all configured peripherals */
	DIS_INT;
	init_periphs();
	init_gpio(&reg);
	init_app_reg(&reg);
	init_trace(&reg);
	init_dut_pwm_dac(&reg);
	init_dut_uart(&reg);
	init_if_uart();
	init_dut_i2c(&reg);
	init_dut_spi(&reg);
	init_rtc(&reg);
	init_dut_adc(&reg);
	init_dut_ic(&reg);
	init_sys(&reg);
	init_led_flash();
	init_wdt();
	EN_INT;
	while (1) {
		_super_loop();
		poll_dut_uart();
		poll_if_uart();
	}
}

/******************************************************************************/
static void _super_loop() {
	void (* const fxn_to_ex_per_tick[])(void) = {update_tick,
		update_debug_inputs,
		update_dut_spi_inputs,
		update_dut_i2c_inputs,
		update_dut_uart_inputs,
		update_dut_pwm_inputs,
		update_dut_dac_inputs,
		update_dut_ic_inputs,
		update_rtc };
	void (* const fxn_to_ex[])(void) = {poll_dut_adc,
										poll_dut_ic,
										poll_dut_spi_ic};
	static uint32_t fxn_index_tick = 0;
	static uint32_t fxn_index = 0;
	if (_is_tick()) {
		flash_fw_version();
		reset_wdt();
		(fxn_to_ex_per_tick[fxn_index_tick++])();
		if (fxn_index_tick
				>= sizeof(fxn_to_ex_per_tick) / sizeof(fxn_to_ex_per_tick[0])) {
			fxn_index_tick = 0;
		}
	} else {
		(fxn_to_ex[fxn_index++])();
		if (fxn_index >= sizeof(fxn_to_ex) / sizeof(fxn_to_ex[0])) {
			fxn_index = 0;
		}
	}
}

/******************************************************************************/
static uint32_t _is_tick() {
	static uint32_t tick = 0;
	if (tick != HAL_GetTick()) {

		tick = HAL_GetTick();
		return 1;
	}
	return 0;
}
