#include "reconstruction/span_builder.h"
#include "reconstruction/printer.h"
#include "trace/event.h"

#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <cstdint>
#include <iostream>
#include <iomanip>

using namespace reconstruction;

static std::string escape_json(const std::string& value) {
    std::string escaped;
    escaped.reserve(value.size());
    for (char c : value) {
        switch (c) {
            case '\\': escaped += "\\\\"; break;
            case '"': escaped += "\\\""; break;
            case '\b': escaped += "\\b"; break;
            case '\f': escaped += "\\f"; break;
            case '\n': escaped += "\\n"; break;
            case '\r': escaped += "\\r"; break;
            case '\t': escaped += "\\t"; break;
            default:
                if (static_cast<unsigned char>(c) < 0x20) {
                    std::ostringstream oss;
                    oss << "\\u" << std::hex << std::setw(4) << std::setfill('0') << (int)c;
                    escaped += oss.str();
                } else {
                    escaped += c;
                }
        }
    }
    return escaped;
}

static void trim(std::string& str) {
    const char* ws = " \t\n\r";
    size_t start = str.find_first_not_of(ws);
    size_t end = str.find_last_not_of(ws);
    if (start == std::string::npos) {
        str.clear();
    } else {
        str = str.substr(start, end - start + 1);
    }
}

static std::string unquote(const std::string& value) {
    if (value.size() >= 2 && value.front() == '"' && value.back() == '"')
        return value.substr(1, value.size() - 2);
    return value;
}

static trace::Event parse_event(const std::string& json) {
    trace::Event ev = {};
    ev.type = trace::EventType::LOG;

    std::string s = json;
    if (s.size() >= 2 && s.front() == '{' && s.back() == '}') {
        s = s.substr(1, s.size() - 2);
    }

    std::stringstream ss(s);
    std::string token;
    while (std::getline(ss, token, ',')) {
        const size_t colon = token.find(':');
        if (colon == std::string::npos) continue;

        std::string key = token.substr(0, colon);
        std::string value = token.substr(colon + 1);
        trim(key);
        trim(value);

        if (key == "\"type\"") {
            if (value == "\"SPAN_START\"") ev.type = trace::EventType::SPAN_START;
            else if (value == "\"SPAN_END\"") ev.type = trace::EventType::SPAN_END;
        } else if (key == "\"span\"") ev.span_id = std::stoull(value);
        else if (key == "\"parent\"") ev.parent_span_id = std::stoull(value);
        else if (key == "\"ts\"") ev.timestamp = std::stoull(value);
        else if (key == "\"tid\"") ev.thread_id = std::stoull(value);
        else if (key == "\"pid\"") ev.process_id = std::stoul(value);
        else if (key == "\"msg\"") ev.message = unquote(value);
        else if (key == "\"level\"") ev.level = std::stoi(value);
        else if (key == "\"file\"") ev.file = unquote(value);
        else if (key == "\"func\"") ev.func = unquote(value);
        else if (key == "\"line\"") ev.line = std::stoi(value);
    }

    return ev;
}

static void write_node_json(std::ostream& os, const SpanNode* node, int indent) {
    const std::string pad(indent, ' ');
    os << pad << "{\n";
    os << pad << "  \"spanId\": " << node->span_id << ",\n";
    os << pad << "  \"parentId\": " << node->parent_id << ",\n";
    os << pad << "  \"name\": \"" << escape_json(node->name) << "\",\n";
    os << pad << "  \"startTs\": " << node->start_ts << ",\n";
    os << pad << "  \"endTs\": " << node->end_ts << ",\n";
    os << pad << "  \"durationUs\": " << (node->end_ts - node->start_ts) << ",\n";
    os << pad << "  \"threadId\": " << node->thread_id << ",\n";
    os << pad << "  \"logs\": [\n";

    for (size_t i = 0; i < node->logs.size(); ++i) {
        const auto& log = node->logs[i];
        os << pad << "    {\n";
        os << pad << "      \"ts\": " << log.ts << ",\n";
        os << pad << "      \"level\": " << log.level << ",\n";
        os << pad << "      \"msg\": \"" << escape_json(log.msg) << "\",\n";
        os << pad << "      \"file\": \"" << escape_json(log.file) << "\",\n";
        os << pad << "      \"func\": \"" << escape_json(log.func) << "\",\n";
        os << pad << "      \"line\": " << log.line << "\n";
        os << pad << "    }";
        if (i + 1 < node->logs.size()) os << ",";
        os << "\n";
    }

    os << pad << "  ],\n";
    os << pad << "  \"children\": [\n";

    for (size_t i = 0; i < node->children.size(); ++i) {
        write_node_json(os, node->children[i], indent + 4);
        if (i + 1 < node->children.size()) os << ",\n";
        else os << "\n";
    }

    os << pad << "  ]\n";
    os << pad << "}";
}

int main(int argc, char* argv[]) {
    // Default output to root project directory
    std::string output_path = "C:\\vs_projects\\tracing_system\\reconstruction.json";
    if (argc > 1) {
        output_path = argv[1];  // User can override with custom output path
    }

    std::vector<std::string> trace_paths = {
        "trace.log",
        "..\\..\\trace.log", 
        "..\\trace.log"
    };

    std::ifstream file;
    std::string trace_path_used;
    for (const auto& path : trace_paths) {
        file.open(path);
        if (file.is_open()) {
            trace_path_used = path;
            break;
        }
    }

    if (!file.is_open()) {
        std::cerr << "Failed to open trace.log. Tried paths:\n";
        for (const auto& path : trace_paths) {
            std::cerr << "  " << path << "\n";
        }
        return 1;
    }

    std::cout << "Reading trace data from: " << trace_path_used << "\n";

    SpanBuilder builder;
    std::string line;
    while (std::getline(file, line)) {
        if (!line.empty()) {
            trace::Event ev = parse_event(line);
            builder.on_event(ev);
        }
    }

    auto roots = builder.build();

    std::ofstream out(output_path);
    if (!out.is_open()) {
        std::cerr << "Failed to open output file " << output_path << "\n";
        return 1;
    }

    out << "{\n";
    out << "  \"roots\": [\n";
    for (size_t i = 0; i < roots.size(); ++i) {
        write_node_json(out, roots[i], 4);
        if (i + 1 < roots.size()) out << ",\n";
        else out << "\n";
    }
    out << "  ],\n";
    out << "  \"stats\": {\n";
    out << "    \"spanCount\": " << roots.size() << ",\n";
    out << "    \"rootCount\": " << roots.size() << "\n";
    out << "  }\n";
    out << "}\n";

    std::cout << "Wrote reconstructed JSON to " << output_path << "\n";
    return 0;
}
