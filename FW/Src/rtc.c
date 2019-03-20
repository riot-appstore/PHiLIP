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
 * @file			rtc.c
 * @author			Kevin Weiss
 * @date			19.03.2019
 * @brief			Controls the rtc peripheral.
 * @}
 ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include <string.h>
#include <errno.h>

#include "stm32f1xx_hal.h"

#include "PHiLIP_typedef.h"
#include "port.h"
#include "app_errno.h"
#include "app_access.h"
#include "app_common.h"
#include "app_defaults.h"
#include "app_reg.h"

#include "rtc.h"

/* Private enums/structs -----------------------------------------------------*/

/** @brief	The parameters for rtc control */
typedef struct {
	RTC_HandleTypeDef hrtc; /**< Handle for the spi device */
	rtc_t *reg; /**< rtc live application registers */
} rtc_dev;
/** @} */

/* Private variables ---------------------------------------------------------*/
static rtc_dev rtc;

/* Functions -----------------------------------------------------------------*/
/**
 * @brief		Initializes rtc registers.
 *
 * @param[in]	reg			Pointer to live register memory map
 * @note		Populates rtc defaults registers and assigns rtc register
 * 				pointers.
 */
void init_rtc(map_t *reg) {
	rtc.hrtc.Instance = RTC;
	rtc.hrtc.Init.AsynchPrediv = RTC_AUTO_1_SECOND;
	rtc.hrtc.Init.OutPut = RTC_OUTPUTSOURCE_NONE;
	HAL_RTC_Init(&rtc.hrtc);
	rtc.reg = &(reg->rtc);
	/* init because we don't want to reset the rtc value after reset */
	rtc.reg->mode.init = 1;
	commit_rtc();
}

/**
 * @brief		Commits the rtc registers and executes operations.
 *
 * @pre			rtc must first be initialized with init_rtc()
 * @return      EOK if init occurred
 * @return      ENOACTION if no init was triggered
 *
 * @note		Only executes actions if the rtc.mode.init is set.
 */
error_t commit_rtc() {
	if (!rtc.reg->mode.init) {
		RTC_TimeTypeDef time;
		RTC_DateTypeDef date;

		time.Seconds = rtc.reg->set_time.second;
		time.Minutes = rtc.reg->set_time.minute;
		time.Hours = rtc.reg->set_time.hour;
		date.WeekDay = rtc.reg->set_time.day_of_week;
		date.Month = rtc.reg->set_time.month;
		date.Date = rtc.reg->set_time.day_of_month;
		date.Year = rtc.reg->set_time.year;

		HAL_RTC_SetTime(&rtc.hrtc, &time, RTC_FORMAT_BIN);

		HAL_RTC_SetDate(&rtc.hrtc, &date, RTC_FORMAT_BIN);
		rtc.reg->mode.init = 1;
		return EOK;
	}
	return ENOACTION;
}

/**
 * @brief		Updates the rtc time.
 */
void update_rtc() {
	if (!rtc.reg->mode.disable) {
		RTC_TimeTypeDef time;
		RTC_DateTypeDef date;
		HAL_RTC_GetDate(&rtc.hrtc, &date, RTC_FORMAT_BIN);
		HAL_RTC_GetTime(&rtc.hrtc, &time, RTC_FORMAT_BIN);
		rtc.reg->time.second = time.Seconds;
		rtc.reg->time.minute = time.Minutes;
		rtc.reg->time.hour = time.Hours;
		rtc.reg->time.day_of_month = date.Date;
		rtc.reg->time.day_of_week = date.WeekDay;
		rtc.reg->time.month = date.Month;
		rtc.reg->time.year = date.Year;
	}
}
