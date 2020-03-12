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
 * @file			rtc.h
 * @author			Kevin Weiss
 * @date			19.03.2019
 * @brief			Controls the rtc peripheral.
 ******************************************************************************
 */
#ifndef RTC_H_
#define RTC_H_

/* Function prototypes ********************************************************/
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
 * @return      0 on success
 *
 * @note		Only executes actions if the rtc.mode.init is clear.
 */
error_t commit_rtc();

/**
 * @brief		Updates the rtc time.
 */
void update_rtc();

#endif /* RTC_H_ */
/** @} */
