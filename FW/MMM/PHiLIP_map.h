/**
 ******************************************************************************
 * @addtogroup PHiLIP_MMM
 * @{
 * @file      PHiLIP_map.h
 * @author    Kevin Weiss
 * @version   1.0.0
 * @date      2019-05-08
 * @details   Generated from the memory map manager version 0.0.9
 ******************************************************************************
 */

#ifndef PHILIP_MAP_H
#define PHILIP_MAP_H

/* Defines -----------------------------------------------------------------*/
#define IF_VERSION_MAJOR 1 /**< Major version of interface */
#define IF_VERSION_MINOR 0 /**< Minor version of interface */
#define IF_VERSION_PATCH 0 /**< Patch version of interface */

/* Global variables --------------------------------------------------------*/
extern const char* const PHILIP_TYPE_NAME[]; /** < type_name enum */
extern const uint8_t  PHILIP_TYPE_SIZE[]; /** <  type_size const array */

#define MAP_T_NUM_OF_RECORDS 169 /**< Number of records in the map */

extern const char* const MAP_T_NAME[]; /** < name const array */
extern const uint16_t MAP_T_OFFSET[]; /** < offset const array */
extern const uint8_t MAP_T_TYPE[]; /** < type const array */
extern const uint16_t MAP_T_ARRAY_SIZE[]; /** < array_size const array */
extern const uint8_t MAP_T_BIT_OFFSET[]; /** < bit_offset const array */
extern const uint8_t MAP_T_BITS[]; /** < bits const array */
#endif /* PHILIP_MAP_H */
/** @} */
