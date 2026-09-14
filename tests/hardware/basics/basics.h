#pragma once
#include <stddef.h>
#include <stdint.h>
typedef uint32_t u32;
typedef uint64_t u64;
#define BASIC_CASES(X) X(empty) X(alu) X(div32) X(mul64) X(div64) X(single) X(minimum) X(copy64) X(copy1024) X(copy_unaligned) X(fill8) X(fill64) X(fill1024) X(aggregate) X(virtual) X(stack)
#define CASE_ID(name) CASE_##name,
enum { BASIC_CASES(CASE_ID) };
#undef CASE_ID
#ifdef __cplusplus
extern "C" {
#endif
#define DECLARE(name) u32 basic_##name(u32);
BASIC_CASES(DECLARE)
#undef DECLARE
void basic_prepare(void);
u32 basic_check_memory(unsigned id, u32 seed);
#ifdef __cplusplus
}
#endif
