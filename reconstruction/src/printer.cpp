#include <iostream>
#include "reconstruction/printer.h"

namespace reconstruction {

void print_span(const SpanNode* node, int depth) {

    for (int i = 0; i < depth; ++i)
        std::cout << "  ";

    std::cout << node->name
              << " (" << (node->end_ts - node->start_ts) << "us, thread " << node->thread_id << ")"
              << "\n";

    for (const auto& log : node->logs) {
        for (int i = 0; i < depth + 1; ++i)
            std::cout << "  ";

        std::cout << "[LOG] " << log.msg;
        if (!log.file.empty() || !log.func.empty() || log.line != 0) {
            std::cout << " at ";
            if (!log.file.empty()) std::cout << log.file << ":";
            if (!log.func.empty()) std::cout << log.func << ":";
            if (log.line != 0) std::cout << log.line;
        }
        std::cout << "\n";
    }

    for (auto* child : node->children) {
        print_span(child, depth + 1);
    }
}

}