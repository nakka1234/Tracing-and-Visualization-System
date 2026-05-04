#include "reconstruction/span_builder.h"

namespace reconstruction {

void SpanBuilder::on_event(const trace::Event& ev) {

    switch (ev.type) {

        case trace::EventType::SPAN_START: {
            auto* node = new SpanNode();

            node->span_id = ev.span_id;
            node->parent_id = ev.parent_span_id;
            node->name = !ev.message.empty() ? ev.message : "";
            node->start_ts = ev.timestamp;
            node->thread_id = ev.thread_id;

            spans[ev.span_id] = node;
            break;
        }

        case trace::EventType::SPAN_END: {
            auto it = spans.find(ev.span_id);
            if (it != spans.end()) {
                it->second->end_ts = ev.timestamp;
            }
            break;
        }

        case trace::EventType::LOG: {
            auto it = spans.find(ev.span_id);
            if (it != spans.end()) {
                LogEvent log;

                log.ts = ev.timestamp;
                log.level = ev.level;
                log.msg = !ev.message.empty() ? ev.message : "";

                log.file = !ev.file.empty() ? ev.file : "";
                log.func = !ev.func.empty() ? ev.func : "";
                log.line = ev.line;

                it->second->logs.push_back(std::move(log));
            }
            break;
        }

        default:
            break;
    }
}

std::vector<SpanNode*> SpanBuilder::build() {

    for (auto& [id, node] : spans) {

        if (node->parent_id != 0) {
            auto it = spans.find(node->parent_id);

            if (it != spans.end()) {
                it->second->children.push_back(node);
            } else {
                roots.push_back(node); // missing parent
            }
        } else {
            roots.push_back(node);
        }

        if (node->end_ts == 0) {
            node->end_ts = node->start_ts; // incomplete span
        }
    }

    return roots;
}

} // namespace reconstruction