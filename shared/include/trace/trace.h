#pragma once
#include "trace/context.h"
#include <cstdint>
#include <thread>
#include <utility> 



namespace trace {

// ✅ FIXED enum
enum class LogLevel {
    LOG_INFO,
    LOG_WARN,
    LOG_ERROR
};

template <typename Fn>
std::thread spawn(Fn&& fn) {
    uint64_t parent = get_current_span();

    return std::thread([parent, f = std::forward<Fn>(fn)]() mutable {
        set_current_span(parent);
        f();
    });
}
// ✅ Class declaration
class TraceScope {
    uint64_t span_id;
    uint64_t prev_span_id;
    const char* name;

public:
    TraceScope(const char* name);
    ~TraceScope();
};

// ✅ dbg API
void dbg_msg(LogLevel level,
             const char* msg,
             const char* file,
             const char* func,
             int line);

} // namespace trace

// macros (OUTSIDE namespace)
#define TRACE_SCOPE(name) trace::TraceScope trace_scope_##__LINE__(name)

#define DBG_MSG(level, msg) \
    trace::dbg_msg(level, msg, __FILE__, __FUNCTION__, __LINE__)