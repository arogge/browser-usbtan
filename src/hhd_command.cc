#include <cstdint>
#include <stdexcept>
#include <gsl/span>
#include <gsl/gsl_util>
#include <stddef.h>

#include "hhd_command.hh"

static uint8_t char2int(char input)
{
  if (input >= '0' && input <= '9') return input - '0';
  if (input >= 'A' && input <= 'F') return input - 'A' + 10;
  if (input >= 'a' && input <= 'f') return input - 'a' + 10;
  throw std::invalid_argument("Invalid input string");
}

HhdCommand::HhdCommand() { cardPrefix[4] = 1; }

const gsl::span<const uint8_t> HhdCommand::get_buffer() const
{
  return {buffer, gsl::narrow<size_t>(pos - buffer)};
}

void HhdCommand::add_data(char c)
{
  *(pos++) = c;
  *length = pos - (buffer + 8);
}

void HhdCommand::add_hex_data(char l, char r)
{
  add_data(char2int(l) * 16 + char2int(r));
}

