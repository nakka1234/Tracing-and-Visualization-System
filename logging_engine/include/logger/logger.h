#pragma once
#include "trace/event.h"

namespace logger {

// lifecycle
void init();
void shutdown();

// main API
void push(const trace::Event& ev);

}