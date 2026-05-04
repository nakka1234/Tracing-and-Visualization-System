#include "trace/trace.h"
#include "logger/logger.h"
#include <chrono>
#include <thread>

using namespace trace;

// Synchronous: compute task (direct call)
void compute() {
    TRACE_SCOPE("compute");
    DBG_MSG(LogLevel::LOG_INFO, "computing");
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    DBG_MSG(LogLevel::LOG_INFO, "compute done");
}

// Synchronous: I/O task
void io_read() {
    TRACE_SCOPE("io_read");
    DBG_MSG(LogLevel::LOG_INFO, "reading file");
    std::this_thread::sleep_for(std::chrono::milliseconds(5));
    DBG_MSG(LogLevel::LOG_INFO, "read complete");
}

// Asynchronous: background task
void background_task(int id) {
    TRACE_SCOPE("background_task");
    DBG_MSG(LogLevel::LOG_WARN, "background task started");
    std::this_thread::sleep_for(std::chrono::milliseconds(15));
    DBG_MSG(LogLevel::LOG_INFO, "background task finished");
}

// Error handling task
void risky_operation() {
    TRACE_SCOPE("risky_operation");
    DBG_MSG(LogLevel::LOG_INFO, "attempting risky operation");
    std::this_thread::sleep_for(std::chrono::milliseconds(8));
    DBG_MSG(LogLevel::LOG_ERROR, "operation failed (expected)");
    DBG_MSG(LogLevel::LOG_INFO, "recovery initiated");
}

// Worker pool: spawn multiple async tasks
void worker_pool() {
    TRACE_SCOPE("worker_pool");
    DBG_MSG(LogLevel::LOG_INFO, "spawning worker threads");

    auto w1 = spawn([] { background_task(1); });
    auto w2 = spawn([] { background_task(2); });
    auto w3 = spawn([] { background_task(3); });

    DBG_MSG(LogLevel::LOG_INFO, "workers spawned, waiting for completion");
    w1.join();
    w2.join();
    w3.join();
    DBG_MSG(LogLevel::LOG_INFO, "all workers done");
}

// Sync -> Async pattern: call sync functions, then spawn async tasks
void sync_then_async() {
    TRACE_SCOPE("sync_then_async");
    
    DBG_MSG(LogLevel::LOG_INFO, "phase 1: sync operations");
    compute();
    io_read();

    DBG_MSG(LogLevel::LOG_INFO, "phase 2: async operations");
    auto async_task = spawn([] { risky_operation(); });
    DBG_MSG(LogLevel::LOG_INFO, "async task spawned, continuing");

    async_task.join();
    DBG_MSG(LogLevel::LOG_INFO, "async task joined");
}

// Mixed async/sync with nested calls
void mixed_workload() {
    TRACE_SCOPE("mixed_workload");
    
    DBG_MSG(LogLevel::LOG_INFO, "starting mixed workload");

    // Direct sync call
    compute();

    // Spawn async background work
    auto bg1 = spawn([] { 
        TRACE_SCOPE("bg_thread_1");
        DBG_MSG(LogLevel::LOG_INFO, "bg thread 1 active");
        io_read();
    });

    auto bg2 = spawn([] {
        TRACE_SCOPE("bg_thread_2");
        DBG_MSG(LogLevel::LOG_WARN, "bg thread 2 active");
        risky_operation();
    });

    // Main thread continues
    DBG_MSG(LogLevel::LOG_INFO, "main thread continuing");
    
    // Wait for background threads
    bg1.join();
    bg2.join();
    
    DBG_MSG(LogLevel::LOG_INFO, "mixed workload complete");
}

int main() {
    logger::init();
    DBG_MSG(LogLevel::LOG_INFO, "program starting");

    {
        TRACE_SCOPE("main");
        
        DBG_MSG(LogLevel::LOG_INFO, "=== Sync then Async ===");
        sync_then_async();

        DBG_MSG(LogLevel::LOG_INFO, "=== Worker Pool ===");
        worker_pool();

        DBG_MSG(LogLevel::LOG_INFO, "=== Mixed Workload ===");
        mixed_workload();

        DBG_MSG(LogLevel::LOG_INFO, "all tasks complete");
    }

    logger::shutdown();
    return 0;
}