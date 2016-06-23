#ifndef SLATEWM_EVENT_HANDLER_H
#define SLATEWM_EVENT_HANDLER_H

#define CLOSE_FOCUSED_CLIENT 0


#include "slate.h"
#include <unordered_map>

namespace ClientHandler {
    void Run(Slate* wm);
};


#endif
