/**
 ******************************************************************************
 * @addtogroup typedefs_mmm
 * @{
 * @file      mm_typedefs_rtc_t.h
 * @version   2.0.0
 *
 * @details   Generated from the memory map manager version 2.0.0
 ******************************************************************************
 */
#ifndef MM_TYPEDEFS_RTC_T_H
#define MM_TYPEDEFS_RTC_T_H

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
/** @brief Controls the RTC */
typedef union {
    struct {
        basic_mode_t mode; /**< basic mode for rtc settings */
        uint8_t second; /**< Seconds of rtc */
        uint8_t minute; /**< Minutes of rtc */
        uint8_t hour; /**< Hours of rtc */
        uint16_t day; /**< Days of rtc */
        uint8_t set_second; /**< Seconds to set of rtc */
        uint8_t set_minute; /**< Minutes to set of rtc */
        uint8_t set_hour; /**< Hours to set of rtc */
        uint16_t set_day; /**< Days to set of rtc */
        uint8_t padding[5]; /**< padding bytes */
    };
    uint8_t data[16]; /**< Array for padding */
} rtc_t;
MM_PACKED_END

#ifdef __cplusplus
}
#endif

#endif /* MM_TYPEDEFS_RTC_T_H */
/** @} **/