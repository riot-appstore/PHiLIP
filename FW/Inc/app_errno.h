/*
 * app_errno.h
 *
 *  Created on: May 17, 2018
 *      Author: kevinweiss
 */

#ifndef APP_ERRNO_H_
#define APP_ERRNO_H_

#ifndef EOK
#define EOK 0
#endif

#ifndef EUNKNOWN
#define EUNKNOWN (__ELASTERROR + 1)
#endif

#ifndef ENOACTION
#define ENOACTION (__ELASTERROR + 2)
#endif

#endif /* APP_ERRNO_H_ */
