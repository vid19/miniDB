#pragma once

#include <cstdint>
#include <string>

namespace minidb {

using Key = std::int64_t;

struct Row {
  Key id{};
  std::string value;
};

}  // namespace minidb
