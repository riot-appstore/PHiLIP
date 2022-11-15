/**
 ******************************************************************************
 * @addtogroup cc_mmm
 * @{
 * @file      mm_cc.h
 * @version   2.0.0
 *
 * @details   Generated from the memory map manager version 2.0.0
 ******************************************************************************
 */
#ifndef MM_CC_H
#define MM_CC_H

#ifdef __cplusplus
extern "C"
{
#endif

#ifdef __linux__
   #include <endian.h>
#else
   #include <machine/endian.h>
#endif

#define MM_PACKED_START _Pragma ("pack(1)")
#define MM_PACKED_END   _Pragma ("pack()")
#define MM_PACKED       __attribute__((packed))
#define MM_ALIGNED(n)   __attribute__((aligned (n)))

#if BYTE_ORDER == BIG_ENDIAN
//#error The mmm only supports little endian
#endif

#ifdef __cplusplus
}
#endif

#endif /* MM_CC_H */
/** @} **/