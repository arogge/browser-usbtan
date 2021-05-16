/**
 * usbtan-cli
 *
 * by Hermann Höhne hoehermann@gmx.de.
 * Largely based on work by Ellebruch Herbert.
 */

/*
aqbanking-5.99.44beta/src/libs/plugins/backends/aqhbci/tan/tan_chiptan_opt.c
*/

#ifndef BROWSER_USBTAN_FLICKERCODE_H_
#define BROWSER_USBTAN_FLICKERCODE_H_

#include <gwenhywfar/buffer.h>

#ifdef __cplusplus
extern "C" {
#endif
int _readBytesHex(const char* p, int len);
int _translate(const char* code, GWEN_BUFFER* cbuf);

#ifdef __cplusplus
}
#endif
#endif  // BROWSER_USBTAN_FLICKERCODE_H_
