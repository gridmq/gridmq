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

#ifndef GRID_FSM_INCLUDED
#define GRID_FSM_INCLUDED

#include "../utils/queue.h"

/*  Base class for state machines. */

struct grid_ctx;
struct grid_fsm;
struct grid_worker;

struct grid_fsm_event {
    struct grid_fsm *fsm;
    int src;
    void *srcptr;
    int type;
    struct grid_queue_item item;
};

void grid_fsm_event_init (struct grid_fsm_event *self);
void grid_fsm_event_term (struct grid_fsm_event *self);
int grid_fsm_event_active (struct grid_fsm_event *self);
void grid_fsm_event_process (struct grid_fsm_event *self);

/*  Special source for actions. It's negative not to clash with user-defined
    sources. */
#define GRID_FSM_ACTION -2

/*  Actions generated by fsm object. The values are negative not to clash
    with user-defined actions. */
#define GRID_FSM_START -2
#define GRID_FSM_STOP -3

/*  Virtual function to be implemented by the derived class to handle the
    incoming events. */
typedef void (*grid_fsm_fn) (struct grid_fsm *self, int src, int type,
    void *srcptr);

struct grid_fsm_owner {
    int src;
    struct grid_fsm *fsm;
};

struct grid_fsm {
    grid_fsm_fn fn;
    grid_fsm_fn shutdown_fn;
    int state;
    int src;
    void *srcptr;
    struct grid_fsm *owner;
    struct grid_ctx *ctx;
    struct grid_fsm_event stopped;
};

void grid_fsm_init_root (struct grid_fsm *self, grid_fsm_fn fn,
    grid_fsm_fn shutdown_fn, struct grid_ctx *ctx);
void grid_fsm_init (struct grid_fsm *self, grid_fsm_fn fn,
    grid_fsm_fn shutdown_fn,
    int src, void *srcptr, struct grid_fsm *owner);
void grid_fsm_term (struct grid_fsm *self);

int grid_fsm_isidle (struct grid_fsm *self);
void grid_fsm_start (struct grid_fsm *self);
void grid_fsm_stop (struct grid_fsm *self);
void grid_fsm_stopped (struct grid_fsm *self, int type);
void grid_fsm_stopped_noevent (struct grid_fsm *self);

/*  Replaces current owner of the fsm by the owner speicified by 'owner'
    parameter. The parameter will hold the old owner afrer the call. */
void grid_fsm_swap_owner (struct grid_fsm *self, struct grid_fsm_owner *owner);

struct grid_worker *grid_fsm_choose_worker (struct grid_fsm *self);

/*  Using this function state machine can trigger an action on itself. */
void grid_fsm_action (struct grid_fsm *self, int type);

/*  Send event from the state machine to its owner. */
void grid_fsm_raise (struct grid_fsm *self, struct grid_fsm_event *event, int type);


/*  Send event to the specified state machine. It's caller's responsibility
    to ensure that the destination state machine will still exist when the
    event is delivered.
    NOTE: This function is a hack to make inproc transport work in the most
    efficient manner. Do not use it outside of inproc transport! */
void grid_fsm_raiseto (struct grid_fsm *self, struct grid_fsm *dst,
    struct grid_fsm_event *event, int src, int type, void *srcptr);

/*  This function is very lowlevel action feeding
    Used in worker threads and timers, shouldn't be used by others
    use grid_fsm_action/grid_fsm_raise/grid_fsm_raiseto instread*/
void grid_fsm_feed (struct grid_fsm *self, int src, int type, void *srcptr);

#endif

