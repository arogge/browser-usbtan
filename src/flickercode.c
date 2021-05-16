/**
 * usbtan-cli
 *
 * by Hermann Höhne hoehermann@gmx.de.
 * Largely based on work by Ellebruch Herbert.
 */

/*
aqbanking-5.99.44beta/src/libs/plugins/backends/aqhbci/tan/tan_chiptan_opt.c
*/

#include <ctype.h>
#include <gwenhywfar/text.h>
#include <assert.h>
#include <gwenhywfar/error.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "flickercode.h"

static int _readBytesDec(const char* p, int len);
static unsigned int _quersumme(unsigned int i);
static int _extractDataForLuhnSum(const char* code, GWEN_BUFFER* xbuf);
static int _calcLuhnSum(const char* code, int len);
static int _calcXorSum(const char* code, int len);
static int __translate(const char* code, GWEN_BUFFER* cbuf);
static int __translateWithLen(const char* code, GWEN_BUFFER* cbuf, int sizeLen);

int _translate(const char* code, GWEN_BUFFER* cbuf)
{
  int rv;

  /* DBG_ERROR(AQHBCI_LOGDOMAIN, "HHD: Raw data is [%s]", code); */
  rv = __translate(code, cbuf);
  if (rv < 0) { return rv; }
  return 0;
}

int __translate(const char* code, GWEN_BUFFER* cbuf)
{
  GWEN_BUFFER* tmpBuf;
  int rv;

  tmpBuf = GWEN_Buffer_new(0, 256, 0, 1);
  /* DBG_INFO(AQHBCI_LOGDOMAIN, "Trying 3 bytes length"); */
  rv = __translateWithLen(code, tmpBuf, 3);
  if (rv < 0) {
    GWEN_Buffer_Reset(tmpBuf);
    /* DBG_INFO(AQHBCI_LOGDOMAIN, "Trying 2 bytes length"); */
    rv = __translateWithLen(code, tmpBuf, 2);
    if (rv < 0) {
      /* DBG_INFO(AQHBCI_LOGDOMAIN, "Invalid challenge data (%d)", rv); */
      GWEN_Buffer_free(tmpBuf);
      return rv;
    }
  }

  GWEN_Buffer_AppendBuffer(cbuf, tmpBuf);
  GWEN_Buffer_free(tmpBuf);
  return rv;
}

