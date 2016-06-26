#include "util.h"
#include <iostream>

/*
 * InterruptibleXNextEvent() is taken from EvilWM, where it was taken from the
 * Blender source and comes with the following copyright notice: */

/* Copyright (c) Mark J. Kilgard, 1994, 1995, 1996. */

/* This program is freely distributable without licensing fees
 * and is provided without guarantee or warrantee expressed or
 * implied. This program is -not- in the public domain. */

/* Unlike XNextEvent, if a signal arrives, InterruptibleXNextEvent will
 * return zero. */

int InterruptibleXNextEvent(Display* d, XEvent *event) {
	fd_set fds;
	int rc;
	int d_fd = ConnectionNumber(d);
	while (true) {
		if (XPending(d)) {
			XNextEvent(d, event);
			return 1;
		}
		FD_ZERO(&fds);
		FD_SET(d_fd, &fds);
		rc = select(d_fd + 1, &fds, NULL, NULL, NULL);
		if (rc < 0) {
			if (errno == EINTR) {
				return 0;
			} else {
				std::cout << "interruptibleXNextEvent(): select()\n" << std::endl;
			}
		}
	}
}