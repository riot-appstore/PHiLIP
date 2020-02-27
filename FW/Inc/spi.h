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
 * @file			spi.h
 * @author			Kevin Weiss
 * @date			13.02.2019
 * @brief			Controls the spi peripheral.
 ******************************************************************************
 */

#ifndef SPI_H_
#define SPI_H_

/* Defines -------------------------------------------------------------------*/
/** @brief	SPI data not available byte */
#define SPI_NO_DATA_BYTE	0xFE

/* Function prototypes -------------------------------------------------------*/
/**
 * @brief		Initializes spi registers.
 *
 * @param[in]	reg			Pointer to live register memory map
 * @param[in]	saved_reg	Pointer to saved register memory map
 * @note		Populates spi defaults registers and assigns spi register
 * 				pointers.
 */
void init_dut_spi(map_t *reg, map_t *saved_reg);

/**
 * @brief		Commits the spi registers and executes operations.
 *
 * @pre			spi must first be initialized with init_dut_spi()
 * @return      EOK if init occurred
 * @return      ENOACTION if no init was triggered
 *
 * @note		Only executes actions if the spi.mode.init is set.
 */
error_t commit_dut_spi();

/**
 * @brief		Updates the spi input levels.
 */
void update_dut_spi_inputs();

/* Interrupts ----------------------------------------------------------------*/
/**
 * @brief Interrupt for the spi line raise or lower
 */
void dut_nss_int();

#endif /* SPI_H_ */
/** @} */