int __translateWithLen(const char* code, GWEN_BUFFER* cbuf, int sizeLen)
{
  unsigned int totalLength; /*handle total length */
  unsigned int inLenAndFlags;
  unsigned int inLen;
  unsigned int outLenAndFlags;
  unsigned int outLen;
  unsigned int codeLen;
  /* preset bit masks for HHD 1.4 */
  unsigned int maskLen = 0x3f;
  unsigned int maskAscFlag = 0x40;
  unsigned int maskCtlFlag = 0x80;

  int rv;
  GWEN_BUFFER* xbuf;
  char numbuf[16];
  uint32_t cBufStartPos;
  uint32_t cBufEndPos;
  unsigned int checkSum;

  assert(code);
  codeLen = strlen(code);

  xbuf = GWEN_Buffer_new(0, 256, 0, 1);

  /* read length */
  rv = _readBytesDec(code, sizeLen);
  if (rv < 0) {
    /* DBG_INFO(AQHBCI_LOGDOMAIN, "here (%d)", rv); */
    GWEN_Buffer_free(xbuf);
    return rv;
  }
  totalLength = (unsigned int)rv;
  if ((totalLength + sizeLen) > codeLen) {
    /* DBG_ERROR(AQHBCI_LOGDOMAIN, "Total length exceeds length of given code
     * (%d+%d > %d)", totalLength, sizeLen, codeLen);*/
    GWEN_Buffer_free(xbuf);
    return GWEN_ERROR_BAD_DATA;
  }
  code += sizeLen;

  /* translate startCode (length is in hex) */
  rv = _readBytesHex(code, 2);
  if (rv < 0) {
    /* DBG_INFO(AQHBCI_LOGDOMAIN, "here (%d)", rv); */
    GWEN_Buffer_free(xbuf);
    return rv;
  }
  inLenAndFlags = (unsigned int)rv;
  inLen = inLenAndFlags & maskLen;
  code += 2;

  outLen = (inLen + 1) / 2;
  outLenAndFlags = outLen | (inLenAndFlags & maskCtlFlag);
  snprintf(numbuf, sizeof(numbuf) - 1, "%02x", outLenAndFlags);
  numbuf[sizeof(numbuf) - 1] = 0;
  GWEN_Buffer_AppendString(xbuf, numbuf);

  /* copy control bytes, if necessary */
  if (inLenAndFlags & maskCtlFlag) {
    unsigned int ctrl = 0;

    do {
      /* control byte(s) follow (HHD1.4) */
      rv = _readBytesHex(code, 2);
      if (rv < 0) {
        /* DBG_INFO(AQHBCI_LOGDOMAIN, "here (%d)", rv);  */
        GWEN_Buffer_free(xbuf);
        return rv;
      }
      ctrl = (unsigned int)rv;
      /* write to output buffer */
      snprintf(numbuf, sizeof(numbuf) - 1, "%02x", ctrl);
      numbuf[sizeof(numbuf) - 1] = 0;
      GWEN_Buffer_AppendString(xbuf, numbuf);
      code += 2;
    } while (ctrl & maskCtlFlag);
  } else {
    /* DBG_ERROR(AQHBCI_LOGDOMAIN, "no control bytes fallback to HHD 1.3.2"); */
    maskLen = 0xf;
    maskAscFlag = 0x10;
  }

  if (inLen) {
    GWEN_Buffer_AppendBytes(xbuf, code, inLen);
    if (inLen % 2) /* fill with "F" if necessary */
      GWEN_Buffer_AppendByte(xbuf, 'F');
  }
  code += inLen;

  /* read DE's */
  while (*code) {
    /* input length is in dec usually no AscFlag for DE's is provided */
    rv = _readBytesDec(code, 2);
    if (rv < 0) {
      /* DBG_INFO(AQHBCI_LOGDOMAIN, "here (%d)", rv); */
      GWEN_Buffer_free(xbuf);
      return rv;
    }
    inLenAndFlags = (unsigned int)rv;
    inLen = inLenAndFlags & maskLen;
    code += 2;

    /* so we have to check whether we need to switch to ASC */
    if ((inLenAndFlags & maskAscFlag) == 0) {
      unsigned int i;

      for (i = 0; i < inLen; i++) {
        if (code[i] < '0' || code[i] > '9') {
          /* contains something other than digits, use ascii encoding */
          /* DBG_ERROR(AQHBCI_LOGDOMAIN, "Switched to ASCII"); */
          inLenAndFlags |= maskAscFlag;
          break;
        }
      }
    }

    /* write to outbuffer */
    if (inLenAndFlags & maskAscFlag) {
      /* ascii */
      outLen = inLen;
      outLenAndFlags
          = outLen | maskAscFlag; /* add encoding flag to length (bit 6 or 4) */
      snprintf(numbuf, sizeof(numbuf) - 1, "%02x", outLenAndFlags);
      numbuf[sizeof(numbuf) - 1] = 0;
      GWEN_Buffer_AppendString(xbuf, numbuf);
      if (inLen) /* hex encode data */
        GWEN_Text_ToHexBuffer(code, inLen, xbuf, 0, 0, 0);
      code += inLen;
    } else {
      /* bcd, pack 2 digits into 1 Byte */
      outLen = (inLen + 1) / 2;
      outLenAndFlags = outLen;
      snprintf(numbuf, sizeof(numbuf) - 1, "%02x", outLenAndFlags);
      numbuf[sizeof(numbuf) - 1] = 0;
      GWEN_Buffer_AppendString(xbuf, numbuf);
      if (inLen) {
        /* data is bcd, just copy */
        GWEN_Buffer_AppendBytes(xbuf, code, inLen);
        if (inLen % 2) /* fill with "F" if necessary */
          GWEN_Buffer_AppendByte(xbuf, 'F');
      }
      code += inLen;
    }
  } /* while */

  /* cbuf starts here */
  cBufStartPos = GWEN_Buffer_GetPos(cbuf);

  /* calculate full length (payload plus checksums) */
  outLen = (GWEN_Buffer_GetUsedBytes(xbuf) + 2 + 1) / 2;
  snprintf(numbuf, sizeof(numbuf) - 1, "%02x", outLen);
  numbuf[sizeof(numbuf) - 1] = 0;
  /* add length to outbuffer */
  GWEN_Buffer_AppendString(cbuf, numbuf);

  /* add translated buffer to output buffer */
  GWEN_Buffer_AppendBuffer(cbuf, xbuf);

  /* cbuf ends here */
  cBufEndPos = GWEN_Buffer_GetPos(cbuf);

  /* get payload for luhn sum */
  GWEN_Buffer_Reset(xbuf);
  rv = _extractDataForLuhnSum(GWEN_Buffer_GetStart(cbuf) + cBufStartPos, xbuf);
  if (rv < 0) {
    /* DBG_INFO(AQHBCI_LOGDOMAIN, "here (%d)", rv); */
    GWEN_Buffer_free(xbuf);
    return rv;
  }

  /* calculate luhn sum */
  rv = _calcLuhnSum(GWEN_Buffer_GetStart(xbuf), GWEN_Buffer_GetUsedBytes(xbuf));
  if (rv < 0) {
    /* DBG_INFO(AQHBCI_LOGDOMAIN, "here (%d)", rv); */
    GWEN_Buffer_free(xbuf);
    return rv;
  }
  checkSum = (unsigned int)rv;

  /* add luhn sum */
  if (checkSum > 9) checkSum += 7;
  checkSum += '0';
  GWEN_Buffer_AppendByte(cbuf, checkSum);

  /* calculate XOR sum */
  rv = _calcXorSum(GWEN_Buffer_GetStart(cbuf) + cBufStartPos,
                   cBufEndPos - cBufStartPos);
  if (rv < 0) {
    /* DBG_INFO(AQHBCI_LOGDOMAIN, "here (%d)", rv); */
    GWEN_Buffer_free(xbuf);
    return rv;
  }
  checkSum = (unsigned int)rv;

  /* add XOR sum */
  if (checkSum > 9) checkSum += 7;
  checkSum += '0';
  GWEN_Buffer_AppendByte(cbuf, checkSum);

  /* finish */
  GWEN_Buffer_free(xbuf);
  return 0;
}

