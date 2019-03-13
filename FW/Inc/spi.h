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
