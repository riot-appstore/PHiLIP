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
 * @file			wdt.h
 * @author			Kevin Weiss
 * @date			21.02.2019
 * @brief			Handles watchdog timer periph.
 ******************************************************************************
 */
#ifndef WDT_H_
#define WDT_H_

/* Function prototypes ********************************************************/
/**
 * @brief		Initializes the watchdog timer.
 */
void init_wdt();

/**
 * @brief		Resets the watchdog timer.
 */
void reset_wdt();

#endif /* WDT_H_ */
/** @} */
