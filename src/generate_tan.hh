#ifndef BROWSER_USBTAN_GENERATE_TAN_H_
#define BROWSER_USBTAN_GENERATE_TAN_H_

class HhdCommand;

struct TanResult {
  int ATC;
  char Tan[30];
  char Cardnummber[11];
  char EndDate[5];
  char IssueDate[7];
};

TanResult generate_tan(HhdCommand& cmd);
#endif //  BROWSER_USBTAN_GENERATE_TAN_H_
