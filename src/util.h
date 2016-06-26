#ifndef SLATEWM_UTIL_H
#define SLATEWM_UTIL_H

#include <X11/Xlib.h>

int InterruptibleXNextEvent(Display*d, XEvent *event);

#endif
