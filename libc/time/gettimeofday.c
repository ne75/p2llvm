#include <stdint.h>
#include <sys/time.h>
#include <propeller2.h>

// official time in "seconds" since the Unix Epoch
// this actually leaves out leap seconds, which is a big can of worms
// we're ignoring for now
static unsigned int lastsec = 1000000000; // Sept. 9, 2001

// microsecond counter reading at last second
static unsigned int lastus = 0;

int gettimeofday(struct timeval *tv, void *unused)
{
    uint32_t now = getus();  // current microsecond count
    uint32_t then = lastus;
    uint32_t elapsed = now - then;
    uint32_t secs, leftover;

    secs = elapsed / 1000000;
    leftover = elapsed % 1000000;
    if (secs) {
        lastsec += secs;
        lastus = now - leftover;
    }
    tv->tv_sec = lastsec;
    tv->tv_usec = leftover;
    return 0;
}

int
settimeofday(const struct timeval *tv, const void *unused)
{
    unsigned int now = getus();
    lastsec = tv->tv_sec;
    lastus = now - tv->tv_usec;
    return 0;
}
