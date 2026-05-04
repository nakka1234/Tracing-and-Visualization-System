#include "logger/logger.h"
#include "trace/event.h"

#include <queue>
#include <mutex>
#include <thread>
#include <condition_variable>
#include <fstream>
#include <atomic>

namespace logger {
static const char* type_to_string(trace::EventType t) {
    switch (t) {
        case trace::EventType::SPAN_START: return "SPAN_START";
        case trace::EventType::SPAN_END: return "SPAN_END";
        case trace::EventType::LOG: return "LOG";
        default: return "UNKNOWN";
    }
}

static std::queue<trace::Event> q;
static std::mutex mtx;
static std::condition_variable cv;

static std::atomic<bool> running{false};
static std::thread worker;
static std::ofstream out;

static void worker_fn() {
    while (running || !q.empty()) {
        std::unique_lock<std::mutex> lock(mtx);

        cv.wait(lock, [] {
            return !q.empty() || !running;
        });

        while (!q.empty()) {
            auto ev = q.front();
            q.pop();

            lock.unlock();

            out << "{";

out << "\"type\":\"" << type_to_string(ev.type) << "\",";
out << "\"span\":" << ev.span_id << ",";
if (ev.type != trace::EventType::LOG) {
    out << "\"parent\":" << ev.parent_span_id << ",";
}

out << "\"ts\":" << ev.timestamp << ",";
out << "\"tid\":" << ev.thread_id << ",";
out << "\"pid\":" << ev.process_id << ",";

out << "\"msg\":\"" << (!ev.message.empty() ? ev.message : "") << "\"";

if (ev.type == trace::EventType::LOG) {
    out << ",\"level\":" << ev.level;
    out << ",\"file\":\"" << (!ev.file.empty() ? ev.file : "") << "\"";
    out << ",\"func\":\"" << (!ev.func.empty() ? ev.func : "") << "\"";
    out << ",\"line\":" << ev.line;
}

out << "}\n";

            lock.lock();
        }
    }
}

void init() {
    out.open("C:\\vs_projects\\tracing_system\\trace.log");
    running = true;
    worker = std::thread(worker_fn);
}

void shutdown() {
    running = false;
    cv.notify_all();

    if (worker.joinable())
        worker.join();

    if (out.is_open()) {
        out.flush();
        out.close();
    }
}

void push(const trace::Event& ev) {
    {
        std::lock_guard<std::mutex> lock(mtx);
        q.push(ev);
    }
    cv.notify_one();
}

}