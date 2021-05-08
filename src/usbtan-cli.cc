/**
 * usbtan-cli
 *
 * by Hermann Höhne hoehermann@gmx.de.
 * Largely based on work by Ellebruch Herbert.
 */

#include <chipcard/client.h>
#include <ctype.h>
#include <gwenhywfar/args.h>
#include <gwenhywfar/cgui.h>
#include <gwenhywfar/ct.h>
#include <gwenhywfar/ctplugin.h>
#include <gwenhywfar/debug.h>
#include <gwenhywfar/text.h>
#include <gwenhywfar/url.h>

#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <gsl/gsl>
#include <stdio.h>

#include "flickercode.h"

#define PROGRAM_VERSION "0.1"

// **************************************************** routine from aqbanking
// start

static char RequestString[101] = "280888113176100001234567041,23";  // Test Tan

/* Kopie von
aqbanking-5.99.44beta/src/libs/plugins/backends/aqhbci/tan/tan_chiptan_opt.c
*/


typedef int (*GetTanfromUSB_GeneratorFn)(unsigned char* HHDCommand,
                                         int fullHHD_Len,
                                         int* pATC,
                                         char* pGeneratedTAN,
                                         uint32_t maxTanLen,
                                         char* pCardnummber,
                                         char* pEndDate,
                                         char* IssueDate);

static bool parse_cmdline(int argc, char** argv, gsl::span<char> out_str)
{
  if (argc <= 1) {
    std::cerr << "Leere Anfrage.\n";
    return false;
  }

  std::stringstream request_sstr{};
  for (int i = 1; i < argc; i++) {
    request_sstr << std::setfill('0') << std::setw(2) << strlen(argv[i])
                 << argv[i];
  }
  int res = snprintf(out_str.data(), out_str.size_bytes(), "%02zu%s",
                     request_sstr.str().length(), request_sstr.str().c_str());
  if (res < 0) {
    std::cerr << "Anfrageformatierung fehlgeschlagen\n";
    return false;
  } else if (static_cast<size_t>(res) > out_str.size_bytes()) {
    std::cerr << "Anfrage zu lang\n";
    return false;
  } else {
    return true;
  }
}

static void* resolve_func(const char* func)
{
  GWEN_PLUGIN_MANAGER* pm = GWEN_PluginManager_FindPluginManager("ct");
  if (!pm) {
    DBG_ERROR(0, "Plugin manager not found");
    return nullptr;
  }

  GWEN_PLUGIN* pl = GWEN_PluginManager_GetPlugin(pm, "chiptanusb");
  if (!pl) {
    DBG_ERROR(0, "Plugin not found");
    return nullptr;
  }
  DBG_INFO(0, "Plugin found");

  GWEN_LIBLOADER* ll = GWEN_Plugin_GetLibLoader(pl);

  void* p;
  if (GWEN_LibLoader_Resolve(ll, func, &p) < 0) { return nullptr; }
  return p;
}

int main(int argc, char** argv)
{
  if (!parse_cmdline(argc, argv, RequestString)) { exit(1); }
  printf("RequestString = %s\r\n", RequestString);


  /* translate challenge string to flicker code */
  GWEN_BUFFER* cbuf = GWEN_Buffer_new(0, 256, 0, 1);
  _translate(RequestString, cbuf);

  /* do something to a buffer */
  // create command buffer and initialize with cardprefix
  char HHDCommand[200];
  memset(HHDCommand, 0xff, sizeof(HHDCommand));
  static char cardPrefix[] = {0, 0, 0, 0, 1, 0, 0};
  memcpy(HHDCommand, cardPrefix, sizeof(cardPrefix));

  uint8_t HHD_Generator_Len = GWEN_Buffer_GetUsedBytes(cbuf) / 2;
  int fullHHD_Len = HHD_Generator_Len + sizeof(cardPrefix) + 1;
  char* pHHDSrc = GWEN_Buffer_GetStart(cbuf);

  HHDCommand[sizeof(cardPrefix)] = HHD_Generator_Len;
  // fwrite(HHDCommand, sizeof(HHDCommand), 1, stdout);
  // printf("XXXXXXXX");

  // take a pointer 9 bytes into the buffer
  char* pHHDDest = &HHDCommand[8];
  for (int i = 0; i < HHD_Generator_Len; i++) {
    *pHHDDest++ = _readBytesHex(pHHDSrc, 2);
    pHHDSrc++;
    pHHDSrc++;
  }
  /* something done to the buffer */
  // fwrite(HHDCommand, sizeof(HHDCommand), 1, stdout);
  // printf("XXXXXXXX");

  GetTanfromUSB_GeneratorFn get_tan
      = (GetTanfromUSB_GeneratorFn)resolve_func("GetTanfromUSB_Generator");

  int ATC;
  char GeneratedTAN[30];
  char Cardnummber[11];
  char EndDate[5];
  char IssueDate[7];

  if (get_tan((unsigned char*)HHDCommand, fullHHD_Len, &ATC, GeneratedTAN,
              sizeof(GeneratedTAN) - 1, Cardnummber, EndDate, IssueDate)
      < 0) {
    std::cerr << "Fehler bei TAN Generierung\n";
    exit(1);
  }

  printf("Ergebnis fuer TAN\r\n");
  printf("TAN = %s\r\n", GeneratedTAN);
  printf("ATC = %d\r\n", ATC);
  printf("Kartennummer = %s\r\n", Cardnummber);
  printf("EndeDatum = %s\r\n", EndDate);
  printf("Ausgabedatum = %s\r\n", IssueDate);

  return 0;
}
