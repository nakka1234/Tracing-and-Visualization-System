#pragma once
#include <cstdint>

namespace trace {

uint64_t get_current_span();
void set_current_span(uint64_t id);

} // namespace trace