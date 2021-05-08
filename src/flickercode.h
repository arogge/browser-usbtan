/**
 * usbtan-cli
 *
 * by Hermann Höhne hoehermann@gmx.de.
 * Largely based on work by Ellebruch Herbert.
 */

#ifndef BROWSER_USBTAN_FLICKERCODE_H_
#define BROWSER_USBTAN_FLICKERCODE_H_

#include <gwenhywfar/buffer.h>

#ifdef __cplusplus
extern "C" {
#endif
// int _readBytesDec(const char* p, int len);
int _readBytesHex(const char* p, int len);
// unsigned int _quersumme(unsigned int i);
// int _extractDataForLuhnSum(const char* code, GWEN_BUFFER* xbuf);
// int _calcLuhnSum(const char* code, int len);
// int _calcXorSum(const char* code, int len);
// int __translate(const char* code, GWEN_BUFFER* cbuf);
int _translate(const char* code, GWEN_BUFFER* cbuf);
// int __translateWithLen(const char* code, GWEN_BUFFER* cbuf, int sizeLen);

#ifdef __cplusplus
}
#endif
#endif  // BROWSER_USBTAN_FLICKERCODE_H_