int _readBytesDec(const char* p, int len)
{
  int r = 0;
  int i;

  for (i = 0; i < len; i++) {
    uint8_t c;

    c = *p;
    if (c == 0) {
      /* DBG_ERROR(AQHBCI_LOGDOMAIN, "Premature end of string"); */
      return GWEN_ERROR_PARTIAL;
    }
    if (c < '0' || c > '9') { return GWEN_ERROR_INVALID; }
    c -= '0';
    r *= 10;
    r += c;
    p++;
  }

  return r;
}

int _readBytesHex(const char* p, int len)
{
  unsigned int r = 0;
  int i;

  for (i = 0; i < len; i++) {
    uint8_t c;

    c = *p;
    if (c == 0) {
      /* DBG_ERROR(AQHBCI_LOGDOMAIN, "Premature end of string"); */
      return GWEN_ERROR_PARTIAL;
    }
    c = toupper(c);
    if ((c >= '0' && c <= '9') || (c >= 'A' && c <= 'F')) {
      c -= '0';
      if (c > 9) c -= 7;
      r *= 16;
      r += c;
      p++;
    } else {
      /* DBG_ERROR(AQHBCI_LOGDOMAIN, "Bad char in data (no hexadecimal digit)");
       */
      return GWEN_ERROR_INVALID;
    }
  }

  return (int)r;
}

unsigned int _quersumme(unsigned int i)
{
  unsigned int qs = 0;

  while (i) {
    qs += i % 10;
    i /= 10;
  }
  return qs;
}

