#include <X11/Xlib.h>
#include "message.h"

void Message::PopulateMessage(json *j, const slate_state_t &state, const XEvent &e) {
    (*j)["Event"] = "Default";
    (*j)["Client"] = state.focused_client;
    (*j)["Workspace"] = 0;
    (*j)["Screen"] = 0;
    (*j)["Keys"] = {
            {"Meta", state.meta},
            {"Shift", state.shift},
            {"Ctl", state.ctl},
            {"Alt", state.alt},
            {"Space", state.space},
            {"Enter", state.enter},
            {"Keymask", state.keymask},
            {"Delta", ""}
    };
}