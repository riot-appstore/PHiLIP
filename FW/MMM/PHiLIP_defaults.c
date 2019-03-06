/**
 ******************************************************************************
 * @addtogroup PHiLIP_MMM
 * @{
 * @file      PHiLIP_defaults.c
 * @author    Kevin Weiss
 * @version   0.0.2
 * @date      2019-03-06
 * @}
 * @details   Generated from the memory map manager
 ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include <stdint.h>

#include "PHiLIP_typedef.h"

#include "PHiLIP_defaults.h"

/* Functions -----------------------------------------------------------------*/
/** @brief Assign defaults for map_t */
void init_defaults_map_t(map_t *init) {
	init->sys.device_num = DEFAULT_SYS_DEVICE_NUM;
	init->i2c.slave_addr_1 = DEFAULT_I2C_SLAVE_ADDR_1;
	init->i2c.slave_addr_2 = DEFAULT_I2C_SLAVE_ADDR_2;
}

