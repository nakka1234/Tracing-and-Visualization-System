#pragma once
#include <cstdint>
#include <string>

namespace trace {

enum class EventType {
    SPAN_START,
    SPAN_END,
    LOG
};

struct Event {
    EventType type;

    uint64_t timestamp;

    uint64_t span_id;
    uint64_t parent_span_id;

    uint64_t thread_id;
    uint32_t process_id;

    std::string message;

    std::string file;
    std::string func;
    int line;

    int level; // for LOG events
};

} // namespace trace