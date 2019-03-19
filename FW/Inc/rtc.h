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
 * @file			rtc.h
 * @author			Kevin Weiss
 * @date			19.03.2019
 * @brief			Controls the rtc peripheral.
 ******************************************************************************
 */
#ifndef RTC_H_
#define RTC_H_

/* Function prototypes -------------------------------------------------------*/
/**
 * @brief		Initializes rtc registers.
 *
 * @param[in]	reg			Pointer to live register memory map
 * @note		Populates rtc defaults registers and assigns rtc register
 * 				pointers.
 */
void init_rtc(map_t *reg);

/**
 * @brief		Commits the rtc registers and executes operations.
 *
 * @pre			rtc must first be initialized with init_rtc()
 * @return      EOK if init occurred
 * @return      ENOACTION if no init was triggered
 *
 * @note		Only executes actions if the rtc.mode.init is set.
 */
error_t commit_rtc();

/**
 * @brief		Updates the rtc time.
 */
void update_rtc();

#endif /* RTC_H_ */
/** @} */
