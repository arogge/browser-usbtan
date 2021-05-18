#include "generate_tan.hh"
#include "hhd_command.hh"
#include "resolve_func.h"
#include <gsl/span>
#include <iostream>
#include <stdint.h>
#include <stdlib.h>

typedef int (*GetTanfromUSB_GeneratorFn)(unsigned char* HHDCommand,
                                         int fullHHD_Len,
                                         int* pATC,
                                         char* pGeneratedTAN,
                                         uint32_t maxTanLen,
                                         char* pCardnummber,
                                         char* pEndDate,
                                         char* IssueDate);

TanResult generate_tan(HhdCommand& cmd) {
  static GetTanfromUSB_GeneratorFn get_tan
      = (GetTanfromUSB_GeneratorFn)resolve_chiptanusb_func(
          "GetTanfromUSB_Generator");
  if(!get_tan) {
    std::cerr << "Could not load TAN module\n";
    exit(1);
  }

  TanResult res;
  if (get_tan((unsigned char*)cmd.get_buffer().data(),
              cmd.get_buffer().size_bytes(), &res.ATC, res.Tan,
              sizeof(res.Tan) - 1, res.Cardnummber, res.EndDate, res.IssueDate)
      < 0) {
    std::cerr << "Error generating TAN\n";
    exit(1);
  }
  return res;
}
