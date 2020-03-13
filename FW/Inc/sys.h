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
 * @file			sys.h
 * @author			Kevin Weiss
 * @date			13.02.2019
 * @brief			System control and management.
 * @details			Deals with build times, serial numbers, ticks, etc.
 ******************************************************************************
 */

#ifndef SYS_H_
#define SYS_H_

/* Function prototypes ********************************************************/
/**
 * @brief		Initializes system registers.
 *
 * @param[in]	reg			Pointer to register memory map
 * @note		Populates system defaults registers and assigns system register
 * 				pointers.
 */
void init_sys(map_t *reg);

/**
 * @brief		Commits the system registers and executes operations.
 *
 * @pre			sys must first be initialized with init_sys()
 * @return      0 if OK
 *
 * @note		Only executes actions if the sys.mode.init is clear.
 *              Update will always be reset.
 */
error_t commit_sys();

/**
 * @brief		Updates the tick count to the app reg.
 *
 * @note		May have concurrent access issues.
 */
void update_tick();

#endif /* SYS_H_ */
/** @} */
