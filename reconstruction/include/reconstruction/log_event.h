#pragma once
#include <string>
#include <cstdint>

namespace reconstruction {

struct LogEvent {
    uint64_t ts = 0;
    int level = 0;
    std::string msg;

    std::string file;
    std::string func;
    int line = 0;
};

} // namespace reconstruction