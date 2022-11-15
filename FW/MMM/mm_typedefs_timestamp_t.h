/**
 ******************************************************************************
 * @addtogroup typedefs_mmm
 * @{
 * @file      mm_typedefs_timestamp_t.h
 * @version   2.0.0
 *
 * @details   Generated from the memory map manager version 2.0.0
 ******************************************************************************
 */
#ifndef MM_TYPEDEFS_TIMESTAMP_T_H
#define MM_TYPEDEFS_TIMESTAMP_T_H

#ifdef __cplusplus
extern "C"
{
#endif

/* includes ******************************************************************/
#include <stdint.h>

#include "mm_cc.h"

/* tyepdefs ******************************************************************/
MM_PACKED_START
/** @brief Time and date */
typedef union {
    struct {
        uint8_t second; /**< The seconds in decimal */
        uint8_t minute; /**< The minutes in decimal */
        uint8_t hour; /**< The hours in decimal */
        uint8_t day_of_month; /**< The months in decimal */
        uint8_t day_of_week; /**< The week in decimal */
        uint8_t month; /**< The month in decimal */
        uint8_t year; /**< The last two digits of the year in decimal (20xx) */
        uint8_t padding[1]; /**< padding bytes */
    };
    uint8_t data[8]; /**< Array for padding */
} timestamp_t;
MM_PACKED_END

#ifdef __cplusplus
}
#endif

#endif /* MM_TYPEDEFS_TIMESTAMP_T_H */
/** @} **/