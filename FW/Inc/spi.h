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

/* Defines ********************************************************************/
/** @brief	SPI data not available byte */
#define SPI_NO_DATA_BYTE	0xFE

/* Function prototypes ********************************************************/
/**
 * @brief		Initializes spi registers.
 *
 * @param[in]	reg			Pointer to live register memory map
 * @note		Populates spi defaults registers and assigns spi register
 * 				pointers.
 */
void init_dut_spi(map_t *reg);

/**
 * @brief		Commits the spi registers and executes operations.
 *
 * @pre			spi must first be initialized with init_dut_spi()
 * @return      0 if OK
 *
 * @note		Only executes actions if the spi.mode.init is clear.
 */
error_t commit_dut_spi();

/**
 * @brief		Updates the spi input levels.
 */
void update_dut_spi_inputs();

/**
 * @brief		Only for use with the STM32xxxx_HAL_Driver.
 */
void init_dut_spi_msp();

/**
 * @brief		Only for use with the STM32xxxx_HAL_Driver.
 */
void deinit_dut_spi_msp();

/**
 * @brief		Only for use with the STM32xxxx_HAL_Driver.
 */
void init_dut_spi_ic_msp();

/**
 * @brief		Only for use with the STM32xxxx_HAL_Driver.
 */
void deinit_dut_spi_ic_msp();

/**
 * @brief       Poll for transfers captured by the IC to put them into the buffer
 */
void poll_dut_spi_ic();

/* Interrupts ----------------------------------------------------------------*/
/**
 * @brief Interrupt callback for the spi line raise or lower
 */
void GPIO_NSS_INT();

#endif /* SPI_H_ */
/** @} */
