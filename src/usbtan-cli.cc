/**
 * usbtan-cli
 *
 * by Hermann Höhne hoehermann@gmx.de.
 * Largely based on work by Ellebruch Herbert.
 */

#include <gsl/span>
#include <gwenhywfar/buffer.h>
#include <iomanip>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <string_view>

#include "flickercode.h"
#include "hhd_command.hh"
#include "generate_tan.hh"

using std::cerr;


static bool parse_cmdline(int argc, char** argv, gsl::span<char> out_str)
{
  if (argc <= 1) {
    cerr << "Leere Anfrage.\n";
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
    cerr << "Anfrageformatierung fehlgeschlagen\n";
    return false;
  } else if (static_cast<size_t>(res) > out_str.size_bytes()) {
    cerr << "Anfrage zu lang\n";
    return false;
  } else {
    return true;
  }
}

static void pad(size_t len)
{
  for (size_t i = len; i > 0; i--) { printf("#"); }
}

int main(int argc, char** argv)
{
  using namespace std::literals;

  char RequestString[200];
  if (!parse_cmdline(argc, argv, RequestString)) { exit(1); }

  /* translate challenge string to flicker code */
  GWEN_BUFFER* cbuf = GWEN_Buffer_new(0, 256, 0, 1);
  _translate(RequestString, cbuf);

  gsl::span<const char> cbuf_view{GWEN_Buffer_GetStart(cbuf),
                                  GWEN_Buffer_GetUsedBytes(cbuf)};

  //HhdCommand cmd{cbuf_view};
  //for (const auto& c : cmd.get_buffer()) { printf("%c", c); }
  //pad(16 - cmd.get_buffer().size_bytes() % 16);
  //
  HhdCommand cmd{"1F85012392302620564445373733373036393532303033303634343530353701"sv};
  // for (const auto& c : cmd2.get_buffer()) { printf("%c", c); }
  // pad(16 - cmd2.get_buffer().size_bytes() % 16);

  TanResult res = generate_tan(cmd);

  printf("Ergebnis fuer TAN\r\n");
  printf("TAN = %s\r\n", res.Tan);
  printf("ATC = %d\r\n", res.ATC);
  printf("Kartennummer = %s\r\n", res.Cardnummber);
  printf("EndeDatum = %s\r\n", res.EndDate);
  printf("Ausgabedatum = %s\r\n", res.IssueDate);

  return 0;
}
