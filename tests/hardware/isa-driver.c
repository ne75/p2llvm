#include "observe.h"
extern unsigned asm_address_add(void), asm_address_sub(void), asm_both_fields(void);
extern unsigned asm_return_aug(void), asm_negative_branch(void), asm_setnib(void), asm_testb_wz(void);
extern unsigned asm_call_sqrt(void), asm_call_lut(void);
extern unsigned probe_delta32;
extern unsigned long long probe_delta64;
void test_body(void) {
    observe("reloc.add", asm_address_add());
    observe("reloc.subtract", asm_address_sub());
    observe("reloc.both_fields", asm_both_fields());
    observe("return.aug", asm_return_aug());
    observe("branch.negative", asm_negative_branch());
    observe("setnib.aug", asm_setnib());
    observe("testb.wz", asm_testb_wz());
    observe("call.hub_name", asm_call_sqrt());
    observe("call.lut_section", asm_call_lut());
    observe("data.delta32", probe_delta32);
    OBSERVE64("data.delta64", probe_delta64);
}
