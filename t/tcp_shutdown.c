/*
    Copyright (c) 2012 Martin Sustrik  All rights reserved.

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

#include "../src/grid.h"
#include "../src/pair.h"
#include "../src/pubsub.h"
#include "../src/pipeline.h"
#include "../src/tcp.h"

#include "testutil.h"
#include "../src/utils/attr.h"
#include "../src/utils/thread.c"
#include "../src/utils/atomic.c"

/*  Stress test the TCP transport. */

#define THREAD_COUNT 100
#define TEST2_THREAD_COUNT 10
#define MESSAGES_PER_THREAD 10
#define TEST_LOOPS 10
#define SOCKET_ADDRESS "tcp://127.0.0.1:5557"

struct grid_atomic active;

static void routine (GRID_UNUSED void *arg)
{
    int s;

    s = grid_socket (AF_SP, GRID_SUB);
    if (s < 0 && grid_errno () == EMFILE)
        return;
    errno_assert (s >= 0);
    test_connect (s, SOCKET_ADDRESS);
    test_close (s);
}

static void routine2 (GRID_UNUSED void *arg)
{
    int s;
    int i;
    int ms;

    s = test_socket (AF_SP, GRID_PULL);

    for (i = 0; i < 10; ++i) {
        test_connect (s, SOCKET_ADDRESS);
    }

    ms = 2000;
    test_setsockopt (s, GRID_SOL_SOCKET, GRID_RCVTIMEO, &ms, sizeof (ms));

    for (i = 0; i < MESSAGES_PER_THREAD; ++i) {
        test_recv (s, "hello");
    }

    test_close (s);
    grid_atomic_dec(&active, 1);
}

int main ()
{
    int sb;
    int i;
    int j;
    struct grid_thread threads [THREAD_COUNT];

    /*  Stress the shutdown algorithm. */

#if defined(SIGPIPE) && defined(SIG_IGN)
    signal (SIGPIPE, SIG_IGN);
#endif

    sb = test_socket (AF_SP, GRID_PUB);
    test_bind (sb, SOCKET_ADDRESS);

    for (j = 0; j != TEST_LOOPS; ++j) {
        for (i = 0; i != THREAD_COUNT; ++i)
            grid_thread_init (&threads [i], routine, NULL);
        for (i = 0; i != THREAD_COUNT; ++i) {
            grid_thread_term (&threads [i]);
	}
    }

    test_close (sb);

    /*  Test race condition of sending message while socket shutting down  */

    sb = test_socket (AF_SP, GRID_PUSH);
    test_bind (sb, SOCKET_ADDRESS);

    for (j = 0; j != TEST_LOOPS; ++j) {
	int ms;
        for (i = 0; i != TEST2_THREAD_COUNT; ++i)
            grid_thread_init (&threads [i], routine2, &threads[i]);
        grid_atomic_init(&active, TEST2_THREAD_COUNT);

	ms = 2000;
	test_setsockopt (sb, GRID_SOL_SOCKET, GRID_SNDTIMEO, &ms, sizeof (ms));
        while (active.n) {
            (void) grid_send (sb, "hello", 5, GRID_DONTWAIT);
        }

        for (i = 0; i != TEST2_THREAD_COUNT; ++i)
            grid_thread_term (&threads [i]);
        grid_atomic_term(&active);
    }

    test_close (sb);

    return 0;
}
