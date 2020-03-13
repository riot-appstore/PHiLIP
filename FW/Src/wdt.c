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
 * @file			wdt.c
 * @author			Kevin Weiss
 * @date			21.02.2019
 * @brief			Handles watchdog timer periph.
 * @}
 ******************************************************************************
 */

/* Includes *******************************************************************/
#include "stm32f1xx_hal.h"

#include "wdt.h"

/* Private variables **********************************************************/
static IWDG_HandleTypeDef hiwdg;

/******************************************************************************/
/*           Initialization                                                   */
/******************************************************************************/
/**
 * @brief		Initializes the watchdog timer.
 */
void init_wdt() {
	hiwdg.Instance = IWDG;
	hiwdg.Init.Prescaler = IWDG_PRESCALER_32;
	hiwdg.Init.Reload = 4095;
	HAL_IWDG_Init(&hiwdg);
}

/******************************************************************************/
/*           Functions                                                        */
/******************************************************************************/
void reset_wdt() {
	HAL_IWDG_Refresh(&hiwdg);
}


