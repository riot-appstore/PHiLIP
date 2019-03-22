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
 * @date			22.03.2019
 * @brief			Controls the rtc peripheral.
 * @}
 ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include <errno.h>

#include "stm32f1xx_hal.h"

#include "PHiLIP_typedef.h"
#include "port.h"
#include "app_errno.h"

#include "rtc.h"

/* Private variables ---------------------------------------------------------*/
static rtc_t *rtc;

/* Functions -----------------------------------------------------------------*/
/**
 * @brief		Initializes rtc registers.
 *
 * @param[in]	reg			Pointer to live register memory map
 * @note		Populates rtc defaults registers and assigns rtc register
 * 				pointers.
 */
void init_rtc(map_t *reg) {
	RTC_TypeDef *rtc_inst = RTC;

	HAL_PWR_EnableBkUpAccess();
	/* Enable BKP CLK enable for backup registers */
	__HAL_RCC_BKP_CLK_ENABLE();
	/* Peripheral clock enable */
	__HAL_RCC_RTC_ENABLE();

	/* Wait to sync */
	CLEAR_BIT(rtc_inst->CRL, RTC_FLAG_RSF);
	while (!(rtc_inst->CRL & RTC_FLAG_RSF));

	/* Enter init mode */
	while (!(rtc_inst->CRL & RTC_CRL_RTOFF));
	/* Disable the write protection for RTC registers */
	SET_BIT(rtc_inst->CRL, RTC_CRL_CNF);

	/* Clear Flags Bits */
	CLEAR_BIT(rtc_inst->CRL, (RTC_FLAG_OW | RTC_FLAG_ALRAF | RTC_FLAG_SEC));

	/* Set the signal which will be routed to RTC Tamper pin*/
	MODIFY_REG(BKP->RTCCR, (BKP_RTCCR_CCO | BKP_RTCCR_ASOE | BKP_RTCCR_ASOS), RTC_OUTPUTSOURCE_NONE);

	/* RTC Prescaler will be automatically calculated to get 1 second timebase */
	/* Get the RTCCLK frequency */
	uint32_t prescaler = HAL_RCCEx_GetPeriphCLKFreq(RCC_PERIPHCLK_RTC);

	/* RTC period = RTCCLK/(RTC_PR + 1) */
	prescaler = prescaler - 1U;

	/* Configure the RTC_PRLH / RTC_PRLL */
	MODIFY_REG(rtc_inst->PRLH, RTC_PRLH_PRL, (prescaler >> 16U));
	MODIFY_REG(rtc_inst->PRLL, RTC_PRLL_PRL, (prescaler & RTC_PRLL_PRL));

	/* Wait for synchro */
	/* Exit init mode */
	CLEAR_BIT(rtc_inst->CRL, RTC_CRL_CNF);
	while(!(rtc_inst->CRL & RTC_CRL_RTOFF));

	rtc = &(reg->rtc);
	/* init because we don't want to reset the rtc value after reset */
	rtc->mode.init = 1;
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
	RTC_TypeDef *rtc_inst = RTC;
	if (!rtc->mode.init) {

	    uint32_t counter_time = (((uint32_t)rtc->set_day * 86400U) + \
	    						((uint32_t)rtc->set_hour * 3600U) + \
								((uint32_t)rtc->set_minute * 60U) + \
								((uint32_t)rtc->set_second));

		/* Enter init mode */
		while (!(rtc_inst->CRL & RTC_CRL_RTOFF));
		/* Disable the write protection for RTC registers */
		SET_BIT(rtc_inst->CRL, RTC_CRL_CNF);



		/* Set RTC COUNTER MSB word */
		WRITE_REG(rtc_inst->CNTH, (counter_time >> 16U));
		/* Set RTC COUNTER LSB word */
		WRITE_REG(rtc_inst->CNTL, (counter_time & RTC_CNTL_RTC_CNT));

		/* Wait for synchro */
		/* Exit init mode */
		CLEAR_BIT(rtc_inst->CRL, RTC_CRL_CNF);
		while(!(rtc_inst->CRL & RTC_CRL_RTOFF));

		CLEAR_BIT(rtc_inst->CRL, (RTC_FLAG_SEC | RTC_FLAG_OW));

		rtc->mode.init = 1;
		return EOK;
	}
	return ENOACTION;
}

/**
 * @brief		Updates the rtc time.
 */
void update_rtc() {
	if (!rtc->mode.disable) {
		RTC_TypeDef *rtc_inst = RTC;
		uint32_t timecounter;
		uint16_t high;
		uint16_t low;

		do {
			high = rtc_inst->CNTH;
			low = rtc_inst->CNTL;
		} while (high != rtc_inst->CNTH);
		timecounter = (((uint32_t) high << 16U) | low);

		rtc->second = timecounter % 60;
		rtc->minute = (timecounter / 60) % 60;
		rtc->hour = (timecounter / (60 * 60)) % 24;
		rtc->day = (timecounter / (60 * 60 * 24));
	}
}