int _extractDataForLuhnSum(const char* code, GWEN_BUFFER* xbuf)
{
  int rv;
  unsigned int len;
  unsigned int i = 0;
  unsigned int LSandFlags;
  unsigned int numCtrlBytes;
  unsigned int moreCtrlBytes;
  unsigned int numBytes;
  /* preset bit masks for HHD 1.4 */
  unsigned int maskLen = 0x3f;

  /* read LC */
  rv = _readBytesHex(code, 2);
  if (rv < 0) {
    /* DBG_INFO(AQHBCI_LOGDOMAIN, "here (%d) at [%s]", rv, code); */
    return rv;
  }
  len = ((unsigned int)rv);
  code += 2;

  if ((strlen(code) + 2) < len * 2) {
    /* DBG_ERROR(AQHBCI_LOGDOMAIN, "Too few bytes in buffer (%d<%d) at [%s]",
       (int)(strlen(code)+2), len*2, code); */
    return GWEN_ERROR_INVALID;
  }

  /* read LS */
  rv = _readBytesHex(code, 2);
  if (rv < 0) {
    /* DBG_INFO(AQHBCI_LOGDOMAIN, "here (%d) at [%s]", rv, code); */
    return rv;
  }
  code += 2;

  /* add control bytes and start code */
  LSandFlags = (unsigned int)rv;
  numCtrlBytes = 0;
  moreCtrlBytes = LSandFlags & 0x80;

  while (moreCtrlBytes) {
    rv = _readBytesHex(code + numCtrlBytes * 2, 2);
    if (rv < 0) {
      /* DBG_INFO(AQHBCI_LOGDOMAIN, "here (%d) at [%s]", rv, code); */
      return rv;
    }
    numCtrlBytes++;
    moreCtrlBytes = (unsigned int)rv & 0x80;
  }
  if (numCtrlBytes == 0) {
    /* set bit mask for HHD 1.3.2 */
    maskLen = 0x0f;
  }
  numBytes = (LSandFlags & maskLen) + numCtrlBytes;
  GWEN_Buffer_AppendBytes(xbuf, code, numBytes * 2);
  code += numBytes * 2;
  i += numBytes + 2; /* add length of LC and LS */

  /* read LDE1, DE1, LDE2, DE2, ... */
  while (i < len - 1) {
    unsigned int v;

    rv = _readBytesHex(code, 2);
    if (rv < 0) {
      /* DBG_INFO(AQHBCI_LOGDOMAIN, "here (%d) at [%s]", rv, code); */
      return rv;
    }
    /*    v=((unsigned int) rv) & 0xf; */
    v = ((unsigned int)rv) & maskLen;
    code += 2;
    if (i + v + 1 > len) {
      /* DBG_INFO(AQHBCI_LOGDOMAIN, "try to read past the end of code (%d) at
       * [%s]", v, code); */
      return GWEN_ERROR_INVALID;
    }
    GWEN_Buffer_AppendBytes(xbuf, code, v * 2);
    code += v * 2;
    i += v + 1;
  }

  return 0;
}

int _calcLuhnSum(const char* code, int len)
{
  int i;
  int sum = 0;
  int next;
  int dif;

  if (len % 2) {
    //* DBG_ERROR(AQHBCI_LOGDOMAIN, "Invalid number of bytes in payload (%d)",
    // len); */
    return GWEN_ERROR_INVALID;
  }

  for (i = 0; i < len; i += 2) {
    int rv;
    unsigned int v;

    rv = _readBytesHex(code, 2);
    if (rv < 0) {
      /* DBG_INFO(AQHBCI_LOGDOMAIN, "here (%d) at [%s]", rv, code); */
      return rv;
    }
    v = (unsigned int)rv;
    sum += (1 * ((v >> 4) & 0xf)) + (_quersumme(2 * (v & 0xf)));
    code += 2;
  }

  next = 10 * ((sum + 9) / 10);

  dif = next - sum;
  return (unsigned int)dif;
}

int _calcXorSum(const char* code, int len)
{
  int i;
  int sum = 0;

  for (i = 0; i < len; i++) {
    int rv;
    unsigned int v;

    rv = _readBytesHex(code, 1);
    if (rv < 0) {
      /* DBG_INFO(AQHBCI_LOGDOMAIN, "here (%d)", rv); */
      return rv;
    }
    v = (unsigned int)rv;
    sum ^= v;
    code++;
  }

  return (unsigned int)sum;
}
