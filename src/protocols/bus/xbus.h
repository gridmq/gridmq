/*
    Copyright (c) 2013 Martin Sustrik  All rights reserved.

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

#ifndef GRID_XBUS_INCLUDED
#define GRID_XBUS_INCLUDED

#include "../../protocol.h"

#include "../utils/dist.h"
#include "../utils/fq.h"

extern struct grid_socktype *grid_xbus_socktype;

struct grid_xbus_data {
    struct grid_dist_data outitem;
    struct grid_fq_data initem;
};

struct grid_xbus {
    struct grid_sockbase sockbase;
    struct grid_dist outpipes;
    struct grid_fq inpipes;
};

void grid_xbus_init (struct grid_xbus *self,
    const struct grid_sockbase_vfptr *vfptr, void *hint);
void grid_xbus_term (struct grid_xbus *self);

int grid_xbus_add (struct grid_sockbase *self, struct grid_pipe *pipe);
void grid_xbus_rm (struct grid_sockbase *self, struct grid_pipe *pipe);
void grid_xbus_in (struct grid_sockbase *self, struct grid_pipe *pipe);
void grid_xbus_out (struct grid_sockbase *self, struct grid_pipe *pipe);
int grid_xbus_events (struct grid_sockbase *self);
int grid_xbus_send (struct grid_sockbase *self, struct grid_msg *msg);
int grid_xbus_recv (struct grid_sockbase *self, struct grid_msg *msg);
int grid_xbus_setopt (struct grid_sockbase *self, int level, int option,
    const void *optval, size_t optvallen);
int grid_xbus_getopt (struct grid_sockbase *self, int level, int option,
    void *optval, size_t *optvallen);

int grid_xbus_ispeer (int socktype);

#endif
