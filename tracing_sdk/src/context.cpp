#include "trace/context.h"

namespace trace {

thread_local uint64_t current_span_id = 0;

uint64_t get_current_span() {
    return current_span_id;
}

void set_current_span(uint64_t id) {
    current_span_id = id;
}

}