// Exercise the real stdio lifecycle with real P2 locks and a pin-free driver.
#include <stdio.h>
#include <errno.h>
#include "observe.h"

static _thread_state_t tls;
_thread_state_t *_TLS = &tls;
void _InitIO(void) {} // Explicitly open the three test streams below.
static unsigned pool_bad, callback_bad, writes, closes;

// Allocate only free locks; never return a lock owned by startup or a stream.
static unsigned free_mask(void) {
    unsigned ids[16], count = 0, mask = 0;
    for (; count < 16; ++count) {
        unsigned id = _locknew();
        if (id >= 16) { pool_bad += id != ~0u; break; }
        if (mask & (1u << id)) { ++pool_bad; break; }
        ids[count] = id;
        mask |= 1u << id;
    }
    while (count) _lockret(ids[--count]);
    return mask;
}
// Probe from another cog: LOCKTRY by the owner is reentrant, and LOCKREL
// by the owner releases the lock before reporting its state.
#ifndef P2_TEST_HOST
static struct {
    unsigned *ids;
    volatile unsigned count, bad, done;
} probe;
static int probe_stack[128];
static void probe_locks(void *unused) {
    for (unsigned i = 0; i < probe.count; ++i)
        if (_locktry(probe.ids[i])) {
            ++probe.bad;
            _unlock(probe.ids[i]);
        }
    probe.done = 1;
    for (;;) {} // The parent stops this cog after reading the result.
}
#endif
static unsigned unheld_locks(unsigned *ids, unsigned count) {
#ifdef P2_TEST_HOST
    extern unsigned stdio_test_is_held(unsigned);
    unsigned bad = 0;
    for (unsigned i = 0; i < count; ++i) bad += !stdio_test_is_held(ids[i]);
    return bad;
#else
    probe.ids = ids;
    probe.count = count;
    probe.bad = probe.done = 0;
    int cog = cogstart(probe_locks, 0, probe_stack, sizeof(probe_stack));
    if (cog < 0) return 1;
    while (!probe.done) {}
    cogstop(cog);
    return probe.bad;
#endif
}
static unsigned count_bits(unsigned mask) {
    unsigned count = 0;
    for (; mask; mask >>= 1) count += mask & 1;
    return count;
}
static int test_open(FILE *fp, const char *name, const char *mode) {
    callback_bad += fp->_lock != -1;
    fp->drvarg[0] = name[0];
    if (name[0] == 'n') return 0; // A driver that never allocates a lock.
    if (name[0] != 'b') {
        fp->_lock = _locknew();
        if ((unsigned)fp->_lock >= 16) { errno = ENOLCK; return -1; }
    }
    if (name[0] == 'a' || name[0] == 'b') {
        errno = EIO; // Fail after/before allocation, respectively.
        return -1;
    }
    return 0;
}
static void check_owned(FILE *fp) {
    if ((unsigned)fp->_lock < 16)
        callback_bad += (free_mask() & (1u << fp->_lock)) != 0;
}
static int test_write(FILE *fp, unsigned char *buf, int size) {
    ++writes;
    check_owned(fp);
    if (fp->drvarg[0] == 'w') { errno = EIO; return 0; }
    return size;
}
static int test_close(FILE *fp) {
    ++closes;
    check_owned(fp);
    callback_bad += fp->_cnt != 0; // Flush must precede driver teardown.
    if (fp->drvarg[0] == 'c') { errno = EIO; return -1; }
    return 0;
}
static _Driver driver = {
    .prefix = "LOCK:", .fopen = test_open,
    .fclose = test_close, .write = test_write
};
_Driver *_driverlist[] = { &driver, NULL };
static void buffer_byte(FILE *fp) {
    fp->_base[0] = 'x';
    fp->_ptr = fp->_base + 1;
    fp->_cnt = 1;
}

