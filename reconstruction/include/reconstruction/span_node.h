#pragma once
#include <cstdint>
#include <string>
#include <vector>

#include "reconstruction/log_event.h"

namespace reconstruction {

struct SpanNode {
    uint64_t span_id = 0;
    uint64_t parent_id = 0;

    std::string name;

    uint64_t start_ts = 0;
    uint64_t end_ts = 0;

    uint64_t thread_id = 0;

    std::vector<SpanNode*> children;
    std::vector<LogEvent> logs;
};

} // namespace reconstruction