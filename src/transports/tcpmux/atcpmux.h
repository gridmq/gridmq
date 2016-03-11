/*
    Copyright (c) 2013-2014 Martin Sustrik  All rights reserved.

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

#ifndef GRID_ATCPMUX_INCLUDED
#define GRID_ATCPMUX_INCLUDED

#include "stcpmux.h"

#include "../../transport.h"

#include "../../aio/fsm.h"
#include "../../aio/usock.h"

#include "../../utils/list.h"

/*  State machine handling accepted TCPMUX sockets. */

/*  In btcpmux, some events are just *assumed* to come from a child atcpmux
    object. By using non-trivial event codes, we can do more reliable sanity
    checking in such scenarios. */
#define GRID_ATCPMUX_ACCEPTED 34231
#define GRID_ATCPMUX_ERROR 34232
#define GRID_ATCPMUX_STOPPED 34233

struct grid_atcpmux {

    /*  The state machine. */
    struct grid_fsm fsm;
    int state;

    /*  Pointer to the associated endpoint. */
    struct grid_epbase *epbase;

    /*  Underlying socket. */
    struct grid_usock usock;

    /*  State machine that takes care of the connection in the active state. */
    struct grid_stcpmux stcpmux;

    /*  Events generated by atcpmux state machine. */
    struct grid_fsm_event accepted;
    struct grid_fsm_event done;

    /*  This member can be used by owner to keep individual atcpmuxes
        in a list. */
    struct grid_list_item item;
};

void grid_atcpmux_init (struct grid_atcpmux *self, int src,
    struct grid_epbase *epbase, struct grid_fsm *owner);
void grid_atcpmux_term (struct grid_atcpmux *self);

int grid_atcpmux_isidle (struct grid_atcpmux *self);
void grid_atcpmux_start (struct grid_atcpmux *self, int fd);
void grid_atcpmux_stop (struct grid_atcpmux *self);

#endif
