/*
    Copyright (c) 2012-2013 Martin Sustrik  All rights reserved.
    Copyright 2015 Garrett D'Amore <garrett@damore.org>

    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files (the "Software"),
    to deal in the Software without restriction, including without limitation
    the rights to use, copy, modify, merge, publish, distribute, sublicense,
    and/or sell copies of the Software, and to permit persons to whom
    the Software is furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included
    in all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
    THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
    FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
    IN THE SOFTWARE.
*/

#include "efd.h"

#if defined GRID_HAVE_EVENTFD
#include "efd_eventfd.inc"
#elif defined GRID_HAVE_PIPE
#include "efd_pipe.inc"
#elif defined GRID_HAVE_SOCKETPAIR
#include "efd_socketpair.inc"
#else
#error
#endif

#if defined GRID_HAVE_POLL

#include <poll.h>

int grid_efd_wait (struct grid_efd *self, int timeout)
{
    int rc;
    struct pollfd pfd;

    pfd.fd = grid_efd_getfd (self);
    pfd.events = POLLIN;
    if (grid_slow (pfd.fd < 0))
        return -EBADF;
    rc = poll (&pfd, 1, timeout);
    if (grid_slow (rc < 0 && errno == EINTR))
        return -EINTR;
    errno_assert (rc >= 0);
    if (grid_slow (rc == 0))
        return -ETIMEDOUT;
    return 0;
}

#else
#error
#endif
