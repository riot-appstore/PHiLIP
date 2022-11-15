/**
 ******************************************************************************
 * @addtogroup philip_map_mmm
 * @{
 * @file      mm_default_philip_map.h
 * @version   2.0.0
 *
 * @details   Generated from the memory map manager version 2.0.0
 ******************************************************************************
 */
#ifndef MM_DEFAULT_PHILIP_MAP_H
#define MM_DEFAULT_PHILIP_MAP_H

#ifdef __cplusplus
extern "C"
{
#endif

/* includes ******************************************************************/#include "mm_typedefs_philip_map.h"

/* defines *******************************************************************/
#define MM_DEFAULT_PHILIP_MAP_SYS_DEVICE_NUM 17061
#define MM_DEFAULT_PHILIP_MAP_I2C_SLAVE_ADDR_1 85
#define MM_DEFAULT_PHILIP_MAP_I2C_SLAVE_ADDR_2 66
#define MM_DEFAULT_PHILIP_MAP_ADC_NUM_OF_SAMPLES 1024
#define MM_DEFAULT_PHILIP_MAP_PWM_H_TICKS 256
#define MM_DEFAULT_PHILIP_MAP_PWM_L_TICKS 256
#define MM_DEFAULT_PHILIP_MAP_DAC_LEVEL 5000
#define MM_DEFAULT_PHILIP_MAP_TMR_MIN_TICK 4294967295

/* functions *****************************************************************/
const void mm_defaults_init_philip_map(philip_map *init);

#ifdef __cplusplus
}
#endif

#endif /* MM_DEFAULT_PHILIP_MAP_H */
/** @} **/