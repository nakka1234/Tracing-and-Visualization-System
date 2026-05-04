#pragma once
#include <unordered_map>
#include <vector>

#include "trace/event.h"
#include "reconstruction/span_node.h"

namespace reconstruction {

class SpanBuilder {
public:
    void on_event(const trace::Event& ev);
    std::vector<SpanNode*> build();

private:
    std::unordered_map<uint64_t, SpanNode*> spans;
    std::vector<SpanNode*> roots;
};

} // namespace reconstruction