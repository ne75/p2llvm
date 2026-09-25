// Real serial open/close callbacks and P2 locks; UART/cog/heap operations mocked.
#include <stdio.h>
#include <errno.h>
#include "../../libc/drivers/FdSerial.h"
#include "observe.h"

extern _Driver _SimpleSerialDriver, _FullDuplexSerialDriver;
_Driver *_driverlist[] = {&_SimpleSerialDriver, &_FullDuplexSerialDriver, NULL};
static _thread_state_t tls;
_thread_state_t *_TLS = &tls;
void _InitIO(void) {}
static FdSerial_t storage;
static unsigned bad, uart_calls, malloc_calls, free_calls, cog_calls, stop_calls;
// Keep failure controls as full-width loads; P2 currently cannot select some
// optimized i1 global loads (tracked in the lock-ownership audit).
static volatile int malloc_fails, allocated, next_cog = -1;
static unsigned last_stop;
void stdio_test_uart_init(unsigned rx, unsigned tx, unsigned baud, unsigned mode) {
    ++uart_calls;
}
void *stdio_test_malloc(size_t size) {
    ++malloc_calls;
    bad += allocated || size != sizeof(storage);
    if (malloc_fails) return NULL;
    allocated = 1;
    return &storage;
}
void stdio_test_free(void *ptr) {
    ++free_calls;
    bad += !allocated || ptr != &storage;
    allocated = 0;
}
int stdio_test_coginit(unsigned mode, void (*func)(void *), void *arg) {
    ++cog_calls;
    return next_cog;
}
void stdio_test_cogstop(unsigned id) { ++stop_calls; last_stop = id; }
// Test names contain no UART parameters, and tests transmit no stream data.
int atoi(const char *text) { ++bad; return 0; }
int _term_read(FILE *fp, unsigned char *buf, int size) { return 0; }
int _term_write(FILE *fp, unsigned char *buf, int size) { return size; }
static unsigned pool_count(void) {
    unsigned ids[16], count = 0, mask = 0;
    while (count < 16) {
        unsigned id = _locknew();
        if (id >= 16) { bad += id != ~0u; break; }
        if (mask & (1u << id)) { ++bad; break; }
        mask |= 1u << id;
        ids[count++] = id;
    }
    for (unsigned i = 0; i < count; ++i) _lockret(ids[i]);
    return count;
}
static unsigned shared_rounds, cog_bad;
static void shared_device(unsigned id, unsigned baseline) {
    FILE *fp[3];
    unsigned starts = cog_calls, stops = stop_calls, frees = free_calls;
    next_cog = id;
    for (unsigned i = 0; i < 3; ++i) {
        fp[i] = fopen("FDS:", "w");
        if (!fp[i]) {
            ++cog_bad;
            while (i) fclose(fp[--i]);
            return;
        }
    }
    cog_bad += cog_calls != starts + 1 || storage.cogId != (int)id + 1;
    for (unsigned i = 0; i < 8; ++i) {
        for (unsigned j = 0; j < 3; ++j)
            cog_bad += freopen("FDS:", "w", fp[j]) != fp[j];
        cog_bad += storage.users != 3 || pool_count() != baseline - 3;
        cog_bad += stop_calls != stops || cog_calls != starts + 1;
        ++shared_rounds;
    }
    for (unsigned i = 0; i < 3; ++i) {
        cog_bad += fclose(fp[i]) != 0;
        if (i < 2) cog_bad += stop_calls != stops || !allocated;
    }
    cog_bad += stop_calls != stops + 1 || last_stop != id;
    cog_bad += allocated || free_calls != frees + 1 || pool_count() != baseline;
}
void test_body(void) {
    unsigned baseline = pool_count(), ids[16], count = 0;
    observe("serial.baseline", baseline);
    while (count < 16) {
        unsigned id = _locknew();
        if (id >= 16) { bad += id != ~0u; break; }
        ids[count++] = id;
    }
    bad += fopen("SSER:", "w") != NULL || errno != ENOLCK;
    bad += fopen("FDS:", "w") != NULL || errno != ENOLCK;
    observe("serial.exhaustion_side_effects", uart_calls + malloc_calls + cog_calls);
    for (unsigned i = 0; i < count; ++i) _lockret(ids[i]);

    for (unsigned failure = 0; failure < 2; ++failure) {
        malloc_fails = failure == 0;
        for (unsigned i = 0; i < 20; ++i) {
            bad += fopen("FDS:", "w") != NULL;
            bad += errno != (malloc_fails ? ENOMEM : EAGAIN);
            bad += stdin->_lock != -1 || stdin->_drv != NULL;
            bad += allocated || pool_count() != baseline;
        }
    }
    observe("serial.failed_cog_starts", cog_calls);
    observe("serial.failed_open_frees", free_calls);
    observe("serial.unexpected_stops", stop_calls);
    FILE *fp = fopen("SSER:", "w");
    if (!fp) ++bad;
    else {
        for (unsigned i = 0; i < 32; ++i) {
            bad += freopen("SSER:", "w", fp) != fp;
            bad += pool_count() != baseline - 1;
        }
        bad += fclose(fp) != 0 || fp->_lock != -1;
    }
    shared_device(0, baseline);
    shared_device(3, baseline);
    shared_device(7, baseline);
    observe("serial.shared_rounds", shared_rounds);
    observe("serial.shared_stops", stop_calls);
    observe("serial.cog_bad", cog_bad);
    observe("serial.final", pool_count());
    observe("serial.bad", bad);
}