void test_body(void) {
    unsigned baseline = free_mask(), lifecycle_bad = 0, failure_bad = 0;
    unsigned invalid_bad = 0, no_lock_bad = 0, rounds = 0;
    FILE *streams[] = {stdin, stdout, stderr};
    observe("stream.baseline", count_bits(baseline));
    for (unsigned i = 0; i < 3; ++i)
        lifecycle_bad += __fopen_driver(streams[i], &driver, "ok", "w") == NULL;
    observe("stream.open", count_bits(free_mask()));
    for (unsigned round = 0; round < 32; ++round) {
        for (unsigned i = 0; i < 3; ++i) {
            if (streams[i]->_drv) buffer_byte(streams[i]);
            lifecycle_bad += freopen("LOCK:ok", "w", streams[i]) != streams[i];
        }
        lifecycle_bad += count_bits(free_mask()) != count_bits(baseline) - 3;
        ++rounds;
    }
    observe("stream.reopened", count_bits(free_mask()));
    for (unsigned i = 0; i < 3; ++i) {
        lifecycle_bad += fclose(streams[i]) != 0;
        lifecycle_bad += streams[i]->_lock != -1;
    }
    observe("stream.closed", count_bits(free_mask()));
    observe("stream.rounds", rounds);
    observe("stream.writes", writes);
    observe("stream.closes", closes);

    // Failed opens must release a reserved FILE slot and any acquired lock.
    const char *failures[] = {"LOCK:before", "LOCK:after"};
    for (unsigned round = 0; round < 20; ++round) {
        for (unsigned i = 0; i < 2; ++i) {
            failure_bad += fopen(failures[i], "w") != NULL || errno != EIO;
            failure_bad += free_mask() != baseline || stdin->_lock != -1;
            failure_bad += stdin->_drv != NULL || stdin->_flag != 0;
        }
    }
    failure_bad += fopen("LOCK:ok", "?") != NULL || errno != EINVAL;
    failure_bad += free_mask() != baseline;
    for (unsigned i = 0; i < 2; ++i) {
        FILE *fp = fopen(i ? "LOCK:write-error" : "LOCK:close-error", "w");
        if (!fp) { ++failure_bad; continue; }
        buffer_byte(fp);
        failure_bad += fclose(fp) != EOF || errno != EIO;
        failure_bad += fp->_lock != -1 || free_mask() != baseline;
    }
    FILE *fp = fopen("LOCK:ok", "w");
    failure_bad += !fp;
    if (fp) failure_bad += freopen("missing:", "w", fp) != NULL || errno != ENOENT;
    failure_bad += free_mask() != baseline;

    // Hold every free lock while checking invalid and lock-free streams.
    unsigned ids[16], count = 0, exhausted = 0;
    while (count < 16) {
        unsigned id = _locknew();
        if (id >= 16) { exhausted = id; break; }
        ids[count++] = id;
        _lock(id);
    }
    failure_bad += fopen("LOCK:ok", "w") != NULL || errno != ENOLCK;
    invalid_bad += fclose(NULL) != EOF || errno != EBADF;
    FILE invalid = {0};
    invalid._lock = count ? ids[0] : -1;
    invalid_bad += fclose(&invalid) != EOF || errno != EBADF;
    invalid._drv = &driver; // A driver alone does not make a stream open.
    invalid_bad += fclose(&invalid) != EOF || errno != EBADF;
    fp = fopen("LOCK:no-lock", "w");
    if (fp) {
        no_lock_bad += fp->_lock != -1;
        no_lock_bad += fclose(fp) != 0 || fp->_lock != -1;
        invalid_bad += fclose(fp) != EOF || errno != EBADF;
    } else ++no_lock_bad;
    char buf[8] = {'x'};
    fp = fmemopen(buf, sizeof(buf), "w");
    if (fp) {
        no_lock_bad += fp->_lock != -1;
        no_lock_bad += fclose(fp) != 0 || buf[0] != 0;
    } else ++no_lock_bad;
    // The invalid sentinel must not alias a real lock during unlock/close.
    _unlock(~0u);
    invalid_bad += free_mask() != 0;
    no_lock_bad += unheld_locks(ids, count);
    for (unsigned i = 0; i < count; ++i) {
        _unlock(ids[i]);
        _lockret(ids[i]);
    }
    observe("stream.exhausted", exhausted);
    observe("stream.lifecycle_bad", lifecycle_bad);
    observe("stream.failure_bad", failure_bad);
    observe("stream.invalid_bad", invalid_bad);
    observe("stream.no_lock_bad", no_lock_bad);
    observe("stream.callback_bad", callback_bad);
    observe("stream.pool_bad", pool_bad);
    observe("stream.final", count_bits(free_mask()));
}
