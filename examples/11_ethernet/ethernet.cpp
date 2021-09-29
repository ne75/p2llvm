/**
 *
 * Ethernet Driver Example/Dev project
 */
#include "enc424j600.h"

#include <propeller.h>
#include <sys/p2es_clock.h>
#include <stdio.h>

// #include "lwip/inet.h"
// #include "lwip/tcp.h"
// #include "lwip/ip4_frag.h"
// #include "lwip/netif.h"
// #include "lwip/init.h"
// #include "lwip/stats.h"
// #include "lwip/udp.h"
// #include "netif/etharp.h"

#define RX_PIN 63
#define TX_PIN 62

using namespace P2;

                //  ad0 al  rd  wr  in, clk
ENC424J600 ethernet(32, 47, 48, 49, 50, 51);

volatile uint8_t d[1500] = {0};

// void dump_packet(struct pbuf *p) {
//     for (int i = 0; i < p->len; i++) {
//         for (int j = 0; j < 8; j++) {
//             if (i*8 + j > p->len-1) {
//                 printf("\n");
//                 return;
//             }
//             printf("%02x ", ((uint8_t*)p->payload)[i*8 + j]);
//         }
//         printf("\n");
//     }
// }

// // NETWORK STUFF
// netif p2_netif;

// err_t p2_link_output(struct netif *netif, struct pbuf *p) {
//     printf("==== TODO: P2 link output ====\n");
//     printf("payload: \n");
//     dump_packet(p);
//     return ERR_OK;
// }

// static pbuf *p2_input_test(netif *netif) {

//     uint8_t sample_arp[] = {
//         0x00, 0x12, 0x34, 0x56, 0x78, 0x9a, // dest
//         0x00, 0x12, 0x34, 0x56, 0x78, 0x9b, // source
//         0x08, 0x06, //
//         0x00, 0x01,
//         0x08, 0x00,
//         0x06,
//         0x04,
//         0x00, 0x02,
//         0x00, 0x12, 0x34, 0x56, 0x78, 0x9b,
//         0xac, 0x10, 0x01, 0x02,
//         0x00, 0x12, 0x34, 0x56, 0x78, 0x9a,
//         0xac, 0x10, 0x01, 0x0a
//     };

//     uint8_t *head = sample_arp;

//     struct pbuf *p, *q;
//     u16_t len;

//     len = sizeof(sample_arp);

//     /* We allocate a pbuf chain of pbufs from the pool. */
//     p = pbuf_alloc(PBUF_RAW, len, PBUF_POOL);

//     if (p != NULL) {

//         /* We iterate over the pbuf chain until we have read the entire * packet into the pbuf. */
//         for (q = p; q != NULL; q = q->next) {
//              Read enough bytes to fill this pbuf in the chain. The
//              * available data in the pbuf is given by the q->len
//              * variable.
//              * This does not necessarily have to be a memcpy, you can also preallocate
//              * pbufs for a DMA-enabled MAC and after receiving truncate it to the
//              * actually received size. In this case, ensure the tot_len member of the
//              * pbuf is the sum of the chained pbuf len members.

//             memcpy(q->payload, head, q->len);
//             head += q->len;
//         }
//     }

//     return p;
// }

// err_t p2_eth_init(struct netif *netif) {
//     printf("init network interface\n");

//     uint8_t mac_addr[] = {0x00, 0x12, 0x34, 0x56, 0x78, 0x9a};

//     p2_netif.hwaddr_len = 6;               // hardware address length is 6 bytes
//     memcpy(p2_netif.hwaddr, mac_addr, 6);  // copy the MAC address
//     p2_netif.mtu = 1500;                   // MTU is standard 1500 bytes

//     p2_netif.output = etharp_output;       // use etharp as the output. make sure to call etharp_tmr at ARP_TMR_INTERVAL
//     p2_netif.linkoutput = p2_link_output; // our tx function
//     p2_netif.flags = NETIF_FLAG_ETHARP | NETIF_FLAG_LINK_UP;

//     return ERR_OK;
// }

// err_t empty_init(struct netif *netif) {
//     return ERR_OK;
// }

// int net_init(void) {
//     ip_addr_t ip_addr, gw_addr, netmask;
//     IP4_ADDR(&ip_addr, 172, 16, 1, 10);
//     IP4_ADDR(&gw_addr, 172, 16, 1, 1);
//     IP4_ADDR(&netmask, 255, 255, 0, 0);

//     // Initialize LWIP
//     lwip_init();

//     memcpy(p2_netif.name, "en", 2);

//     // Add our netif to LWIP (netif_add calls our driver initialization function)
//     printf("netif add\n");
//     if (netif_add(&p2_netif, &ip_addr, &netmask, &gw_addr, NULL, p2_eth_init, ethernet_input) == NULL) {
//         printf("\nnetif_add failed\n");
//         return -1;
//     }

//     netif_set_addr(&p2_netif, &ip_addr, &netmask, &gw_addr);
//     netif_set_default(&p2_netif);
//     netif_set_up(&p2_netif);

//     if (netif_is_up(&p2_netif)) {
//         printf("Network interface is up\n");
//     } else {
//         printf("Network interface is down: %x\n", p2_netif.flags);
//     }

//     return 0;
// }

int main() {
    _clkset(_SETFREQ, _CLOCKFREQ);
    _uart_init(RX_PIN, TX_PIN, 230400); // FIXME: this is SUPPOSED to called by the C standard lib but it isn't working

    printf("==== Ethernet Demo ====\n");

    ethernet.start();
    waitx(CLKFREQ/10);

    while(1) {
        ethernet.write(0x7e00, 0x0101);
        waitx(CLKFREQ);
    }

    // net_init();

    // udp_pcb *udp_sock = udp_new();
    // udp_bind(udp_sock, IP_ADDR_ANY, 1234);

    // char msg[] = "Hello World!";
    // pbuf *p = pbuf_alloc(PBUF_TRANSPORT, sizeof(msg), PBUF_RAM);
    // memcpy(p->payload, msg, sizeof(msg));

    // ip_addr_t dest;
    // IP4_ADDR(&dest, 172, 16, 1, 2);
    // udp_sendto(udp_sock, p, &dest, 5678);
    // pbuf_free(p);

    // // ethernet.start();
    // // waitx(CLKFREQ);
    // // ethernet.write(0x0101, 0x0101);

    // // for (int i = 0; i < 1500; i+=10) {
    // //     d[i] = 1;
    // // }

    // // dirh(57);
    // // while(1) {
    // //     outnot(57);
    // //     ethernet.tx((uint8_t*)d, 1500);
    // //     waitx(CLKFREQ/5);
    // // }

    // p2_netif.input(p2_input_test(&p2_netif), &p2_netif);

    // while(1) {
    //     etharp_tmr();
    //     waitcnt(CLKFREQ + CNT);
    // }
}