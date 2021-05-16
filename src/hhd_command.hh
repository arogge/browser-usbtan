#ifndef BROWSER_USBTAN_HHD_COMMAND_H_
#define BROWSER_USBTAN_HHD_COMMAND_H_

#include <cstdint>
#include <gsl/span>

class HhdCommand {
  uint8_t buffer[7 + 1 + 256]{};
  const gsl::span<uint8_t, 7> cardPrefix{buffer, 7};
  uint8_t* const length = buffer + 7;
  uint8_t* pos = buffer + 8;

 public:
  HhdCommand();
  template <typename Range>
  HhdCommand(Range r) : HhdCommand(r.begin(), r.end())
  {
  }

  template <typename Iterator>
  HhdCommand(Iterator begin, Iterator end) : HhdCommand()
  {
    while (begin != end) {
      char l = *(begin++);
      char r = *(begin++);
      add_hex_data(l, r);
    }
  }
  const gsl::span<const uint8_t> get_buffer() const;

 private:
  void add_data(char c);
  void add_hex_data(char l, char r);
};
#endif //  BROWSER_USBTAN_HHD_COMMAND_H_
