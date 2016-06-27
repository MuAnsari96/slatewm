#ifndef SLATEWM_EVENT_HANDLER_H
#define SLATEWM_EVENT_HANDLER_H

#define Kill_FOCUSED_CLIENT 0
#define HIDE_FOCUSED_CLIENT 1
#define UNHIDE_FOCUSED_CLIENT 2
#define SWITCH_WORKSPACE 3

#include "slate.h"
#include <unordered_map>

namespace ClientHandler {
    void Run(Slate* wm);
};


#endif
