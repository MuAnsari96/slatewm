#include "message.h"

void Message::PopulateMessage(json *j, const slate_state_t &state) {
    (*j)["Event"] = "";
    (*j)["Client"] = 0;
    (*j)["Workspace"] = 0;
    (*j)["Screen"] = 0;
    (*j)["Keys"] = {
            {"Meta", state.meta},
            {"Shift", state.shift},
            {"Ctl", state.ctl},
            {"Alt", state.alt},
            {"Keymask", state.keymask},
            {"Delta", ""}
    };
}