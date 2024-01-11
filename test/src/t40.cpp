#include <stdlib.h>
#include <stdio.h>
#include <string.h>

char *test_str = 
"node_name_s:pdu31\n"
"ip_s:172.16.1.25\n"
"src_port_u16:ea60\n"
"ch0.name_s:wigwag_light\n"
"ch0.limit_f32:3dcccccd\n"
"ch0.persistence_u32:00000000\n"
"ch0.default_on_b:0\n"
"ch1.name_s:landing_light\n"
"ch1.limit_f32:41200000\n"
"ch1.persistence_u32:0000000a\n"
"ch1.default_on_b:0\n"
"ch2.name_s:taxi_light\n"
"ch2.limit_f32:41200000\n"
"ch2.persistence_u32:0000000a\n"
"ch2.default_on_b:0\n"
"ch3.name_s:strobe_light\n"
"ch3.limit_f32:40c00000\n"
"ch3.persistence_u32:00000064\n"
"ch3.default_on_b:0\n"
"ch4.name_s:nav_light\n"
"ch4.limit_f32:40a00000\n"
"ch4.persistence_u32:0000000a\n"
"ch4.default_on_b:0\n"
"ch5.name_s:comm_radio\n"
"ch5.limit_u64:4080000040800000\n"
"ch5.persistence_u64:0000006400000064\n"
"ch5.default_on_b:0\n"
"ch6.name_s:adsb_xpdr\n"
"ch6.limit_u64:3f8000003f800000\n"
"ch6.persistence_u64:0000000a0000000a\n"
"ch6.default_on_b:0\n"
"ch7.name_s:fc1_logic\n"
"ch7.limit_u64:4080000040800000\n"
"ch7.persistence_u64:0000000a0000000a\n"
"ch7.default_on_b:1\n"
"ch8.name_s:trim_ctrl\n"
"ch8.limit_u64:4000000040000000\n"
"ch8.persistence_u64:0000000a0000000a\n"
"ch8.default_on_b:0\n"
"ch9.name_s:nws2\n"
"ch9.limit_u64:3f8000003f800000\n"
"ch9.persistence_u64:0000000a0000000a\n"
"ch9.default_on_b:1\n";

char test_dst[1100] = {0};

__attribute__((noinline)) int test_cpy(char *d, char *s, int l) {
    int t = CNT;
    memcpy(d, s, l);
    t = CNT - t;

    return t;
}

int main(void) {
    // fast memory op tests

    auto len = strlen(test_str) + 1;

    printf("length of test: %d\n", len);

    int t = test_cpy(test_dst, test_str, len);

    for (int i = 0; i < 1062; i++) {
        if (test_str[i] != test_dst[i]) {
            printf("mismatch at %d: ", i);
            printf("%x ?= %x\n", test_str[i], test_dst[i]);
        }
    }

    printf("%d\n", t);

    busywait();
}