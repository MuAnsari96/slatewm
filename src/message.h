#ifndef SLATEWM_MESSAGE_H
#define SLATEWM_MESSAGE_H
#include "json.h"
#include "slate.h"
#include <X11/Xlib.h>

using json = nlohmann::json;

namespace Message {
    void PopulateMessage(json *j, const slate_state_t &state, const XEvent &e);


}
#endif //SLATEWM_MESSAGE_H
