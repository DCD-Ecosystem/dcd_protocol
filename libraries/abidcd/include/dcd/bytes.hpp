#pragma once

#include "from_json.hpp"
#include "to_json.hpp"
#include "operators.hpp"
#include <vector>

namespace dcd {

struct bytes {
   std::vector<char> data;
};

DCD_REFLECT(bytes, data);
DCD_COMPARE(bytes);

template <typename S>
void from_json(bytes& obj, S& stream) {
   return dcd::from_json_hex(obj.data, stream);
}

template <typename S>
void to_json(const bytes& obj, S& stream) {
   return dcd::to_json_hex(obj.data.data(), obj.data.size(), stream);
}

} // namespace dcd
