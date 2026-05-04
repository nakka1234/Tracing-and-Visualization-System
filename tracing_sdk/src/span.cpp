#include "trace/trace.h"
#include "trace/context.h"
#include "trace/event.h"
#include "logger/logger.h"

#include <atomic>
#include <thread>
#include <chrono>

#ifdef _WIN32
#define NOMINMAX
#include <windows.h>
#else
#include <unistd.h>
#endif

namespace trace {

static std::atomic<uint64_t> global_span_id{1};

// ==========================
// Helpers
// ==========================

static uint64_t get_timestamp() {
    using namespace std::chrono;
    return duration_cast<microseconds>(
        steady_clock::now().time_since_epoch()
    ).count();
}

static uint32_t get_pid() {
#ifdef _WIN32
    return GetCurrentProcessId();
#else
    return getpid();
#endif
}

static uint64_t get_tid() {
    return std::hash<std::thread::id>{}(std::this_thread::get_id());
}

// ==========================
// TraceScope Implementation
// ==========================

TraceScope::TraceScope(const char* name) {
    span_id = global_span_id.fetch_add(1);

    prev_span_id = get_current_span();
    set_current_span(span_id);

    this->name = name;

    trace::Event ev{};
    ev.type = trace::EventType::SPAN_START;

    ev.span_id = span_id;
    ev.parent_span_id = prev_span_id;  // ✅ valid

    ev.timestamp = get_timestamp();
    ev.thread_id = get_tid();
    ev.process_id = get_pid();

    ev.message = name;

    logger::push(ev);
}

TraceScope::~TraceScope() {

    trace::Event ev{};
    ev.type = trace::EventType::SPAN_END;

    ev.span_id = span_id;
    ev.parent_span_id = prev_span_id;  // ✅ valid

    ev.timestamp = get_timestamp();
    ev.thread_id = get_tid();
    ev.process_id = get_pid();

    ev.message = name;

    logger::push(ev);

    set_current_span(prev_span_id);
}

// ==========================
// DBG_MSG Implementation
// ==========================

void dbg_msg(LogLevel level,
             const char* msg,
             const char* file,
             const char* func,
             int line) {

    uint64_t current_span = get_current_span();

    trace::Event ev{};
    ev.type = trace::EventType::LOG;

    ev.span_id = current_span;
    ev.parent_span_id = 0;  // ✅ not used for logs

    ev.timestamp = get_timestamp();
    ev.thread_id = get_tid();
    ev.process_id = get_pid();

    ev.message = msg;
    ev.level = static_cast<int>(level);
    ev.file = file;
    ev.func = func;
    ev.line = line;

    logger::push(ev);
}

} // namespace trace