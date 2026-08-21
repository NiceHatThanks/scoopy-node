#pragma once
#include <cstddef>
#include <cstdint>
#include <span>

#include "esphome/core/helpers.h"
#include "esphome/core/string_ref.h"

namespace esphome::captive_portal {

static constexpr size_t JSON_ESCAPE_MAX_EXPANSION = 6;

inline const char *json_escape_into_buffer(std::span<char> buf, StringRef value) {
  if (buf.empty())
    return "";
  const size_t limit = buf.size() - 1;
  size_t pos = 0;
  for (char ch : value) {
    auto c = static_cast<unsigned char>(ch);
    char escape = '\0';
    switch (c) {
      case '"': escape = '"'; break;
      case '\\': escape = '\\'; break;
      case '\n': escape = 'n'; break;
      case '\r': escape = 'r'; break;
      case '\t': escape = 't'; break;
      case '\b': escape = 'b'; break;
      case '\f': escape = 'f'; break;
      default: break;
    }
    if (escape != '\0') {
      if (pos + 2 > limit) break;
      buf[pos++] = '\\';
      buf[pos++] = escape;
    } else if (c < 0x20) {
      if (pos + JSON_ESCAPE_MAX_EXPANSION > limit) break;
      buf[pos++] = '\\';
      buf[pos++] = 'u';
      buf[pos++] = '0';
      buf[pos++] = '0';
      buf[pos++] = format_hex_char(static_cast<uint8_t>(c >> 4));
      buf[pos++] = format_hex_char(static_cast<uint8_t>(c & 0x0F));
    } else {
      if (pos + 1 > limit) break;
      buf[pos++] = static_cast<char>(c);
    }
  }
  buf[pos] = '\0';
  return buf.data();
}

}  // namespace esphome::captive_portal
