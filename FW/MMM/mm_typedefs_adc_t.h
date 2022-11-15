/**
 ******************************************************************************
 * @addtogroup typedefs_mmm
 * @{
 * @file      mm_typedefs_adc_t.h
 * @version   2.0.0
 *
 * @details   Generated from the memory map manager version 2.0.0
 ******************************************************************************
 */
#ifndef MM_TYPEDEFS_ADC_T_H
#define MM_TYPEDEFS_ADC_T_H

#ifdef __cplusplus
extern "C"
{
#endif

/* includes ******************************************************************/
#include <stdint.h>

#include "mm_cc.h"
#include "mm_bitfields.h"

/* tyepdefs ******************************************************************/
MM_PACKED_START
/** @brief Controls and reads the ADC */
typedef union {
    struct {
        adc_mode_t mode; /**< Mode settings for the ADC */
        basic_gpio_t dut_adc;
        uint32_t num_of_samples; /**< Number of sample in the sum */
        uint32_t index; /**< Sample index increases when new sample read */
        uint16_t sample; /**< Current 12 bit sample value */
        uint32_t sum; /**< Sum of the last num_of_samples */
        uint32_t current_sum; /**< Current collection of the sums */
        uint8_t counter; /**< Sum counter increases when available */
        uint8_t padding[11]; /**< padding bytes */
    };
    uint8_t data[32]; /**< Array for padding */
} adc_t;
MM_PACKED_END

#ifdef __cplusplus
}
#endif

#endif /* MM_TYPEDEFS_ADC_T_H */
/** @} **/