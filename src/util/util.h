#ifndef SLATEWM_UTIL_H
#define SLATEWM_UTIL_H

#include <X11/Xlib.h>

namespace Util {
    int InterruptibleXNextEvent(Display *d, XEvent *event);
}

#endif
