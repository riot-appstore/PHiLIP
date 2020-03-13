/*
 * Copyright 2020 Kevin Weiss for HAW Hamburg
 *
 * This file is subject to the terms and conditions of the MIT License. See the
 * file LICENSE in the top level directory for more details.
 * SPDX-License-Identifier:    MIT
 */

/**
 ******************************************************************************
 * @addtogroup Application
 * @{
 * @file			app_reg.c
 * @author			Kevin Weiss
 * @date			13.02.2019
 * @brief			Accessing application registers.
 * @details			Deals with setting up registers, reading, writing and
 * 					access.
 * @}
 ******************************************************************************
 */

/* Includes *******************************************************************/
#include <errno.h>
#include <stdint.h>

#include "stm32f1xx_hal.h"

#include "PHiLIP_typedef.h"
#include "app_common.h"
#include "app_access.h"
#include "app_reg.h"

#include "gpio.h"
#include "uart.h"
#include "i2c.h"
#include "spi.h"
#include "pwm_dac.h"
#include "rtc.h"
#include "adc.h"
#include "tmr.h"
#include "sys.h"

/* Private variables **********************************************************/
static map_t *app_reg;

/******************************************************************************/
/*           Initialization                                                   */
/******************************************************************************/
void init_app_reg(map_t *reg) {
	app_reg = reg;

	for (uint16_t i = 0; i < sizeof(app_reg->user_reg); i++) {
		app_reg->user_reg[i] = i;
	}
}

/******************************************************************************/
/*           Functions                                                        */
/******************************************************************************/
error_t execute_reg_change() {
	commit_debug();
	commit_dut_i2c();
	commit_dut_spi();
	commit_dut_pwm();
	commit_dut_dac();
	commit_dut_uart();
	commit_rtc();
	commit_dut_adc();
	commit_sys();
	commit_dut_ic();

	return 0;
}

/******************************************************************************/
uint32_t get_reg_size() {
	return sizeof(*app_reg);
}

void unprotected_read_uint8(uint32_t index, uint8_t *data) {
	*data = app_reg->data8[index];
}

error_t read_reg(uint32_t index, uint8_t *data) {
	return read_regs(index, data, 1);
}

error_t read_regs(uint32_t index, uint8_t *data, uint16_t size) {
	if (index + size > get_reg_size()) {
		return EOVERFLOW;
	}
	copy_until_same(data, &app_reg->data8[index], size);

	return 0;
}

/******************************************************************************/
error_t write_reg(uint32_t index, uint8_t data, uint8_t access) {

	return write_regs(index, &data, 1, access);
}

error_t write_regs(uint32_t index, uint8_t *data, uint16_t size, uint8_t access) {
	if (index + size > get_reg_size()) {
		return EOVERFLOW;
	}
	for (int i = 0; i < size; i++) {
		if (!(MAP_T_ACCESS[index + i] & access)) {
			return EACCES;
		}
	}
	for (int i = 0; i < size; i++) {
		DIS_INT;
		app_reg->data8[index + i] = data[i];
		EN_INT;
	}
	return 0;
}

/******************************************************************************/
void add_index(uint16_t *i) {
	(*i)++;
	if (*i == get_reg_size()) {
		*i = 0;
	}
}
void sub_index(uint16_t *i) {
	if (*i == 0) {
		*i = get_reg_size();
	}
	(*i)--;
}
