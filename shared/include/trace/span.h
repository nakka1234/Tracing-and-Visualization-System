#pragma once
#include <cstdint>

namespace trace {

struct Span {
    uint64_t span_id;
    uint64_t parent_span_id;
    const char* name;
};

}