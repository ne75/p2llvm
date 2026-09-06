#ifndef P2_TEST_OBSERVE_H
#define P2_TEST_OBSERVE_H
#ifdef __cplusplus
extern "C" {
#endif
void observe(const char *name, unsigned value);
void test_body(void);
#ifdef __cplusplus
}
#endif
#define OBSERVE64(name, value) do { \
    unsigned long long v_ = (value); \
    observe(name ".lo", (unsigned)v_); \
    observe(name ".hi", (unsigned)(v_ >> 32)); \
} while (0)
#endif
