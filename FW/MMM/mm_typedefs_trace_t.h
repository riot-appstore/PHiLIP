/**
 ******************************************************************************
 * @addtogroup typedefs_mmm
 * @{
 * @file      mm_typedefs_trace_t.h
 * @version   2.0.0
 *
 * @details   Generated from the memory map manager version 2.0.0
 ******************************************************************************
 */
#ifndef MM_TYPEDEFS_TRACE_T_H
#define MM_TYPEDEFS_TRACE_T_H

#ifdef __cplusplus
extern "C"
{
#endif

/* includes ******************************************************************/
#include <stdint.h>

#include "mm_cc.h"

/* tyepdefs ******************************************************************/
MM_PACKED_START
/** @brief Saved timestamps and events */
typedef union {
    struct {
        uint32_t index; /**< Index of the current trace */
        uint8_t tick_div[128]; /**< The tick divisor of the event - max should be 16 for interface */
        uint8_t source[128]; /**< The event source of the event - 0:no source selected - 1:DEBUG0 pin - 2:DEBUG1 pin - 3:DEBUG2 pin - 4:DUT_IC */
        uint16_t value[128]; /**< The value of the event - 0:falling edge interrupt - 1:rising edge interrupt */
        uint32_t tick[128]; /**< The tick when the event occurred */
    };
    uint8_t data[1028]; /**< Array for padding */
} trace_t;
MM_PACKED_END

#ifdef __cplusplus
}
#endif

#endif /* MM_TYPEDEFS_TRACE_T_H */
/** @} **/