#include <cstdio>
#include <string>
#include <cassert>
#include <gsl/gsl>
#include <nlohmann/json.hpp>

constexpr ssize_t max_msg_size = 1024 * 1024;

using namespace std::string_literals;
using json = nlohmann::json;

json read_request(FILE* fp) {
  uint32_t len;
  std::vector<char> buf(max_msg_size);

  fread(&len, sizeof(len), 1, fp);
  assert(len <= max_msg_size); // message size limit is 1 MB
  fread(buf.data(), len, 1, fp);
  return json::parse(buf);
}

void emit_response(FILE* fp, json response) {
  auto respstr = response.dump();
  auto len = gsl::narrow<uint32_t>(respstr.length());
  assert(len <= max_msg_size); // message size limit is 1 MB

  // protocol says: length in 32-bit native byte-order followed by data
  fwrite(&len, sizeof(len), 1, fp);
  fprintf(fp, "%s", respstr.c_str());
}

int main() {
  auto js = read_request(stdin);
  auto flickercode = js["flickercode"].get<std::string>();
  std::reverse(flickercode.begin(), flickercode.end());
  js["tan"] = flickercode;
  emit_response(stdout, js);
  return 0;
}
