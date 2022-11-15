/*
 * Copyright 2020 Kevin Weiss for HAW Hamburg
 *
 * This file is subject to the terms and conditions of the MIT License. See the
 * file LICENSE in the top level directory for more details.
 * SPDX-License-Identifier:    MIT
 */

/**
 ******************************************************************************
 * @addtogroup periph
 * @{
 * @file			sys.c
 * @author			Kevin Weiss
 * @date			13.02.2019
 * @brief			System control and management.
 * @details			Deals with build times, serial numbers, ticks, etc.
 * @}
 ******************************************************************************
 */

/* Includes *******************************************************************/
#include <errno.h>
#include <stdint.h>
#include <string.h>

#include "stm32f1xx_hal.h"
#include "build_defs.h"

#include "mm_typedefs.h"
#include "mm_meta.h"
#include "app_common.h"
#include "app_defaults.h"
#include "port.h"

#include "sys.h"

/* Private function prototypes ************************************************/
static void _init_gpio();

/* Private variables **********************************************************/
static sys_t *sys_reg;

/******************************************************************************/
/*           Initialization                                                   */
/******************************************************************************/
void init_sys(map_t *reg) {
	sys_reg = &(reg->sys);

	_init_gpio();

	uint32_t tmp[3];
	tmp[0] = HAL_GetUIDw0();
	tmp[1] = HAL_GetUIDw1();
	tmp[2] = HAL_GetUIDw2();
	memcpy(sys_reg->sn, tmp, sizeof(tmp));

	sys_reg->fw_rev[1] = FW_REV_PATCH;
	sys_reg->fw_rev[2] = FW_REV_MINOR;
	sys_reg->fw_rev[3] = FW_REV_MAJOR;
    sys_reg->fw_rev[0] = 0;

	sys_reg->if_rev[1] = MM_PATCH_VERSION;
	sys_reg->if_rev[2] = MM_MINOR_VERSION;
	sys_reg->if_rev[3] = MM_MAJOR_VERSION;
    sys_reg->if_rev[0] = 0;

	sys_reg->build_time.second = BUILD_SEC;
	sys_reg->build_time.minute = BUILD_MIN;
	sys_reg->build_time.hour = BUILD_HOUR;
	sys_reg->build_time.day_of_month = BUILD_DAY;
	sys_reg->build_time.month = BUILD_MONTH;
	sys_reg->build_time.year = BUILD_YEAR;

	sys_reg->status.board = BOARD_ID;

	sys_reg->sys_clk = SystemCoreClock;

	commit_sys();
}

error_t commit_sys() {
	sys_t *sys_inst = sys_reg;

	sys_inst->status.update = 0;
	if (sys_inst->mode.init) {
		return 0;
	}
	if (sys_inst->mode.dut_rst) {
		HAL_GPIO_WritePin(DUT_RST_GPIO_Port, DUT_RST_Pin, GPIO_PIN_RESET);
	} else {
		HAL_GPIO_WritePin(DUT_RST_GPIO_Port, DUT_RST_Pin, GPIO_PIN_SET);
	}
	sys_inst->mode.init = 1;

	return 0;
}

/******************************************************************************/
static void _init_gpio() {
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	HAL_GPIO_WritePin(DUT_RST, GPIO_PIN_SET);

	GPIO_InitStruct.Pin = DUT_RST_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_MEDIUM;
	HAL_GPIO_Init(DUT_RST_GPIO_Port, &GPIO_InitStruct);
}

/******************************************************************************/
/*           Functions                                                        */
/******************************************************************************/
void update_tick() {
	 sys_reg->tick = get_tick();
}
