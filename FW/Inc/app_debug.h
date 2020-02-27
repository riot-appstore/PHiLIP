/*
 * Copyright 2020 Kevin Weiss for HAW Hamburg
 *
 * This file is subject to the terms and conditions of the MIT License. See the
 * file LICENSE in the top level directory for more details.
 * SPDX-License-Identifier:    MIT
 */

/*
 * app_debug.h
 *
 *  Created on: May 19, 2018
 *      Author: kevinweiss
 */

#ifndef APP_DEBUG_H_
#define APP_DEBUG_H_

void debug_print(char *str);

#define DEBUG_PRINT

#ifdef DEBUG_PRINT
#define DEBUG_MSG(x) debug_print(x)
#elif
#define DEBUG_MSG(x)
#endif

#endif /* APP_DEBUG_H_ */
