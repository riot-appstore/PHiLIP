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

/* Includes ------------------------------------------------------------------*/
#include <errno.h>
#include <stdint.h>
#include <string.h>

#include "stm32f1xx_hal.h"
#include "build_defs.h"

#include "PHiLIP_typedef.h"
#include "PHiLIP_map.h"
#include "app_errno.h"
#include "app_common.h"
#include "app_defaults.h"
#include "port.h"

#include "sys.h"

/* Private variables ---------------------------------------------------------*/
static sys_t *sys_reg;
static sys_t *saved_sys_reg;

/* Functions -----------------------------------------------------------------*/
/**
 * @brief		Initializes system registers.
 *
 * @param[in]	reg			Pointer to live register memory map
 * @param[in]	saved_reg	Pointer to saved register memory map
 * @note		Populates system defaults registers and assigns system register
 * 				pointers.
 */
void init_sys(map_t *reg, map_t *saved_reg) {
	sys_reg = &(reg->sys);
	saved_sys_reg = &(saved_reg->sys);

	uint32_t tmp[3];
	tmp[0] = HAL_GetUIDw0();
	tmp[1] = HAL_GetUIDw1();
	tmp[2] = HAL_GetUIDw2();
	memcpy(sys_reg->sn, tmp, sizeof(tmp));

	sys_reg->fw_rev[1] = FW_REV_PATCH;
	sys_reg->fw_rev[2] = FW_REV_MINOR;
	sys_reg->fw_rev[3] = FW_REV_MAJOR;
    sys_reg->fw_rev[0] = 0;

	sys_reg->if_rev[1] = IF_VERSION_PATCH;
	sys_reg->if_rev[2] = IF_VERSION_MINOR;
	sys_reg->if_rev[3] = IF_VERSION_MAJOR;
    sys_reg->if_rev[0] = 0;

	sys_reg->build_time.second = BUILD_SEC;
	sys_reg->build_time.minute = BUILD_MIN;
	sys_reg->build_time.hour = BUILD_HOUR;
	sys_reg->build_time.day_of_month = BUILD_DAY;
	sys_reg->build_time.month = BUILD_MONTH;
	sys_reg->build_time.year = BUILD_YEAR;

#if BLUEPILL
	sys_reg->status.board = 1;
#endif

	sys_reg->sys_clk = SystemCoreClock;

	commit_sys();
}

/**
 * @brief		Commits the system registers and executes operations.
 *
 * @pre			sys must first be initialized with init_sys()
 * @return      EOK if init occurred
 * @return      ENOACTION if no init was triggered
 *
 * @note		Only executes actions if the sys.mode.init is set.  Update will
 * 				always be reset.
 */
error_t commit_sys() {
	sys_t *sys_inst = sys_reg;
	sys_t *saved_sys_inst = saved_sys_reg;
	error_t ret = ENOACTION;

	if (!sys_inst->mode.init) {
		if (sys_inst->mode.dut_rst) {
			HAL_GPIO_WritePin(DUT_RST_GPIO_Port, DUT_RST_Pin, GPIO_PIN_RESET);
		} else {
			HAL_GPIO_WritePin(DUT_RST_GPIO_Port, DUT_RST_Pin, GPIO_PIN_SET);
		}
		sys_inst->mode.init = 1;
		ret = EOK;
	}
	sys_inst->status.update = 0;

	copy_until_same(saved_sys_inst, sys_inst, sizeof(*sys_inst));
	return ret;
}

/**
 * @brief		Updates the tick count to the app reg.
 *
 * @note		May have concurrent access issues.
 */
void update_tick() {
	 sys_reg->tick = get_tick();
}
