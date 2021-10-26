
build/opt/src/t29:	file format elf32-p2

Disassembly of section .text:

00000000 <__entry>:
       0: f8 a1 03 f6  			 mov r0, ptra	
       4: 00 01 ec fc  			 coginit #0, #256	
       8: 2e 00 64 fd  			 reta	

Disassembly of section .cog:

00000100 <__start>:
     100: f8 a1 03 f6  			 mov r0, ptra	
     104: 01 a0 63 fd  			 cogid r0	
     108: 05 a0 97 fb  			 tjz r0, #5
     10c: f8 a1 03 f6  			 mov r0, ptra	
     110: d0 a3 03 fb  			 rdlong r1, r0	
     114: 04 a0 07 f1  			 add r0, #4	
     118: d0 a1 03 fb  			 rdlong r0, r0	
     11c: 2c a2 63 fd  			 jmp r1	
     120: e0 03 00 ff  			 augs #992
     124: 00 f0 07 f6  			 mov ptra, #0	
     128: 5c 1b c0 fd  			 calla #7004
     12c: 0f 00 00 ff  			 augs #15
     130: 64 a0 07 f6  			 mov r0, #100	
     134: 0f 00 00 ff  			 augs #15
     138: 64 a2 07 f6  			 mov r1, #100	
     13c: d0 a3 5b f2  			 cmps r1, r0	wcz
     140: 14 00 90 ad  		if_z	 jmp #20
     144: d1 a5 03 fb  			 rdlong r2, r1	
     148: 2e a4 63 fd  			 calla r2	
     14c: 04 a2 07 f1  			 add r1, #4	
     150: d0 a3 5b f2  			 cmps r1, r0	wcz
     154: ec ff 9f 5d  		if_nz	 jmp #-20
     158: 00 a0 07 f6  			 mov r0, #0	
     15c: d0 a1 03 fc  			 wrpin r0, r0
     160: 00 09 c0 fd  			 calla #2304
     164: 0f 00 00 ff  			 augs #15
     168: 68 a0 07 f6  			 mov r0, #104	
     16c: 0f 00 00 ff  			 augs #15
     170: 64 a2 07 f6  			 mov r1, #100	
     174: d0 a3 5b f2  			 cmps r1, r0	wcz
     178: 14 00 90 ad  		if_z	 jmp #20
     17c: d1 a5 03 fb  			 rdlong r2, r1	
     180: 2e a4 63 fd  			 calla r2	
     184: 04 a2 07 f1  			 add r1, #4	
     188: d0 a3 5b f2  			 cmps r1, r0	wcz
     18c: ec ff 9f 5d  		if_nz	 jmp #-20
     190: fc ff 9f fd  			 jmp #-4

00000194 <_init>:
     194: 28 04 64 fd  			 setq #2
     198: 61 a1 67 fc  			 wrlong r0, #353
     19c: 0f 00 00 ff  			 augs #15
     1a0: 64 a0 07 f6  			 mov r0, #100	
     1a4: 0f 00 00 ff  			 augs #15
     1a8: 64 a2 07 f6  			 mov r1, #100	
     1ac: d0 a3 5b f2  			 cmps r1, r0	wcz
     1b0: 18 00 90 ad  		if_z	 jmp #24
     1b4: d1 a5 03 fb  			 rdlong r2, r1	
     1b8: 2e a4 63 fd  			 calla r2	
     1bc: 04 a2 07 f1  			 add r1, #4	
     1c0: d0 a3 5b f2  			 cmps r1, r0	wcz
     1c4: 04 00 90 ad  		if_z	 jmp #4
     1c8: e8 ff 9f fd  			 jmp #-24
     1cc: 28 04 64 fd  			 setq #2
     1d0: 5f a1 07 fb  			 rdlong r0, #351	
     1d4: 2e 00 64 fd  			 reta	

000001d8 <_fini>:
     1d8: 28 04 64 fd  			 setq #2
     1dc: 61 a1 67 fc  			 wrlong r0, #353
     1e0: 0f 00 00 ff  			 augs #15
     1e4: 68 a0 07 f6  			 mov r0, #104	
     1e8: 0f 00 00 ff  			 augs #15
     1ec: 64 a2 07 f6  			 mov r1, #100	
     1f0: d0 a3 5b f2  			 cmps r1, r0	wcz
     1f4: 18 00 90 ad  		if_z	 jmp #24
     1f8: d1 a5 03 fb  			 rdlong r2, r1	
     1fc: 2e a4 63 fd  			 calla r2	
     200: 04 a2 07 f1  			 add r1, #4	
     204: d0 a3 5b f2  			 cmps r1, r0	wcz
     208: 04 00 90 ad  		if_z	 jmp #4
     20c: e8 ff 9f fd  			 jmp #-24
     210: 28 04 64 fd  			 setq #2
     214: 5f a1 07 fb  			 rdlong r0, #351	
     218: 2e 00 64 fd  			 reta	

0000021c <__unreachable>:
     21c: fc ff 9f fd  			 jmp #-4

00000220 <__sdiv>:
     220: 28 06 64 fd  			 setq #3
     224: 61 a1 67 fc  			 wrlong r0, #353
     228: d1 a5 43 f6  			 abs r2, r1	
     22c: d0 a3 63 f5  			 xor r1, r0	
     230: d0 a1 43 f6  			 abs r0, r0	
     234: d2 a1 13 fd  			 qdiv r0, r2
     238: 18 a0 63 fd  			 getqx r0	
     23c: d0 a5 63 f6  			 neg r2, r0	
     240: ff ff 7f ff  			 augs #8388607
     244: ff a7 07 f6  			 mov r3, #511	
     248: d3 a3 5b f2  			 cmps r1, r3	wcz
     24c: d0 a1 03 16  	if_nc_and_nz	 mov r0, r0	
     250: d2 a1 03 e6  	if_c_or_z	 mov r0, r2	
     254: d0 df 03 f6  			 mov r31, r0	
     258: 28 06 64 fd  			 setq #3
     25c: 5f a1 07 fb  			 rdlong r0, #351	
     260: 2e 00 64 fd  			 reta	

00000264 <__srem>:
     264: 28 06 64 fd  			 setq #3
     268: 61 a1 67 fc  			 wrlong r0, #353
     26c: d1 a3 43 f6  			 abs r1, r1	
     270: d0 a5 43 f6  			 abs r2, r0	
     274: d1 a5 13 fd  			 qdiv r2, r1
     278: 19 a2 63 fd  			 getqy r1	
     27c: d1 a5 63 f6  			 neg r2, r1	
     280: ff ff 7f ff  			 augs #8388607
     284: ff a7 07 f6  			 mov r3, #511	
     288: d3 a1 5b f2  			 cmps r0, r3	wcz
     28c: d1 a1 03 16  	if_nc_and_nz	 mov r0, r1	
     290: d2 a1 03 e6  	if_c_or_z	 mov r0, r2	
     294: d0 df 03 f6  			 mov r31, r0	
     298: 28 06 64 fd  			 setq #3
     29c: 5f a1 07 fb  			 rdlong r0, #351	
     2a0: 2e 00 64 fd  			 reta	

000002a4 <__memcpy>:
     2a4: 28 06 64 fd  			 setq #3
     2a8: 61 a3 67 fc  			 wrlong r1, #353
     2ac: 00 a0 5f f2  			 cmps r0, #0	wcz
     2b0: 30 00 90 ad  		if_z	 jmp #48
     2b4: 00 a2 5f f2  			 cmps r1, #0	wcz
     2b8: 28 00 90 ad  		if_z	 jmp #40
     2bc: 00 a4 5f f2  			 cmps r2, #0	wcz
     2c0: 20 00 90 ad  		if_z	 jmp #32
     2c4: d0 a7 03 f6  			 mov r3, r0	
     2c8: d1 a9 c3 fa  			 rdbyte r4, r1	
     2cc: d3 a9 43 fc  			 wrbyte r4, r3
     2d0: 01 a6 07 f1  			 add r3, #1	
     2d4: 01 a2 07 f1  			 add r1, #1	
     2d8: 01 a4 87 f1  			 sub r2, #1	
     2dc: 00 a4 5f f2  			 cmps r2, #0	wcz
     2e0: e4 ff 9f 5d  		if_nz	 jmp #-28
     2e4: d0 df 03 f6  			 mov r31, r0	
     2e8: 28 06 64 fd  			 setq #3
     2ec: 5f a3 07 fb  			 rdlong r1, #351	
     2f0: 2e 00 64 fd  			 reta	

000002f4 <__memset>:
     2f4: 28 02 64 fd  			 setq #1
     2f8: 61 a7 67 fc  			 wrlong r3, #353
     2fc: 00 a0 5f f2  			 cmps r0, #0	wcz
     300: 24 00 90 ad  		if_z	 jmp #36
     304: 00 a4 5f f2  			 cmps r2, #0	wcz
     308: 1c 00 90 ad  		if_z	 jmp #28
     30c: 00 a6 07 f6  			 mov r3, #0	
     310: d0 a9 03 f6  			 mov r4, r0	
     314: d3 a9 03 f1  			 add r4, r3	
     318: d4 a3 43 fc  			 wrbyte r1, r4
     31c: 01 a6 07 f1  			 add r3, #1	
     320: d2 a7 1b f2  			 cmp r3, r2	wcz
     324: e8 ff 9f cd  		if_c	 jmp #-24
     328: d0 df 03 f6  			 mov r31, r0	
     32c: 28 02 64 fd  			 setq #1
     330: 5f a7 07 fb  			 rdlong r3, #351	
     334: 2e 00 64 fd  			 reta	

Disassembly of section .text:

00000900 <main>:
     900: 28 04 64 fd  			 setq #2
     904: 61 a1 67 fc  			 wrlong r0, #353
     908: 10 f0 07 f1  			 add ptra, #16	
     90c: f8 ed 03 f6  			 mov pa, ptra	
     910: 10 ec 87 f1  			 sub pa, #16	
     914: f6 01 68 fc  			 wrlong #0, pa
     918: 07 80 00 ff  			 augs #32775
     91c: f8 a0 07 f6  			 mov r0, #248	
     920: d1 f0 08 ff  			 augs #585937
     924: 00 a3 07 f6  			 mov r1, #256	
     928: e0 18 c0 fd  			 calla #6368
     92c: 3f a0 07 f6  			 mov r0, #63	
     930: 3e a2 07 f6  			 mov r1, #62	
     934: c2 01 00 ff  			 augs #450
     938: 00 a4 07 f6  			 mov r2, #0	
     93c: 4c 1a c0 fd  			 calla #6732
     940: f8 a3 03 f6  			 mov r1, ptra	
     944: 00 a2 07 f1  			 add r1, #0	
     948: 0f 00 00 ff  			 augs #15
     94c: 68 a0 07 f6  			 mov r0, #104	
     950: d1 a1 63 fc  			 wrlong r0, r1
     954: 04 f0 07 f1  			 add ptra, #4	
     958: 68 0a c0 fd  			 calla #2664
     95c: 04 f0 87 f1  			 sub ptra, #4	
     960: 01 a0 07 f6  			 mov r0, #1	
     964: 3f a1 67 fc  			 wrlong r0, #319
     968: 02 a2 07 f6  			 mov r1, #2	
     96c: 3e a3 67 fc  			 wrlong r1, #318
     970: 00 a4 07 f6  			 mov r2, #0	
     974: 3d a5 67 fc  			 wrlong r2, #317
     978: c4 09 c0 fd  			 calla #2500
     97c: 3f a5 07 fb  			 rdlong r2, #319	
     980: 3e a3 07 fb  			 rdlong r1, #318	
     984: d2 a1 03 f6  			 mov r0, r2	
     988: c4 09 c0 fd  			 calla #2500
     98c: 3d a1 07 fb  			 rdlong r0, #317	
     990: f8 a5 03 f6  			 mov r2, ptra	
     994: 00 a4 07 f1  			 add r2, #0	
     998: 0f 00 00 ff  			 augs #15
     99c: 87 a2 07 f6  			 mov r1, #135	
     9a0: d2 a3 63 fc  			 wrlong r1, r2
     9a4: 04 f0 07 f1  			 add ptra, #4	
     9a8: 68 0a c0 fd  			 calla #2664
     9ac: 04 f0 87 f1  			 sub ptra, #4	
     9b0: d0 df 03 f6  			 mov r31, r0	
     9b4: 10 f0 87 f1  			 sub ptra, #16	
     9b8: 28 04 64 fd  			 setq #2
     9bc: 5f a1 07 fb  			 rdlong r0, #351	
     9c0: 2e 00 64 fd  			 reta	

000009c4 <_Z1assb>:
     9c4: 28 02 64 fd  			 setq #1
     9c8: 61 a1 67 fc  			 wrlong r0, #353
     9cc: 61 a7 67 fc  			 wrlong r3, #353
     9d0: 06 f0 07 f1  			 add ptra, #6	
     9d4: f8 ed 03 f6  			 mov pa, ptra	
     9d8: 06 ec 87 f1  			 sub pa, #6	
     9dc: f6 a1 53 fc  			 wrword r0, pa
     9e0: f8 ed 03 f6  			 mov pa, ptra	
     9e4: 04 ec 87 f1  			 sub pa, #4	
     9e8: f6 a3 53 fc  			 wrword r1, pa
     9ec: f8 ed 03 f6  			 mov pa, ptra	
     9f0: 02 ec 87 f1  			 sub pa, #2	
     9f4: f6 a5 43 fc  			 wrbyte r2, pa
     9f8: f8 a3 03 f6  			 mov r1, ptra	
     9fc: 00 a2 07 f1  			 add r1, #0	
     a00: 0f 00 00 ff  			 augs #15
     a04: 6e a0 07 f6  			 mov r0, #110	
     a08: d1 a1 63 fc  			 wrlong r0, r1
     a0c: 04 f0 07 f1  			 add ptra, #4	
     a10: 68 0a c0 fd  			 calla #2664
     a14: 04 f0 87 f1  			 sub ptra, #4	
     a18: f8 ed 03 f6  			 mov pa, ptra	
     a1c: 02 ec 87 f1  			 sub pa, #2	
     a20: f6 a1 c3 fa  			 rdbyte r0, pa	
     a24: 01 a0 07 f5  			 and r0, #1	
     a28: 00 a0 5f f2  			 cmps r0, #0	wcz
     a2c: 04 00 90 ad  		if_z	 jmp #4
     a30: 20 00 90 fd  			 jmp #32
     a34: f8 a3 03 f6  			 mov r1, ptra	
     a38: 00 a2 07 f1  			 add r1, #0	
     a3c: 0f 00 00 ff  			 augs #15
     a40: 6b a0 07 f6  			 mov r0, #107	
     a44: d1 a1 63 fc  			 wrlong r0, r1
     a48: 04 f0 07 f1  			 add ptra, #4	
     a4c: 68 0a c0 fd  			 calla #2664
     a50: 04 f0 87 f1  			 sub ptra, #4	
     a54: 06 f0 87 f1  			 sub ptra, #6	
     a58: 5f a7 07 fb  			 rdlong r3, #351	
     a5c: 28 02 64 fd  			 setq #1
     a60: 5f a1 07 fb  			 rdlong r0, #351	
     a64: 2e 00 64 fd  			 reta	

00000a68 <__simple_printf>:
     a68: 28 22 64 fd  			 setq #17
     a6c: 61 a1 67 fc  			 wrlong r0, #353
     a70: 20 f0 07 f1  			 add ptra, #32	
     a74: f8 a1 03 f6  			 mov r0, ptra	
     a78: 74 a0 87 f1  			 sub r0, #116	
     a7c: f8 ed 03 f6  			 mov pa, ptra	
     a80: 04 ec 87 f1  			 sub pa, #4	
     a84: f6 a1 63 fc  			 wrlong r0, pa
     a88: f8 ed 03 f6  			 mov pa, ptra	
     a8c: 08 ec 87 f1  			 sub pa, #8	
     a90: f6 a1 63 fc  			 wrlong r0, pa
     a94: 00 ac 07 f6  			 mov r6, #0	
     a98: f8 ed 03 f6  			 mov pa, ptra	
     a9c: 70 ec 87 f1  			 sub pa, #112	
     aa0: f6 b5 03 fb  			 rdlong r10, pa	
     aa4: 0f 00 00 ff  			 augs #15
     aa8: 9c ae 07 f6  			 mov r7, #156	
     aac: 25 a4 07 f6  			 mov r2, #37	
     ab0: ff ff 7f ff  			 augs #8388607
     ab4: ff b1 07 f6  			 mov r8, #511	
     ab8: 0f 00 00 ff  			 augs #15
     abc: 5c a7 07 f6  			 mov r3, #348	
     ac0: 34 a6 07 f1  			 add r3, #52	
     ac4: 2d a8 07 f6  			 mov r4, #45	
     ac8: 0f 00 00 ff  			 augs #15
     acc: 73 b2 07 f6  			 mov r9, #115	
     ad0: d6 b7 03 f6  			 mov r11, r6	
     ad4: da ab 03 f6  			 mov r5, r10	
     ad8: 01 aa 07 f1  			 add r5, #1	
     adc: da a1 c3 fa  			 rdbyte r0, r10	
     ae0: 25 a0 5f f2  			 cmps r0, #37	wcz
     ae4: 04 00 90 ad  		if_z	 jmp #4
     ae8: 0c 02 90 fd  			 jmp #524
     aec: d5 b9 c3 fa  			 rdbyte r12, r5	
     af0: 00 b8 5f f2  			 cmps r12, #0	wcz
     af4: 00 03 90 ad  		if_z	 jmp #768
     af8: 02 b4 07 f1  			 add r10, #2	
     afc: dc bb 03 f6  			 mov r13, r12	
     b00: d6 ab 03 f6  			 mov r5, r6	
     b04: dd a1 03 f6  			 mov r0, r13	
     b08: 07 a0 67 f7  			 signx r0, #7	
     b0c: 80 a0 1f f2  			 cmp r0, #128	wcz
     b10: d0 a3 03 e6  	if_c_or_z	 mov r1, r0	
     b14: 80 a2 07 16  	if_nc_and_nz	 mov r1, #128	
     b18: d7 bd 03 f6  			 mov r14, r7	
     b1c: d1 bd 03 f1  			 add r14, r1	
     b20: de a3 c3 fa  			 rdbyte r1, r14	
     b24: 02 a2 07 f5  			 and r1, #2	
     b28: 00 a2 5f f2  			 cmps r1, #0	wcz
     b2c: 2c 00 90 ad  		if_z	 jmp #44
     b30: 0a a2 07 f6  			 mov r1, #10	
     b34: d1 ab 03 fd  			 qmul r5, r1
     b38: 18 a2 63 fd  			 getqx r1	
     b3c: d1 a1 03 f1  			 add r0, r1	
     b40: 30 a0 87 f1  			 sub r0, #48	
     b44: da bb c3 fa  			 rdbyte r13, r10	
     b48: 01 b4 07 f1  			 add r10, #1	
     b4c: 00 ba 5f f2  			 cmps r13, #0	wcz
     b50: d0 ab 03 f6  			 mov r5, r0	
     b54: a0 02 90 ad  		if_z	 jmp #672
     b58: a8 ff 9f fd  			 jmp #-88
     b5c: dd a1 03 f6  			 mov r0, r13	
     b60: df a0 07 f5  			 and r0, #223	
     b64: 4c a0 5f f2  			 cmps r0, #76	wcz
     b68: 20 00 90 5d  		if_nz	 jmp #32
     b6c: da bb c3 fa  			 rdbyte r13, r10	
     b70: 01 b4 07 f1  			 add r10, #1	
     b74: dd a1 03 f6  			 mov r0, r13	
     b78: df a0 07 f5  			 and r0, #223	
     b7c: 4c a0 5f f2  			 cmps r0, #76	wcz
     b80: e8 ff 9f ad  		if_z	 jmp #-24
     b84: 00 ba 5f f2  			 cmps r13, #0	wcz
     b88: 6c 02 90 ad  		if_z	 jmp #620
     b8c: dd a1 03 f6  			 mov r0, r13	
     b90: 07 a0 67 f7  			 signx r0, #7	
     b94: 72 a0 5f f2  			 cmps r0, #114	wcz
     b98: 1c 00 90 1d  	if_nc_and_nz	 jmp #28
     b9c: 25 a0 5f f2  			 cmps r0, #37	wcz
     ba0: 74 01 90 ad  		if_z	 jmp #372
     ba4: 63 a0 5f f2  			 cmps r0, #99	wcz
     ba8: 74 01 90 ad  		if_z	 jmp #372
     bac: 64 a0 5f f2  			 cmps r0, #100	wcz
     bb0: 20 00 90 ad  		if_z	 jmp #32
     bb4: 1c ff 9f fd  			 jmp #-228
     bb8: 73 a0 5f f2  			 cmps r0, #115	wcz
     bbc: 9c 01 90 ad  		if_z	 jmp #412
     bc0: 75 a0 5f f2  			 cmps r0, #117	wcz
     bc4: 0c 00 90 ad  		if_z	 jmp #12
     bc8: 78 a0 5f f2  			 cmps r0, #120	wcz
     bcc: 04 00 90 ad  		if_z	 jmp #4
     bd0: 00 ff 9f fd  			 jmp #-256
     bd4: f8 ed 03 f6  			 mov pa, ptra	
     bd8: 08 ec 87 f1  			 sub pa, #8	
     bdc: f6 a1 03 fb  			 rdlong r0, pa	
     be0: d0 a3 03 f6  			 mov r1, r0	
     be4: 04 a2 87 f1  			 sub r1, #4	
     be8: f8 ed 03 f6  			 mov pa, ptra	
     bec: 08 ec 87 f1  			 sub pa, #8	
     bf0: f6 a3 63 fc  			 wrlong r1, pa
     bf4: ff ba 07 f5  			 and r13, #255	
     bf8: d0 bd 03 fb  			 rdlong r14, r0	
     bfc: 64 ba 5f f2  			 cmps r13, #100	wcz
     c00: 20 00 90 5d  		if_nz	 jmp #32
     c04: d8 bd 5b f2  			 cmps r14, r8	wcz
     c08: 18 00 90 1d  	if_nc_and_nz	 jmp #24
     c0c: d4 a1 03 f6  			 mov r0, r4	
     c10: d3 a3 03 f6  			 mov r1, r3	
     c14: c0 12 c0 fd  			 calla #4800
     c18: de bd 63 f6  			 neg r14, r14	
     c1c: 01 aa 87 f1  			 sub r5, #1	
     c20: 01 b6 07 f1  			 add r11, #1	
     c24: 30 b8 5f f2  			 cmps r12, #48	wcz
     c28: 30 a0 07 a6  		if_z	 mov r0, #48	
     c2c: 20 a0 07 56  		if_nz	 mov r0, #32	
     c30: 78 ba 5f f2  			 cmps r13, #120	wcz
     c34: 10 a2 07 a6  		if_z	 mov r1, #16	
     c38: 0a a2 07 56  		if_nz	 mov r1, #10	
     c3c: d6 b9 03 f6  			 mov r12, r6	
     c40: d5 bb 03 f6  			 mov r13, r5	
     c44: d1 bd 13 fd  			 qdiv r14, r1
     c48: 18 be 63 fd  			 getqx r15	
     c4c: d1 bf 03 fd  			 qmul r15, r1
     c50: 18 c0 63 fd  			 getqx r16	
     c54: de c3 03 f6  			 mov r17, r14	
     c58: e0 c3 83 f1  			 sub r17, r16	
     c5c: d9 c1 03 f6  			 mov r16, r9	
     c60: e1 c1 03 f1  			 add r16, r17	
     c64: f8 c3 03 f6  			 mov r17, ptra	
     c68: 20 c2 87 f1  			 sub r17, #32	
     c6c: dc c3 03 f1  			 add r17, r12	
     c70: e0 c1 c3 fa  			 rdbyte r16, r16	
     c74: e1 c1 43 fc  			 wrbyte r16, r17
     c78: 01 aa 87 f1  			 sub r5, #1	
     c7c: 01 b8 07 f1  			 add r12, #1	
     c80: d1 bd 1b f2  			 cmp r14, r1	wcz
     c84: df bd 03 f6  			 mov r14, r15	
     c88: b4 ff 9f 3d  		if_nc	 jmp #-76
     c8c: 02 ba 5f f2  			 cmps r13, #2	wcz
     c90: d6 bb 03 f6  			 mov r13, r6	
     c94: 24 00 90 cd  		if_c	 jmp #36
     c98: d6 bb 03 f6  			 mov r13, r6	
     c9c: d5 bd 03 f6  			 mov r14, r5	
     ca0: d3 a3 03 f6  			 mov r1, r3	
     ca4: c0 12 c0 fd  			 calla #4800
     ca8: 01 ba 07 f1  			 add r13, #1	
     cac: 01 bc 87 f1  			 sub r14, #1	
     cb0: 01 aa 5f f2  			 cmps r5, #1	wcz
     cb4: de ab 03 f6  			 mov r5, r14	
     cb8: e4 ff 9f 1d  	if_nc_and_nz	 jmp #-28
     cbc: dc ab 03 f6  			 mov r5, r12	
     cc0: 01 aa 87 f1  			 sub r5, #1	
     cc4: f8 a1 03 f6  			 mov r0, ptra	
     cc8: 20 a0 87 f1  			 sub r0, #32	
     ccc: d5 a1 03 f1  			 add r0, r5	
     cd0: d0 a1 c3 fa  			 rdbyte r0, r0	
     cd4: 07 a0 67 f7  			 signx r0, #7	
     cd8: d3 a3 03 f6  			 mov r1, r3	
     cdc: c0 12 c0 fd  			 calla #4800
     ce0: 00 aa 5f f2  			 cmps r5, #0	wcz
     ce4: d8 ff 9f 5d  		if_nz	 jmp #-40
     ce8: db b9 03 f1  			 add r12, r11	
     cec: dd b9 03 f1  			 add r12, r13	
     cf0: dc b7 03 f6  			 mov r11, r12	
     cf4: dc fd 9f fd  			 jmp #-548
     cf8: 00 a0 5f f2  			 cmps r0, #0	wcz
     cfc: f8 00 90 ad  		if_z	 jmp #248
     d00: 07 a0 67 f7  			 signx r0, #7	
     d04: d3 a3 03 f6  			 mov r1, r3	
     d08: c0 12 c0 fd  			 calla #4800
     d0c: 01 b6 07 f1  			 add r11, #1	
     d10: d5 b5 03 f6  			 mov r10, r5	
     d14: bc fd 9f fd  			 jmp #-580
     d18: d2 a1 03 f6  			 mov r0, r2	
     d1c: 24 00 90 fd  			 jmp #36
     d20: f8 ed 03 f6  			 mov pa, ptra	
     d24: 08 ec 87 f1  			 sub pa, #8	
     d28: f6 a1 03 fb  			 rdlong r0, pa	
     d2c: d0 a3 03 f6  			 mov r1, r0	
     d30: 04 a2 87 f1  			 sub r1, #4	
     d34: f8 ed 03 f6  			 mov pa, ptra	
     d38: 08 ec 87 f1  			 sub pa, #8	
     d3c: f6 a3 63 fc  			 wrlong r1, pa
     d40: d0 a1 03 fb  			 rdlong r0, r0	
     d44: d5 a3 03 f6  			 mov r1, r5	
     d48: 0c 0e c0 fd  			 calla #3596
     d4c: ef a1 03 f6  			 mov r0, r31	
     d50: db a1 03 f1  			 add r0, r11	
     d54: d0 b7 03 f6  			 mov r11, r0	
     d58: 78 fd 9f fd  			 jmp #-648
     d5c: f8 ed 03 f6  			 mov pa, ptra	
     d60: 08 ec 87 f1  			 sub pa, #8	
     d64: f6 a1 03 fb  			 rdlong r0, pa	
     d68: d0 a3 03 f6  			 mov r1, r0	
     d6c: 04 a2 87 f1  			 sub r1, #4	
     d70: f8 ed 03 f6  			 mov pa, ptra	
     d74: 08 ec 87 f1  			 sub pa, #8	
     d78: f6 a3 63 fc  			 wrlong r1, pa
     d7c: d0 bb 03 fb  			 rdlong r13, r0	
     d80: dd a1 c3 fa  			 rdbyte r0, r13	
     d84: 00 a0 5f f2  			 cmps r0, #0	wcz
     d88: 2c 00 90 ad  		if_z	 jmp #44
     d8c: 00 b8 07 f6  			 mov r12, #0	
     d90: 07 a0 67 f7  			 signx r0, #7	
     d94: d3 a3 03 f6  			 mov r1, r3	
     d98: c0 12 c0 fd  			 calla #4800
     d9c: 01 aa 87 f1  			 sub r5, #1	
     da0: 01 b8 07 f1  			 add r12, #1	
     da4: 01 ba 07 f1  			 add r13, #1	
     da8: dd a1 c3 fa  			 rdbyte r0, r13	
     dac: 00 a0 5f f2  			 cmps r0, #0	wcz
     db0: 08 00 90 ad  		if_z	 jmp #8
     db4: d8 ff 9f fd  			 jmp #-40
     db8: d6 b9 03 f6  			 mov r12, r6	
     dbc: 01 aa 5f f2  			 cmps r5, #1	wcz
     dc0: d5 bb 03 f6  			 mov r13, r5	
     dc4: 24 00 90 cd  		if_c	 jmp #36
     dc8: 20 a0 07 f6  			 mov r0, #32	
     dcc: d3 a3 03 f6  			 mov r1, r3	
     dd0: c0 12 c0 fd  			 calla #4800
     dd4: dd a1 03 f6  			 mov r0, r13	
     dd8: 01 a0 87 f1  			 sub r0, #1	
     ddc: 01 ba 5f f2  			 cmps r13, #1	wcz
     de0: d0 bb 03 f6  			 mov r13, r0	
     de4: e0 ff 9f 1d  	if_nc_and_nz	 jmp #-32
     de8: d5 b9 03 f1  			 add r12, r5	
     dec: db b9 03 f1  			 add r12, r11	
     df0: dc b7 03 f6  			 mov r11, r12	
     df4: dc fc 9f fd  			 jmp #-804
     df8: db df 03 f6  			 mov r31, r11	
     dfc: 20 f0 87 f1  			 sub ptra, #32	
     e00: 28 22 64 fd  			 setq #17
     e04: 5f a1 07 fb  			 rdlong r0, #351	
     e08: 2e 00 64 fd  			 reta	

00000e0c <PUTC>:
     e0c: 28 08 64 fd  			 setq #4
     e10: 61 a1 67 fc  			 wrlong r0, #353
     e14: d1 a5 03 f6  			 mov r2, r1	
     e18: 0f 00 00 ff  			 augs #15
     e1c: 5c a3 07 f6  			 mov r1, #348	
     e20: 34 a2 07 f1  			 add r1, #52	
     e24: c0 12 c0 fd  			 calla #4800
     e28: 02 a4 5f f2  			 cmps r2, #2	wcz
     e2c: 24 00 90 cd  		if_c	 jmp #36
     e30: 20 a0 07 f6  			 mov r0, #32	
     e34: d2 a9 03 f6  			 mov r4, r2	
     e38: d4 a7 03 f6  			 mov r3, r4	
     e3c: c0 12 c0 fd  			 calla #4800
     e40: 01 a6 87 f1  			 sub r3, #1	
     e44: 02 a8 5f f2  			 cmps r4, #2	wcz
     e48: d3 a9 03 f6  			 mov r4, r3	
     e4c: ec ff 9f 1d  	if_nc_and_nz	 jmp #-20
     e50: 04 00 90 fd  			 jmp #4
     e54: 01 a4 07 f6  			 mov r2, #1	
     e58: d2 df 03 f6  			 mov r31, r2	
     e5c: 28 08 64 fd  			 setq #4
     e60: 5f a1 07 fb  			 rdlong r0, #351	
     e64: 2e 00 64 fd  			 reta	

00000e68 <__fopen_driver>:
     e68: 28 12 64 fd  			 setq #9
     e6c: 61 a1 67 fc  			 wrlong r0, #353
     e70: d1 a9 03 f6  			 mov r4, r1	
     e74: 0f 00 00 ff  			 augs #15
     e78: 94 a2 07 f6  			 mov r1, #148	
     e7c: d1 a3 03 fb  			 rdlong r1, r1	
     e80: 00 a2 5f f2  			 cmps r1, #0	wcz
     e84: 6c 01 90 ad  		if_z	 jmp #364
     e88: d3 a3 c3 fa  			 rdbyte r1, r3	
     e8c: 61 a2 5f f2  			 cmps r1, #97	wcz
     e90: 20 00 00 ff  			 augs #32
     e94: 00 aa 07 f6  			 mov r5, #0	
     e98: 2c 00 90 ad  		if_z	 jmp #44
     e9c: 77 a2 5f f2  			 cmps r1, #119	wcz
     ea0: 24 00 90 ad  		if_z	 jmp #36
     ea4: 72 a2 5f f2  			 cmps r1, #114	wcz
     ea8: 4c 00 90 5d  		if_nz	 jmp #76
     eac: d3 a3 03 f6  			 mov r1, r3	
     eb0: 01 a2 07 f1  			 add r1, #1	
     eb4: d1 a3 c3 fa  			 rdbyte r1, r1	
     eb8: 2b a2 5f f2  			 cmps r1, #43	wcz
     ebc: 80 ac 07 a6  		if_z	 mov r6, #128	
     ec0: 01 ac 07 56  		if_nz	 mov r6, #1	
     ec4: 34 00 90 fd  			 jmp #52
     ec8: d3 ad 03 f6  			 mov r6, r3	
     ecc: 01 ac 07 f1  			 add r6, #1	
     ed0: d6 ad c3 fa  			 rdbyte r6, r6	
     ed4: 2b ac 5f f2  			 cmps r6, #43	wcz
     ed8: 80 ae 07 a6  		if_z	 mov r7, #128	
     edc: 02 ae 07 56  		if_nz	 mov r7, #2	
     ee0: d7 ad 03 f6  			 mov r6, r7	
     ee4: d5 ad 43 f5  			 or r6, r5	
     ee8: 61 a2 5f f2  			 cmps r1, #97	wcz
     eec: d6 ad 03 a6  		if_z	 mov r6, r6	
     ef0: d7 ad 03 56  		if_nz	 mov r6, r7	
     ef4: 04 00 90 fd  			 jmp #4
     ef8: 00 ac 07 f6  			 mov r6, #0	
     efc: d0 af 03 f6  			 mov r7, r0	
     f00: 0c ae 07 f1  			 add r7, #12	
     f04: d7 ad 63 fc  			 wrlong r6, r7
     f08: d0 b1 03 f6  			 mov r8, r0	
     f0c: 08 b0 07 f1  			 add r8, #8	
     f10: d8 01 68 fc  			 wrlong #0, r8
     f14: d4 a3 03 f6  			 mov r1, r4	
     f18: 04 a2 07 f1  			 add r1, #4	
     f1c: d1 b3 03 fb  			 rdlong r9, r1	
     f20: 00 b2 5f f2  			 cmps r9, #0	wcz
     f24: d6 a3 03 f6  			 mov r1, r6	
     f28: 30 00 90 ad  		if_z	 jmp #48
     f2c: d2 a3 03 f6  			 mov r1, r2	
     f30: d3 a5 03 f6  			 mov r2, r3	
     f34: 2e b2 63 fd  			 calla r9	
     f38: ff ff 7f ff  			 augs #8388607
     f3c: ff a3 07 f6  			 mov r1, #511	
     f40: d1 df 5b f2  			 cmps r31, r1	wcz
     f44: 04 00 90 1d  	if_nc_and_nz	 jmp #4
     f48: 80 00 90 fd  			 jmp #128
     f4c: d8 a3 03 fb  			 rdlong r1, r8	
     f50: 00 a2 5f f2  			 cmps r1, #0	wcz
     f54: 2c 00 90 5d  		if_nz	 jmp #44
     f58: d7 a3 03 fb  			 rdlong r1, r7	
     f5c: 01 00 00 ff  			 augs #1
     f60: 00 a4 07 f6  			 mov r2, #0	
     f64: d2 a3 43 f5  			 or r1, r2	
     f68: d7 a3 63 fc  			 wrlong r1, r7
     f6c: d0 a3 03 f6  			 mov r1, r0	
     f70: 1c a2 07 f1  			 add r1, #28	
     f74: d8 a3 63 fc  			 wrlong r1, r8
     f78: d0 a5 03 f6  			 mov r2, r0	
     f7c: 10 a4 07 f1  			 add r2, #16	
     f80: d2 11 68 fc  			 wrlong #8, r2
     f84: d0 a5 03 f6  			 mov r2, r0	
     f88: 18 a4 07 f1  			 add r2, #24	
     f8c: d2 a9 63 fc  			 wrlong r4, r2
     f90: d0 a5 03 f6  			 mov r2, r0	
     f94: 04 a4 07 f1  			 add r2, #4	
     f98: d2 a3 63 fc  			 wrlong r1, r2
     f9c: d0 01 68 fc  			 wrlong #0, r0
     fa0: d5 ad 03 f5  			 and r6, r5	
     fa4: 00 ac 5f f2  			 cmps r6, #0	wcz
     fa8: 4c 00 90 ad  		if_z	 jmp #76
     fac: 14 a8 07 f1  			 add r4, #20	
     fb0: d4 a7 03 fb  			 rdlong r3, r4	
     fb4: 00 a6 5f f2  			 cmps r3, #0	wcz
     fb8: 3c 00 90 ad  		if_z	 jmp #60
     fbc: 00 a2 07 f6  			 mov r1, #0	
     fc0: 02 a4 07 f6  			 mov r2, #2	
     fc4: 2e a6 63 fd  			 calla r3	
     fc8: 2c 00 90 fd  			 jmp #44
     fcc: d0 a3 03 f6  			 mov r1, r0	
     fd0: 04 a2 07 f1  			 add r1, #4	
     fd4: d1 01 68 fc  			 wrlong #0, r1
     fd8: d0 a3 03 f6  			 mov r1, r0	
     fdc: 18 a2 07 f1  			 add r1, #24	
     fe0: d1 01 68 fc  			 wrlong #0, r1
     fe4: d8 01 68 fc  			 wrlong #0, r8
     fe8: d7 01 68 fc  			 wrlong #0, r7
     fec: 10 a0 07 f1  			 add r0, #16	
     ff0: d0 01 68 fc  			 wrlong #0, r0
     ff4: 00 a0 07 f6  			 mov r0, #0	
     ff8: d0 df 03 f6  			 mov r31, r0	
     ffc: 28 12 64 fd  			 setq #9
    1000: 5f a1 07 fb  			 rdlong r0, #351	
    1004: 2e 00 64 fd  			 reta	

00001008 <fclose>:
    1008: 28 0a 64 fd  			 setq #5
    100c: 61 a1 67 fc  			 wrlong r0, #353
    1010: 00 a0 5f f2  			 cmps r0, #0	wcz
    1014: 6c 00 90 ad  		if_z	 jmp #108
    1018: d0 a3 03 f6  			 mov r1, r0	
    101c: 18 a2 07 f1  			 add r1, #24	
    1020: d1 a7 03 fb  			 rdlong r3, r1	
    1024: 00 a6 5f f2  			 cmps r3, #0	wcz
    1028: 58 00 90 ad  		if_z	 jmp #88
    102c: d0 a5 03 f6  			 mov r2, r0	
    1030: 0c a4 07 f1  			 add r2, #12	
    1034: d2 a9 03 fb  			 rdlong r4, r2	
    1038: d4 ab 03 f6  			 mov r5, r4	
    103c: 83 aa 07 f5  			 and r5, #131	
    1040: 00 aa 5f f2  			 cmps r5, #0	wcz
    1044: 04 00 90 5d  		if_nz	 jmp #4
    1048: 38 00 90 fd  			 jmp #56
    104c: 02 a8 07 f5  			 and r4, #2	
    1050: 00 a8 5f f2  			 cmps r4, #0	wcz
    1054: 08 00 90 ad  		if_z	 jmp #8
    1058: e4 10 c0 fd  			 calla #4324
    105c: d1 a7 03 fb  			 rdlong r3, r1	
    1060: 08 a6 07 f1  			 add r3, #8	
    1064: d3 a7 03 fb  			 rdlong r3, r3	
    1068: 00 a6 5f f2  			 cmps r3, #0	wcz
    106c: 34 00 90 ad  		if_z	 jmp #52
    1070: 2e a6 63 fd  			 calla r3	
    1074: 00 de 5f f2  			 cmps r31, #0	wcz
    1078: 01 a6 07 56  		if_nz	 mov r3, #1	
    107c: 00 a6 07 a6  		if_z	 mov r3, #0	
    1080: 24 00 90 fd  			 jmp #36
    1084: 0f 00 00 ff  			 augs #15
    1088: 44 a1 07 f6  			 mov r0, #324	
    108c: d0 a1 03 fb  			 rdlong r0, r0	
    1090: 08 a0 07 f1  			 add r0, #8	
    1094: d0 0b 68 fc  			 wrlong #5, r0
    1098: ff ff 7f ff  			 augs #8388607
    109c: ff a7 07 f6  			 mov r3, #511	
    10a0: 30 00 90 fd  			 jmp #48
    10a4: 00 a6 07 f6  			 mov r3, #0	
    10a8: d0 a9 03 f6  			 mov r4, r0	
    10ac: 04 a8 07 f1  			 add r4, #4	
    10b0: d4 01 68 fc  			 wrlong #0, r4
    10b4: d1 01 68 fc  			 wrlong #0, r1
    10b8: d0 a3 03 f6  			 mov r1, r0	
    10bc: 08 a2 07 f1  			 add r1, #8	
    10c0: d1 01 68 fc  			 wrlong #0, r1
    10c4: d2 01 68 fc  			 wrlong #0, r2
    10c8: 10 a0 07 f1  			 add r0, #16	
    10cc: d0 01 68 fc  			 wrlong #0, r0
    10d0: 00 a6 67 f7  			 signx r3, #0	
    10d4: d3 df 03 f6  			 mov r31, r3	
    10d8: 28 0a 64 fd  			 setq #5
    10dc: 5f a1 07 fb  			 rdlong r0, #351	
    10e0: 2e 00 64 fd  			 reta	

000010e4 <fflush>:
    10e4: 28 0c 64 fd  			 setq #6
    10e8: 61 a1 67 fc  			 wrlong r0, #353
    10ec: 00 a0 5f f2  			 cmps r0, #0	wcz
    10f0: 04 00 90 5d  		if_nz	 jmp #4
    10f4: 0c 00 90 fd  			 jmp #12
    10f8: 50 11 c0 fd  			 calla #4432
    10fc: ef a3 03 f6  			 mov r1, r31	
    1100: 3c 00 90 fd  			 jmp #60
    1104: 00 a2 07 f6  			 mov r1, #0	
    1108: 34 a4 07 f6  			 mov r2, #52	
    110c: 0f 00 00 ff  			 augs #15
    1110: 5c a7 07 f6  			 mov r3, #348	
    1114: d1 ab 03 f6  			 mov r5, r1	
    1118: d5 a9 03 f6  			 mov r4, r5	
    111c: d2 ab 03 fd  			 qmul r5, r2
    1120: 18 ac 63 fd  			 getqx r6	
    1124: d3 a1 03 f6  			 mov r0, r3	
    1128: d6 a1 03 f1  			 add r0, r6	
    112c: 50 11 c0 fd  			 calla #4432
    1130: 01 a8 07 f1  			 add r4, #1	
    1134: 07 aa 1f f2  			 cmp r5, #7	wcz
    1138: d4 ab 03 f6  			 mov r5, r4	
    113c: dc ff 9f cd  		if_c	 jmp #-36
    1140: d1 df 03 f6  			 mov r31, r1	
    1144: 28 0c 64 fd  			 setq #6
    1148: 5f a1 07 fb  			 rdlong r0, #351	
    114c: 2e 00 64 fd  			 reta	

00001150 <_fflush>:
    1150: 28 12 64 fd  			 setq #9
    1154: 61 a1 67 fc  			 wrlong r0, #353
    1158: d0 a7 03 f6  			 mov r3, r0	
    115c: 00 a6 5f f2  			 cmps r3, #0	wcz
    1160: b4 00 90 ad  		if_z	 jmp #180
    1164: d3 a9 03 f6  			 mov r4, r3	
    1168: 0c a8 07 f1  			 add r4, #12	
    116c: d4 ad 03 fb  			 rdlong r6, r4	
    1170: d6 a1 03 f6  			 mov r0, r6	
    1174: 83 a0 07 f5  			 and r0, #131	
    1178: 00 a0 5f f2  			 cmps r0, #0	wcz
    117c: a0 00 90 ad  		if_z	 jmp #160
    1180: d3 ab 03 f6  			 mov r5, r3	
    1184: 14 aa 07 f1  			 add r5, #20	
    1188: d5 a1 03 fb  			 rdlong r0, r5	
    118c: a8 19 c0 fd  			 calla #6568
    1190: 00 b0 07 f6  			 mov r8, #0	
    1194: d3 a5 03 fb  			 rdlong r2, r3	
    1198: 01 a4 5f f2  			 cmps r2, #1	wcz
    119c: d3 af 03 f6  			 mov r7, r3	
    11a0: 08 ae 07 f1  			 add r7, #8	
    11a4: cc 00 90 cd  		if_c	 jmp #204
    11a8: d6 a1 03 f6  			 mov r0, r6	
    11ac: 02 a0 07 f5  			 and r0, #2	
    11b0: 00 a0 5f f2  			 cmps r0, #0	wcz
    11b4: 04 00 90 ad  		if_z	 jmp #4
    11b8: 70 00 90 fd  			 jmp #112
    11bc: d6 a1 03 f6  			 mov r0, r6	
    11c0: 01 a0 07 f5  			 and r0, #1	
    11c4: 00 a0 5f f2  			 cmps r0, #0	wcz
    11c8: a8 00 90 ad  		if_z	 jmp #168
    11cc: d3 a1 03 f6  			 mov r0, r3	
    11d0: 18 a0 07 f1  			 add r0, #24	
    11d4: d0 a1 03 fb  			 rdlong r0, r0	
    11d8: 14 a0 07 f1  			 add r0, #20	
    11dc: d0 b1 03 fb  			 rdlong r8, r0	
    11e0: d2 a3 63 f6  			 neg r1, r2	
    11e4: 01 a4 07 f6  			 mov r2, #1	
    11e8: d3 a1 03 f6  			 mov r0, r3	
    11ec: 2e b0 63 fd  			 calla r8	
    11f0: ef b1 03 f6  			 mov r8, r31	
    11f4: 1f b0 47 f0  			 shr r8, #31	
    11f8: d6 a1 03 f6  			 mov r0, r6	
    11fc: 08 a0 07 f5  			 and r0, #8	
    1200: 00 a0 5f f2  			 cmps r0, #0	wcz
    1204: 01 a0 07 a6  		if_z	 mov r0, #1	
    1208: 00 a0 07 56  		if_nz	 mov r0, #0	
    120c: d0 b1 03 f5  			 and r8, r0	
    1210: 00 b0 67 f7  			 signx r8, #0	
    1214: 5c 00 90 fd  			 jmp #92
    1218: 00 b0 07 f6  			 mov r8, #0	
    121c: 90 00 90 fd  			 jmp #144
    1220: ff ff 7f ff  			 augs #8388607
    1224: ff b1 07 f6  			 mov r8, #511	
    1228: 84 00 90 fd  			 jmp #132
    122c: d3 01 68 fc  			 wrlong #0, r3
    1230: d7 a3 03 fb  			 rdlong r1, r7	
    1234: d3 a1 03 f6  			 mov r0, r3	
    1238: 18 a0 07 f1  			 add r0, #24	
    123c: d0 a1 03 fb  			 rdlong r0, r0	
    1240: 10 a0 07 f1  			 add r0, #16	
    1244: d0 b3 03 fb  			 rdlong r9, r0	
    1248: d3 a1 03 f6  			 mov r0, r3	
    124c: 2e b2 63 fd  			 calla r9	
    1250: d2 df 5b f2  			 cmps r31, r2	wcz
    1254: 1c 00 90 ad  		if_z	 jmp #28
    1258: d4 a1 03 fb  			 rdlong r0, r4	
    125c: 10 00 00 ff  			 augs #16
    1260: 00 a2 07 f6  			 mov r1, #0	
    1264: d1 a1 43 f5  			 or r0, r1	
    1268: d4 a1 63 fc  			 wrlong r0, r4
    126c: ff ff 7f ff  			 augs #8388607
    1270: ff b1 07 f6  			 mov r8, #511	
    1274: 80 ac 07 f5  			 and r6, #128	
    1278: 00 ac 5f f2  			 cmps r6, #0	wcz
    127c: 14 00 90 ad  		if_z	 jmp #20
    1280: d4 a1 03 fb  			 rdlong r0, r4	
    1284: ff ff 7f ff  			 augs #8388607
    1288: fc a3 07 f6  			 mov r1, #508	
    128c: d1 a1 03 f5  			 and r0, r1	
    1290: d4 a1 63 fc  			 wrlong r0, r4
    1294: d7 a1 03 fb  			 rdlong r0, r7	
    1298: d3 a3 03 f6  			 mov r1, r3	
    129c: 04 a2 07 f1  			 add r1, #4	
    12a0: d1 a1 63 fc  			 wrlong r0, r1
    12a4: d3 01 68 fc  			 wrlong #0, r3
    12a8: d5 a1 03 fb  			 rdlong r0, r5	
    12ac: 10 1a c0 fd  			 calla #6672
    12b0: d8 df 03 f6  			 mov r31, r8	
    12b4: 28 12 64 fd  			 setq #9
    12b8: 5f a1 07 fb  			 rdlong r0, #351	
    12bc: 2e 00 64 fd  			 reta	

000012c0 <fputc>:
    12c0: 28 0e 64 fd  			 setq #7
    12c4: 61 a1 67 fc  			 wrlong r0, #353
    12c8: d0 a7 03 f6  			 mov r3, r0	
    12cc: d1 ab 03 f6  			 mov r5, r1	
    12d0: 0c aa 07 f1  			 add r5, #12	
    12d4: d5 a1 03 fb  			 rdlong r0, r5	
    12d8: d0 a5 03 f6  			 mov r2, r0	
    12dc: 80 a4 07 f5  			 and r2, #128	
    12e0: 00 a4 5f f2  			 cmps r2, #0	wcz
    12e4: 14 00 90 ad  		if_z	 jmp #20
    12e8: f7 ff 7f ff  			 augs #8388599
    12ec: fc a5 07 f6  			 mov r2, #508	
    12f0: d2 a1 03 f5  			 and r0, r2	
    12f4: 02 a0 47 f5  			 or r0, #2	
    12f8: d5 a1 63 fc  			 wrlong r0, r5
    12fc: 18 00 00 ff  			 augs #24
    1300: 02 a4 07 f6  			 mov r2, #2	
    1304: d0 a9 03 f6  			 mov r4, r0	
    1308: d2 a9 03 f5  			 and r4, r2	
    130c: ff ff 7f ff  			 augs #8388607
    1310: ff ad 07 f6  			 mov r6, #511	
    1314: 02 a8 5f f2  			 cmps r4, #2	wcz
    1318: a8 00 90 5d  		if_nz	 jmp #168
    131c: d1 af 03 f6  			 mov r7, r1	
    1320: 04 ae 07 f1  			 add r7, #4	
    1324: d7 a5 03 fb  			 rdlong r2, r7	
    1328: d2 a9 03 f6  			 mov r4, r2	
    132c: 01 a8 07 f1  			 add r4, #1	
    1330: d7 a9 63 fc  			 wrlong r4, r7
    1334: d2 a7 43 fc  			 wrbyte r3, r2
    1338: d1 a5 03 fb  			 rdlong r2, r1	
    133c: 01 a4 07 f1  			 add r2, #1	
    1340: d1 a5 63 fc  			 wrlong r2, r1
    1344: d1 a9 03 f6  			 mov r4, r1	
    1348: 10 a8 07 f1  			 add r4, #16	
    134c: d4 a9 03 fb  			 rdlong r4, r4	
    1350: d4 a5 5b f2  			 cmps r2, r4	wcz
    1354: 1c 00 90 3d  		if_nc	 jmp #28
    1358: 01 00 00 ff  			 augs #1
    135c: 00 a8 07 f6  			 mov r4, #0	
    1360: d4 a1 03 f5  			 and r0, r4	
    1364: 00 a0 5f f2  			 cmps r0, #0	wcz
    1368: 5c 00 90 ad  		if_z	 jmp #92
    136c: 0a a6 5f f2  			 cmps r3, #10	wcz
    1370: 54 00 90 5d  		if_nz	 jmp #84
    1374: d1 a1 03 f6  			 mov r0, r1	
    1378: 08 a0 07 f1  			 add r0, #8	
    137c: d0 a9 03 fb  			 rdlong r4, r0	
    1380: d7 a9 63 fc  			 wrlong r4, r7
    1384: d1 01 68 fc  			 wrlong #0, r1
    1388: d1 a1 03 f6  			 mov r0, r1	
    138c: 18 a0 07 f1  			 add r0, #24	
    1390: d0 a1 03 fb  			 rdlong r0, r0	
    1394: 10 a0 07 f1  			 add r0, #16	
    1398: d0 af 03 fb  			 rdlong r7, r0	
    139c: d1 a1 03 f6  			 mov r0, r1	
    13a0: d4 a3 03 f6  			 mov r1, r4	
    13a4: 2e ae 63 fd  			 calla r7	
    13a8: d2 df 5b f2  			 cmps r31, r2	wcz
    13ac: 18 00 90 ad  		if_z	 jmp #24
    13b0: d5 a1 03 fb  			 rdlong r0, r5	
    13b4: 10 00 00 ff  			 augs #16
    13b8: 00 a2 07 f6  			 mov r1, #0	
    13bc: d1 a1 43 f5  			 or r0, r1	
    13c0: d5 a1 63 fc  			 wrlong r0, r5
    13c4: d6 a7 03 f6  			 mov r3, r6	
    13c8: d3 df 03 f6  			 mov r31, r3	
    13cc: 28 0e 64 fd  			 setq #7
    13d0: 5f a1 07 fb  			 rdlong r0, #351	
    13d4: 2e 00 64 fd  			 reta	

000013d8 <_InitIO>:
    13d8: 28 0a 64 fd  			 setq #5
    13dc: 61 a1 67 fc  			 wrlong r0, #353
    13e0: 0f 00 00 ff  			 augs #15
    13e4: 94 aa 07 f6  			 mov r5, #148	
    13e8: d5 a3 03 fb  			 rdlong r1, r5	
    13ec: 0f 00 00 ff  			 augs #15
    13f0: 5c a9 07 f6  			 mov r4, #348	
    13f4: 0f 00 00 ff  			 augs #15
    13f8: 86 a4 07 f6  			 mov r2, #134	
    13fc: 0f 00 00 ff  			 augs #15
    1400: 84 a6 07 f6  			 mov r3, #132	
    1404: d4 a1 03 f6  			 mov r0, r4	
    1408: 68 0e c0 fd  			 calla #3688
    140c: 08 a0 07 f1  			 add r0, #8	
    1410: 10 00 00 ff  			 augs #16
    1414: fc a2 07 f6  			 mov r1, #252	
    1418: d0 a3 63 fc  			 wrlong r1, r0
    141c: d4 a1 03 f6  			 mov r0, r4	
    1420: 04 a0 07 f1  			 add r0, #4	
    1424: d0 a3 63 fc  			 wrlong r1, r0
    1428: d4 a1 03 f6  			 mov r0, r4	
    142c: 0c a0 07 f1  			 add r0, #12	
    1430: d0 a3 03 fb  			 rdlong r1, r0	
    1434: 10 a2 47 f5  			 or r1, #16	
    1438: d0 a3 63 fc  			 wrlong r1, r0
    143c: d4 a1 03 f6  			 mov r0, r4	
    1440: 10 a0 07 f1  			 add r0, #16	
    1444: d0 a1 68 fc  			 wrlong #80, r0
    1448: d5 a3 03 fb  			 rdlong r1, r5	
    144c: d4 a1 03 f6  			 mov r0, r4	
    1450: 34 a0 07 f1  			 add r0, #52	
    1454: 0f 00 00 ff  			 augs #15
    1458: 71 a6 07 f6  			 mov r3, #113	
    145c: 68 0e c0 fd  			 calla #3688
    1460: 68 a8 07 f1  			 add r4, #104	
    1464: d5 a3 03 fb  			 rdlong r1, r5	
    1468: d4 a1 03 f6  			 mov r0, r4	
    146c: 68 0e c0 fd  			 calla #3688
    1470: 28 0a 64 fd  			 setq #5
    1474: 5f a1 07 fb  			 rdlong r0, #351	
    1478: 2e 00 64 fd  			 reta	

0000147c <SimpleSerialExit>:
    147c: 2e 00 64 fd  			 reta	

00001480 <_serial_fopen>:
    1480: 28 0c 64 fd  			 setq #6
    1484: 61 a1 67 fc  			 wrlong r0, #353
    1488: d1 ab 03 f6  			 mov r5, r1	
    148c: d0 a7 03 f6  			 mov r3, r0	
    1490: 10 00 00 ff  			 augs #16
    1494: f0 a1 07 f6  			 mov r0, #496	
    1498: d0 a9 03 fb  			 rdlong r4, r0	
    149c: 10 00 00 ff  			 augs #16
    14a0: f4 a1 07 f6  			 mov r0, #500	
    14a4: d0 a3 03 fb  			 rdlong r1, r0	
    14a8: 1c a0 07 f6  			 mov r0, #28	
    14ac: d0 a5 03 fb  			 rdlong r2, r0	
    14b0: 00 aa 5f f2  			 cmps r5, #0	wcz
    14b4: 80 00 90 ad  		if_z	 jmp #128
    14b8: d5 ad c3 fa  			 rdbyte r6, r5	
    14bc: 00 ac 5f f2  			 cmps r6, #0	wcz
    14c0: 74 00 90 ad  		if_z	 jmp #116
    14c4: d5 a1 03 f6  			 mov r0, r5	
    14c8: fc 15 c0 fd  			 calla #5628
    14cc: ef a5 03 f6  			 mov r2, r31	
    14d0: ff ac 07 f5  			 and r6, #255	
    14d4: 00 ac 5f f2  			 cmps r6, #0	wcz
    14d8: 5c 00 90 ad  		if_z	 jmp #92
    14dc: 2c ac 5f f2  			 cmps r6, #44	wcz
    14e0: 18 00 90 ad  		if_z	 jmp #24
    14e4: d5 a1 03 f6  			 mov r0, r5	
    14e8: 01 a0 07 f1  			 add r0, #1	
    14ec: 01 aa 07 f1  			 add r5, #1	
    14f0: d5 ad c3 fa  			 rdbyte r6, r5	
    14f4: d0 ab 03 f6  			 mov r5, r0	
    14f8: d4 ff 9f fd  			 jmp #-44
    14fc: 01 aa 07 f1  			 add r5, #1	
    1500: d5 a1 03 f6  			 mov r0, r5	
    1504: fc 15 c0 fd  			 calla #5628
    1508: ef a9 03 f6  			 mov r4, r31	
    150c: d5 a1 c3 fa  			 rdbyte r0, r5	
    1510: 00 a0 5f f2  			 cmps r0, #0	wcz
    1514: 20 00 90 ad  		if_z	 jmp #32
    1518: 2c a0 5f f2  			 cmps r0, #44	wcz
    151c: 08 00 90 ad  		if_z	 jmp #8
    1520: 01 aa 07 f1  			 add r5, #1	
    1524: e4 ff 9f fd  			 jmp #-28
    1528: 01 aa 07 f1  			 add r5, #1	
    152c: d5 a1 03 f6  			 mov r0, r5	
    1530: fc 15 c0 fd  			 calla #5628
    1534: ef a3 03 f6  			 mov r1, r31	
    1538: d3 a1 03 f6  			 mov r0, r3	
    153c: 2c a0 07 f1  			 add r0, #44	
    1540: d0 a5 63 fc  			 wrlong r2, r0
    1544: d3 a1 03 f6  			 mov r0, r3	
    1548: 28 a0 07 f1  			 add r0, #40	
    154c: d0 a3 63 fc  			 wrlong r1, r0
    1550: d3 a1 03 f6  			 mov r0, r3	
    1554: 24 a0 07 f1  			 add r0, #36	
    1558: d0 a9 63 fc  			 wrlong r4, r0
    155c: d4 a1 03 f6  			 mov r0, r4	
    1560: 4c 1a c0 fd  			 calla #6732
    1564: d3 a1 03 f6  			 mov r0, r3	
    1568: 0c a0 07 f1  			 add r0, #12	
    156c: d0 a3 03 fb  			 rdlong r1, r0	
    1570: 08 a2 47 f5  			 or r1, #8	
    1574: d0 a3 63 fc  			 wrlong r1, r0
    1578: 14 a6 07 f1  			 add r3, #20	
    157c: 98 19 c0 fd  			 calla #6552
    1580: d3 df 63 fc  			 wrlong r31, r3
    1584: 00 de 07 f6  			 mov r31, #0	
    1588: 28 0c 64 fd  			 setq #6
    158c: 5f a1 07 fb  			 rdlong r0, #351	
    1590: 2e 00 64 fd  			 reta	

00001594 <_serial_getbyte>:
    1594: 61 a1 67 fc  			 wrlong r0, #353
    1598: 0d a0 07 f1  			 add r0, #13	
    159c: d0 a1 c3 fa  			 rdbyte r0, r0	
    15a0: 80 a0 07 f5  			 and r0, #128	
    15a4: 00 a0 5f f2  			 cmps r0, #0	wcz
    15a8: 18 00 90 5d  		if_nz	 jmp #24
    15ac: ff ff 7f ff  			 augs #8388607
    15b0: ff a1 07 f6  			 mov r0, #511	
    15b4: 10 1b c0 fd  			 calla #6928
    15b8: d0 df 5b f2  			 cmps r31, r0	wcz
    15bc: f4 ff 9f ad  		if_z	 jmp #-12
    15c0: 04 00 90 fd  			 jmp #4
    15c4: 10 1b c0 fd  			 calla #6928
    15c8: ef a1 03 f6  			 mov r0, r31	
    15cc: d0 df 03 f6  			 mov r31, r0	
    15d0: 5f a1 07 fb  			 rdlong r0, #351	
    15d4: 2e 00 64 fd  			 reta	

000015d8 <_serial_putbyte>:
    15d8: 28 02 64 fd  			 setq #1
    15dc: 61 a1 67 fc  			 wrlong r0, #353
    15e0: d0 a3 03 f6  			 mov r1, r0	
    15e4: 07 a0 67 f7  			 signx r0, #7	
    15e8: d0 1a c0 fd  			 calla #6864
    15ec: d1 df 03 f6  			 mov r31, r1	
    15f0: 28 02 64 fd  			 setq #1
    15f4: 5f a1 07 fb  			 rdlong r0, #351	
    15f8: 2e 00 64 fd  			 reta	

000015fc <atol>:
    15fc: 28 10 64 fd  			 setq #8
    1600: 61 a1 67 fc  			 wrlong r0, #353
    1604: 0f 00 00 ff  			 augs #15
    1608: 9c a4 07 f6  			 mov r2, #156	
    160c: d0 a7 03 f6  			 mov r3, r0	
    1610: d3 a3 c3 fa  			 rdbyte r1, r3	
    1614: 07 a2 67 f7  			 signx r1, #7	
    1618: 80 a2 1f f2  			 cmp r1, #128	wcz
    161c: d1 a9 03 e6  	if_c_or_z	 mov r4, r1	
    1620: 80 a8 07 16  	if_nc_and_nz	 mov r4, #128	
    1624: d2 ab 03 f6  			 mov r5, r2	
    1628: d4 ab 03 f1  			 add r5, r4	
    162c: 01 a0 07 f1  			 add r0, #1	
    1630: d5 a9 c3 fa  			 rdbyte r4, r5	
    1634: 10 a8 07 f5  			 and r4, #16	
    1638: 00 a8 5f f2  			 cmps r4, #0	wcz
    163c: cc ff 9f 5d  		if_nz	 jmp #-52
    1640: 2d a2 5f f2  			 cmps r1, #45	wcz
    1644: 01 a0 07 a6  		if_z	 mov r0, #1	
    1648: 00 a0 07 56  		if_nz	 mov r0, #0	
    164c: d0 a7 03 f1  			 add r3, r0	
    1650: d3 a1 c3 fa  			 rdbyte r0, r3	
    1654: 07 a0 67 f7  			 signx r0, #7	
    1658: 80 a0 1f f2  			 cmp r0, #128	wcz
    165c: d0 a9 03 e6  	if_c_or_z	 mov r4, r0	
    1660: 80 a8 07 16  	if_nc_and_nz	 mov r4, #128	
    1664: d2 ab 03 f6  			 mov r5, r2	
    1668: d4 ab 03 f1  			 add r5, r4	
    166c: d5 a9 c3 fa  			 rdbyte r4, r5	
    1670: 02 a8 07 f5  			 and r4, #2	
    1674: 00 a8 5f f2  			 cmps r4, #0	wcz
    1678: 04 00 90 ad  		if_z	 jmp #4
    167c: 08 00 90 fd  			 jmp #8
    1680: 00 a8 07 f6  			 mov r4, #0	
    1684: 4c 00 90 fd  			 jmp #76
    1688: 00 a8 07 f6  			 mov r4, #0	
    168c: 0a aa 07 f6  			 mov r5, #10	
    1690: d5 a9 03 fd  			 qmul r4, r5
    1694: 18 a8 63 fd  			 getqx r4	
    1698: 01 a6 07 f1  			 add r3, #1	
    169c: d3 ad c3 fa  			 rdbyte r6, r3	
    16a0: 07 ac 67 f7  			 signx r6, #7	
    16a4: 80 ac 1f f2  			 cmp r6, #128	wcz
    16a8: d6 af 03 e6  	if_c_or_z	 mov r7, r6	
    16ac: 80 ae 07 16  	if_nc_and_nz	 mov r7, #128	
    16b0: d2 b1 03 f6  			 mov r8, r2	
    16b4: d7 b1 03 f1  			 add r8, r7	
    16b8: d0 a9 03 f1  			 add r4, r0	
    16bc: 30 a8 87 f1  			 sub r4, #48	
    16c0: d8 a1 c3 fa  			 rdbyte r0, r8	
    16c4: 02 a0 07 f5  			 and r0, #2	
    16c8: 00 a0 5f f2  			 cmps r0, #0	wcz
    16cc: d6 a1 03 f6  			 mov r0, r6	
    16d0: bc ff 9f 5d  		if_nz	 jmp #-68
    16d4: d4 a1 63 f6  			 neg r0, r4	
    16d8: 2d a2 5f f2  			 cmps r1, #45	wcz
    16dc: d0 a1 03 a6  		if_z	 mov r0, r0	
    16e0: d4 a1 03 56  		if_nz	 mov r0, r4	
    16e4: d0 df 03 f6  			 mov r31, r0	
    16e8: 28 10 64 fd  			 setq #8
    16ec: 5f a1 07 fb  			 rdlong r0, #351	
    16f0: 2e 00 64 fd  			 reta	

000016f4 <_term_write>:
    16f4: 28 02 64 fd  			 setq #1
    16f8: 61 a1 67 fc  			 wrlong r0, #353
    16fc: 28 08 64 fd  			 setq #4
    1700: 61 a7 67 fc  			 wrlong r3, #353
    1704: d1 a7 03 f6  			 mov r3, r1	
    1708: d0 a3 03 f6  			 mov r1, r0	
    170c: 00 a0 07 f6  			 mov r0, #0	
    1710: d1 a9 03 f6  			 mov r4, r1	
    1714: 18 a8 07 f1  			 add r4, #24	
    1718: d4 a9 03 fb  			 rdlong r4, r4	
    171c: 20 a8 07 f1  			 add r4, #32	
    1720: d4 ad 03 fb  			 rdlong r6, r4	
    1724: 00 ac 5f f2  			 cmps r6, #0	wcz
    1728: 40 00 90 ad  		if_z	 jmp #64
    172c: 01 a4 5f f2  			 cmps r2, #1	wcz
    1730: 38 00 90 cd  		if_c	 jmp #56
    1734: 00 ae 07 f6  			 mov r7, #0	
    1738: 0d a8 07 f6  			 mov r4, #13	
    173c: d3 ab c3 fa  			 rdbyte r5, r3	
    1740: 0a aa 5f f2  			 cmps r5, #10	wcz
    1744: 08 00 90 5d  		if_nz	 jmp #8
    1748: d4 a1 03 f6  			 mov r0, r4	
    174c: 2e ac 63 fd  			 calla r6	
    1750: 01 a6 07 f1  			 add r3, #1	
    1754: d5 a1 03 f6  			 mov r0, r5	
    1758: 2e ac 63 fd  			 calla r6	
    175c: 01 ae 07 f1  			 add r7, #1	
    1760: d2 af 5b f2  			 cmps r7, r2	wcz
    1764: d4 ff 9f cd  		if_c	 jmp #-44
    1768: d2 a1 03 f6  			 mov r0, r2	
    176c: d0 df 03 f6  			 mov r31, r0	
    1770: 28 08 64 fd  			 setq #4
    1774: 5f a7 07 fb  			 rdlong r3, #351	
    1778: 28 02 64 fd  			 setq #1
    177c: 5f a1 07 fb  			 rdlong r0, #351	
    1780: 2e 00 64 fd  			 reta	

00001784 <_term_read>:
    1784: 28 20 64 fd  			 setq #16
    1788: 61 a1 67 fc  			 wrlong r0, #353
    178c: d1 ad 03 f6  			 mov r6, r1	
    1790: d0 a3 03 f6  			 mov r1, r0	
    1794: 00 be 07 f6  			 mov r15, #0	
    1798: 18 a0 07 f1  			 add r0, #24	
    179c: d0 a1 03 fb  			 rdlong r0, r0	
    17a0: d0 a7 03 f6  			 mov r3, r0	
    17a4: 20 a6 07 f1  			 add r3, #32	
    17a8: d3 af 03 fb  			 rdlong r7, r3	
    17ac: 00 ae 5f f2  			 cmps r7, #0	wcz
    17b0: 04 01 90 ad  		if_z	 jmp #260
    17b4: 1c a0 07 f1  			 add r0, #28	
    17b8: d0 b1 03 fb  			 rdlong r8, r0	
    17bc: 00 b0 5f f2  			 cmps r8, #0	wcz
    17c0: f4 00 90 ad  		if_z	 jmp #244
    17c4: d1 a1 03 f6  			 mov r0, r1	
    17c8: 0c a0 07 f1  			 add r0, #12	
    17cc: d0 b3 03 fb  			 rdlong r9, r0	
    17d0: 00 b4 07 f6  			 mov r10, #0	
    17d4: 02 00 00 ff  			 augs #2
    17d8: 00 a0 07 f6  			 mov r0, #0	
    17dc: d9 b7 03 f6  			 mov r11, r9	
    17e0: d0 b7 03 f5  			 and r11, r0	
    17e4: 10 b2 07 f5  			 and r9, #16	
    17e8: 00 b2 5f f2  			 cmps r9, #0	wcz
    17ec: d2 a5 03 56  		if_nz	 mov r2, r2	
    17f0: 01 a4 07 a6  		if_z	 mov r2, #1	
    17f4: ff ff 7f ff  			 augs #8388607
    17f8: ff b9 07 f6  			 mov r12, #511	
    17fc: 0d a6 07 f6  			 mov r3, #13	
    1800: 0a ba 07 f6  			 mov r13, #10	
    1804: 08 a8 07 f6  			 mov r4, #8	
    1808: 20 aa 07 f6  			 mov r5, #32	
    180c: da bf 03 f6  			 mov r15, r10	
    1810: df bd 03 f6  			 mov r14, r15	
    1814: d2 bd 5b f2  			 cmps r14, r2	wcz
    1818: 98 00 90 3d  		if_nc	 jmp #152
    181c: d1 a1 03 f6  			 mov r0, r1	
    1820: 2e b0 63 fd  			 calla r8	
    1824: dc df 5b f2  			 cmps r31, r12	wcz
    1828: 88 00 90 ad  		if_z	 jmp #136
    182c: 00 b2 5f f2  			 cmps r9, #0	wcz
    1830: 1c 00 90 ad  		if_z	 jmp #28
    1834: 0d de 5f f2  			 cmps r31, #13	wcz
    1838: ef c1 03 f6  			 mov r16, r31	
    183c: 14 00 90 5d  		if_nz	 jmp #20
    1840: d3 a1 03 f6  			 mov r0, r3	
    1844: 2e ae 63 fd  			 calla r7	
    1848: dd c1 03 f6  			 mov r16, r13	
    184c: 04 00 90 fd  			 jmp #4
    1850: ef c1 03 f6  			 mov r16, r31	
    1854: d6 a1 03 f6  			 mov r0, r6	
    1858: de a1 03 f1  			 add r0, r14	
    185c: d0 c1 43 fc  			 wrbyte r16, r0
    1860: de bf 03 f6  			 mov r15, r14	
    1864: 01 be 07 f1  			 add r15, #1	
    1868: 00 b2 5f f2  			 cmps r9, #0	wcz
    186c: 38 00 90 ad  		if_z	 jmp #56
    1870: 00 b6 5f f2  			 cmps r11, #0	wcz
    1874: e0 a1 03 f6  			 mov r0, r16	
    1878: 28 00 90 5d  		if_nz	 jmp #40
    187c: 01 bc 5f f2  			 cmps r14, #1	wcz
    1880: da bf 03 f6  			 mov r15, r10	
    1884: 20 00 90 cd  		if_c	 jmp #32
    1888: d4 a1 03 f6  			 mov r0, r4	
    188c: 2e ae 63 fd  			 calla r7	
    1890: d5 a1 03 f6  			 mov r0, r5	
    1894: 2e ae 63 fd  			 calla r7	
    1898: 01 bc 87 f1  			 sub r14, #1	
    189c: d4 a1 03 f6  			 mov r0, r4	
    18a0: de bf 03 f6  			 mov r15, r14	
    18a4: 2e ae 63 fd  			 calla r7	
    18a8: 0a c0 5f f2  			 cmps r16, #10	wcz
    18ac: 60 ff 9f 5d  		if_nz	 jmp #-160
    18b0: 04 00 90 fd  			 jmp #4
    18b4: de bf 03 f6  			 mov r15, r14	
    18b8: df df 03 f6  			 mov r31, r15	
    18bc: 28 20 64 fd  			 setq #16
    18c0: 5f a1 07 fb  			 rdlong r0, #351	
    18c4: 2e 00 64 fd  			 reta	

000018c8 <dummy>:
    18c8: 2e 00 64 fd  			 reta	

000018cc <_cnt>:
    18cc: 1a de 63 fd  			 getct r31	
    18d0: 2e 00 64 fd  			 reta	

000018d4 <_waitcnt>:
    18d4: 00 a0 67 fa  			 addct1 r0, #0
    18d8: 24 22 60 fd  			 waitct1	
    18dc: 2e 00 64 fd  			 reta	

000018e0 <_clkset>:
    18e0: 28 04 64 fd  			 setq #2
    18e4: 61 a1 67 fc  			 wrlong r0, #353
    18e8: f0 a4 07 f6  			 mov r2, #240	
    18ec: 00 a4 63 fd  			 hubset r2
    18f0: 18 a4 07 f6  			 mov r2, #24	
    18f4: d2 a1 63 fc  			 wrlong r0, r2
    18f8: 14 a4 07 f6  			 mov r2, #20	
    18fc: d2 a3 63 fc  			 wrlong r1, r2
    1900: 00 a0 63 fd  			 hubset r0
    1904: 86 01 00 ff  			 augs #390
    1908: 40 a3 07 f6  			 mov r1, #320	
    190c: 1f a2 63 fd  			 waitx r1
    1910: 03 a0 47 f5  			 or r0, #3	
    1914: 00 a0 63 fd  			 hubset r0
    1918: 28 04 64 fd  			 setq #2
    191c: 5f a1 07 fb  			 rdlong r0, #351	
    1920: 2e 00 64 fd  			 reta	

00001924 <_coginit>:
    1924: 28 04 64 fd  			 setq #2
    1928: 61 a1 67 fc  			 wrlong r0, #353
    192c: 28 a4 63 fd  			 setq r2
    1930: d1 a1 f3 fc  			 coginit r0, r1	wc
    1934: 6c a2 63 fd  			 wrc r1
    1938: ff ff 7f ff  			 augs #8388607
    193c: ff a5 07 f6  			 mov r2, #511	
    1940: 00 a2 5f f2  			 cmps r1, #0	wcz
    1944: d0 a1 03 a6  		if_z	 mov r0, r0	
    1948: d2 a1 03 56  		if_nz	 mov r0, r2	
    194c: d0 df 03 f6  			 mov r31, r0	
    1950: 28 04 64 fd  			 setq #2
    1954: 5f a1 07 fb  			 rdlong r0, #351	
    1958: 2e 00 64 fd  			 reta	

0000195c <cogstart>:
    195c: 28 02 64 fd  			 setq #1
    1960: 61 a1 67 fc  			 wrlong r0, #353
    1964: 61 a7 67 fc  			 wrlong r3, #353
    1968: d2 a7 03 f6  			 mov r3, r2	
    196c: 04 a6 07 f1  			 add r3, #4	
    1970: d3 a3 63 fc  			 wrlong r1, r3
    1974: d2 a1 63 fc  			 wrlong r0, r2
    1978: 10 a0 07 f6  			 mov r0, #16	
    197c: 00 00 00 ff  			 augs #0
    1980: 00 a3 07 f6  			 mov r1, #256	
    1984: 24 19 c0 fd  			 calla #6436
    1988: 5f a7 07 fb  			 rdlong r3, #351	
    198c: 28 02 64 fd  			 setq #1
    1990: 5f a1 07 fb  			 rdlong r0, #351	
    1994: 2e 00 64 fd  			 reta	

00001998 <_locknew>:
    1998: 04 de 63 fd  			 locknew r31	
    199c: 2e 00 64 fd  			 reta	

000019a0 <_lockret>:
    19a0: 05 a0 63 fd  			 lockret r0
    19a4: 2e 00 64 fd  			 reta	

000019a8 <_lock>:
    19a8: 61 a1 67 fc  			 wrlong r0, #353
    19ac: 04 f0 07 f1  			 add ptra, #4	
    19b0: f8 ed 03 f6  			 mov pa, ptra	
    19b4: 04 ec 87 f1  			 sub pa, #4	
    19b8: f6 a1 63 fc  			 wrlong r0, pa
    19bc: f8 ed 03 f6  			 mov pa, ptra	
    19c0: 04 ec 87 f1  			 sub pa, #4	
    19c4: f6 a1 03 fb  			 rdlong r0, pa	
    19c8: 06 a0 73 fd  			 locktry r0	wc
    19cc: ec ff 9f 3d  		if_nc	 jmp #-20
    19d0: 04 f0 87 f1  			 sub ptra, #4	
    19d4: 5f a1 07 fb  			 rdlong r0, #351	
    19d8: 2e 00 64 fd  			 reta	

000019dc <_locktry>:
    19dc: 61 a1 67 fc  			 wrlong r0, #353
    19e0: 04 f0 07 f1  			 add ptra, #4	
    19e4: f8 ed 03 f6  			 mov pa, ptra	
    19e8: 04 ec 87 f1  			 sub pa, #4	
    19ec: f6 a1 63 fc  			 wrlong r0, pa
    19f0: f8 ed 03 f6  			 mov pa, ptra	
    19f4: 04 ec 87 f1  			 sub pa, #4	
    19f8: f6 a1 03 fb  			 rdlong r0, pa	
    19fc: 06 a0 73 fd  			 locktry r0	wc
    1a00: 6c de 63 fd  			 wrc r31
    1a04: 04 f0 87 f1  			 sub ptra, #4	
    1a08: 5f a1 07 fb  			 rdlong r0, #351	
    1a0c: 2e 00 64 fd  			 reta	

00001a10 <_unlock>:
    1a10: 61 a1 67 fc  			 wrlong r0, #353
    1a14: 04 f0 07 f1  			 add ptra, #4	
    1a18: f8 ed 03 f6  			 mov pa, ptra	
    1a1c: 04 ec 87 f1  			 sub pa, #4	
    1a20: f6 a1 63 fc  			 wrlong r0, pa
    1a24: f8 ed 03 f6  			 mov pa, ptra	
    1a28: 04 ec 87 f1  			 sub pa, #4	
    1a2c: f6 a1 03 fb  			 rdlong r0, pa	
    1a30: 07 a0 63 fd  			 lockrel r0	
    1a34: 04 f0 87 f1  			 sub ptra, #4	
    1a38: 5f a1 07 fb  			 rdlong r0, #351	
    1a3c: 2e 00 64 fd  			 reta	

00001a40 <_rev>:
    1a40: 69 a0 63 fd  			 rev r0
    1a44: d0 df 03 f6  			 mov r31, r0	
    1a48: 2e 00 64 fd  			 reta	

00001a4c <_uart_init>:
    1a4c: 28 06 64 fd  			 setq #3
    1a50: 61 a3 67 fc  			 wrlong r1, #353
    1a54: 10 00 00 ff  			 augs #16
    1a58: f4 a7 07 f6  			 mov r3, #500	
    1a5c: d3 a3 63 fc  			 wrlong r1, r3
    1a60: 10 00 00 ff  			 augs #16
    1a64: f0 a7 07 f6  			 mov r3, #496	
    1a68: d3 a1 63 fc  			 wrlong r0, r3
    1a6c: 40 a0 63 fd  			 dirl r0	
    1a70: 40 a2 63 fd  			 dirl r1	
    1a74: 1c a6 07 f6  			 mov r3, #28	
    1a78: d3 a5 63 fc  			 wrlong r2, r3
    1a7c: 14 a6 07 f6  			 mov r3, #20	
    1a80: d3 a7 03 fb  			 rdlong r3, r3	
    1a84: d2 a7 13 fd  			 qdiv r3, r2
    1a88: 18 a4 63 fd  			 getqx r2	
    1a8c: 10 00 00 ff  			 augs #16
    1a90: f8 a7 07 f6  			 mov r3, #504	
    1a94: d3 a5 63 fc  			 wrlong r2, r3
    1a98: 7c a8 07 f6  			 mov r4, #124	
    1a9c: d1 a9 03 fc  			 wrpin r4, r1
    1aa0: 10 a4 67 f0  			 shl r2, #16	
    1aa4: 07 a4 47 f5  			 or r2, #7	
    1aa8: d1 a5 13 fc  			 wxpin r2, r1
    1aac: 41 a2 63 fd  			 dirh r1	
    1ab0: 3e a2 07 f6  			 mov r1, #62	
    1ab4: d0 a3 03 fc  			 wrpin r1, r0
    1ab8: d0 a5 13 fc  			 wxpin r2, r0
    1abc: 41 a0 63 fd  			 dirh r0	
    1ac0: d3 df 03 fb  			 rdlong r31, r3	
    1ac4: 28 06 64 fd  			 setq #3
    1ac8: 5f a3 07 fb  			 rdlong r1, #351	
    1acc: 2e 00 64 fd  			 reta	

00001ad0 <_uart_putc>:
    1ad0: 28 02 64 fd  			 setq #1
    1ad4: 61 a1 67 fc  			 wrlong r0, #353
    1ad8: 10 00 00 ff  			 augs #16
    1adc: f4 a3 07 f6  			 mov r1, #500	
    1ae0: d1 a3 03 fb  			 rdlong r1, r1	
    1ae4: d1 a1 23 fc  			 wypin r0, r1
    1ae8: 10 00 00 ff  			 augs #16
    1aec: f8 a1 07 f6  			 mov r0, #504	
    1af0: d0 a1 03 fb  			 rdlong r0, r0	
    1af4: 0a a2 07 f6  			 mov r1, #10	
    1af8: d1 a1 03 fd  			 qmul r0, r1
    1afc: 18 a0 63 fd  			 getqx r0	
    1b00: 1f a0 63 fd  			 waitx r0
    1b04: 28 02 64 fd  			 setq #1
    1b08: 5f a1 07 fb  			 rdlong r0, #351	
    1b0c: 2e 00 64 fd  			 reta	

00001b10 <_uart_getc>:
    1b10: 28 02 64 fd  			 setq #1
    1b14: 61 a1 67 fc  			 wrlong r0, #353
    1b18: 10 00 00 ff  			 augs #16
    1b1c: f0 a1 07 f6  			 mov r0, #496	
    1b20: d0 a1 03 fb  			 rdlong r0, r0	
    1b24: 40 a0 73 fd  			 testp r0	wc
    1b28: 6c a2 63 fd  			 wrc r1
    1b2c: 00 a2 5f f2  			 cmps r1, #0	wcz
    1b30: 0c 00 90 ad  		if_z	 jmp #12
    1b34: d0 a1 8b fa  			 rdpin r0, r0	
    1b38: 18 a0 47 f0  			 shr r0, #24	
    1b3c: 04 00 90 fd  			 jmp #4
    1b40: ff a0 07 f6  			 mov r0, #255	
    1b44: 07 a0 67 f7  			 signx r0, #7	
    1b48: d0 df 03 f6  			 mov r31, r0	
    1b4c: 28 02 64 fd  			 setq #1
    1b50: 5f a1 07 fb  			 rdlong r0, #351	
    1b54: 2e 00 64 fd  			 reta	

00001b58 <__cxa_pure_virtual>:
    1b58: fc ff 9f fd  			 jmp #-4

00001b5c <_cstd_init>:
    1b5c: 61 a1 67 fc  			 wrlong r0, #353
    1b60: 98 19 c0 fd  			 calla #6552
    1b64: 10 00 00 ff  			 augs #16
    1b68: fc a1 07 f6  			 mov r0, #508	
    1b6c: d0 df 63 fc  			 wrlong r31, r0
    1b70: 98 19 c0 fd  			 calla #6552
    1b74: 0f 00 00 ff  			 augs #15
    1b78: 58 a1 07 f6  			 mov r0, #344	
    1b7c: d0 df 63 fc  			 wrlong r31, r0
    1b80: d8 13 c0 fd  			 calla #5080
    1b84: 5f a1 07 fb  			 rdlong r0, #351	
    1b88: 2e 00 64 fd  			 reta	

00001b8c <_malloc>:
    1b8c: 28 0c 64 fd  			 setq #6
    1b90: 61 a1 67 fc  			 wrlong r0, #353
    1b94: d0 a5 03 f6  			 mov r2, r0	
    1b98: 10 00 00 ff  			 augs #16
    1b9c: fc ab 07 f6  			 mov r5, #508	
    1ba0: d5 a1 03 fb  			 rdlong r0, r5	
    1ba4: a8 19 c0 fd  			 calla #6568
    1ba8: 07 a2 07 f1  			 add r1, #7	
    1bac: 03 a2 47 f0  			 shr r1, #3	
    1bb0: d1 a7 03 f6  			 mov r3, r1	
    1bb4: 01 a6 07 f1  			 add r3, #1	
    1bb8: d2 a1 03 f6  			 mov r0, r2	
    1bbc: 04 a0 07 f1  			 add r0, #4	
    1bc0: d0 a9 03 fb  			 rdlong r4, r0	
    1bc4: 00 a8 5f f2  			 cmps r4, #0	wcz
    1bc8: 5c 00 90 ad  		if_z	 jmp #92
    1bcc: d4 a1 03 f6  			 mov r0, r4	
    1bd0: 04 a0 07 f1  			 add r0, #4	
    1bd4: d0 a1 03 fb  			 rdlong r0, r0	
    1bd8: d1 a1 1b f2  			 cmp r0, r1	wcz
    1bdc: 04 00 90 1d  	if_nc_and_nz	 jmp #4
    1be0: 08 00 90 fd  			 jmp #8
    1be4: 04 a4 07 f1  			 add r2, #4	
    1be8: 28 00 90 fd  			 jmp #40
    1bec: d4 ad 03 f6  			 mov r6, r4	
    1bf0: d6 a9 03 fb  			 rdlong r4, r6	
    1bf4: 00 a8 5f f2  			 cmps r4, #0	wcz
    1bf8: 2c 00 90 ad  		if_z	 jmp #44
    1bfc: d4 a1 03 f6  			 mov r0, r4	
    1c00: 04 a0 07 f1  			 add r0, #4	
    1c04: d0 a1 03 fb  			 rdlong r0, r0	
    1c08: d1 a1 1b f2  			 cmp r0, r1	wcz
    1c0c: dc ff 9f ed  	if_c_or_z	 jmp #-36
    1c10: d6 a5 03 f6  			 mov r2, r6	
    1c14: d3 a1 5b f2  			 cmps r0, r3	wcz
    1c18: 3c 00 90 5d  		if_nz	 jmp #60
    1c1c: d4 a1 03 fb  			 rdlong r0, r4	
    1c20: d2 a1 63 fc  			 wrlong r0, r2
    1c24: 54 00 90 fd  			 jmp #84
    1c28: d2 a3 03 fb  			 rdlong r1, r2	
    1c2c: d3 a1 03 f6  			 mov r0, r3	
    1c30: 03 a0 67 f0  			 shl r0, #3	
    1c34: 2e a2 63 fd  			 calla r1	
    1c38: ef a9 03 f6  			 mov r4, r31	
    1c3c: d5 a1 03 fb  			 rdlong r0, r5	
    1c40: 10 1a c0 fd  			 calla #6672
    1c44: 00 a8 5f f2  			 cmps r4, #0	wcz
    1c48: 04 00 90 ad  		if_z	 jmp #4
    1c4c: 34 00 90 fd  			 jmp #52
    1c50: 00 a8 07 f6  			 mov r4, #0	
    1c54: 48 00 90 fd  			 jmp #72
    1c58: d3 a1 83 f1  			 sub r0, r3	
    1c5c: d4 a3 03 f6  			 mov r1, r4	
    1c60: 04 a2 07 f1  			 add r1, #4	
    1c64: d1 a1 63 fc  			 wrlong r0, r1
    1c68: 03 a0 67 f0  			 shl r0, #3	
    1c6c: d0 a9 03 f1  			 add r4, r0	
    1c70: d4 a1 03 f6  			 mov r0, r4	
    1c74: 04 a0 07 f1  			 add r0, #4	
    1c78: d0 a7 63 fc  			 wrlong r3, r0
    1c7c: d5 a1 03 fb  			 rdlong r0, r5	
    1c80: 10 1a c0 fd  			 calla #6672
    1c84: d4 a1 03 f6  			 mov r0, r4	
    1c88: 04 a0 07 f1  			 add r0, #4	
    1c8c: d0 a7 63 fc  			 wrlong r3, r0
    1c90: 08 05 00 ff  			 augs #1288
    1c94: 0c a1 07 f6  			 mov r0, #268	
    1c98: d4 a1 63 fc  			 wrlong r0, r4
    1c9c: 08 a8 07 f1  			 add r4, #8	
    1ca0: d4 df 03 f6  			 mov r31, r4	
    1ca4: 28 0c 64 fd  			 setq #6
    1ca8: 5f a1 07 fb  			 rdlong r0, #351	
    1cac: 2e 00 64 fd  			 reta	

00001cb0 <malloc>:
    1cb0: 28 02 64 fd  			 setq #1
    1cb4: 61 a1 67 fc  			 wrlong r0, #353
    1cb8: d0 a3 03 f6  			 mov r1, r0	
    1cbc: 0f 00 00 ff  			 augs #15
    1cc0: 4c a1 07 f6  			 mov r0, #332	
    1cc4: 8c 1b c0 fd  			 calla #7052
    1cc8: 28 02 64 fd  			 setq #1
    1ccc: 5f a1 07 fb  			 rdlong r0, #351	
    1cd0: 2e 00 64 fd  			 reta	

00001cd4 <free>:
    1cd4: 28 14 64 fd  			 setq #10
    1cd8: 61 a1 67 fc  			 wrlong r0, #353
    1cdc: d0 a3 03 f6  			 mov r1, r0	
    1ce0: d1 a7 03 f6  			 mov r3, r1	
    1ce4: f8 a7 07 f1  			 add r3, #504	
    1ce8: d3 a1 03 fb  			 rdlong r0, r3	
    1cec: 08 05 00 ff  			 augs #1288
    1cf0: 0c a5 07 f6  			 mov r2, #268	
    1cf4: d2 a1 5b f2  			 cmps r0, r2	wcz
    1cf8: 04 00 90 5d  		if_nz	 jmp #4
    1cfc: 0c 00 90 fd  			 jmp #12
    1d00: 28 14 64 fd  			 setq #10
    1d04: 5f a1 07 fb  			 rdlong r0, #351	
    1d08: 2e 00 64 fd  			 reta	
    1d0c: d1 a5 03 f6  			 mov r2, r1	
    1d10: 08 a4 87 f1  			 sub r2, #8	
    1d14: d2 01 68 fc  			 wrlong #0, r2
    1d18: 10 00 00 ff  			 augs #16
    1d1c: fc a9 07 f6  			 mov r4, #508	
    1d20: d4 a1 03 fb  			 rdlong r0, r4	
    1d24: a8 19 c0 fd  			 calla #6568
    1d28: 0f 00 00 ff  			 augs #15
    1d2c: 4c ad 07 f6  			 mov r6, #332	
    1d30: d6 ab 03 f6  			 mov r5, r6	
    1d34: 04 aa 07 f1  			 add r5, #4	
    1d38: d5 a1 03 fb  			 rdlong r0, r5	
    1d3c: 00 a0 5f f2  			 cmps r0, #0	wcz
    1d40: 80 00 90 ad  		if_z	 jmp #128
    1d44: fc a3 07 f1  			 add r1, #508	
    1d48: 00 ac 07 f6  			 mov r6, #0	
    1d4c: d0 af 03 f6  			 mov r7, r0	
    1d50: 04 ae 07 f1  			 add r7, #4	
    1d54: d7 b1 03 fb  			 rdlong r8, r7	
    1d58: d8 b3 03 f6  			 mov r9, r8	
    1d5c: 03 b2 67 f0  			 shl r9, #3	
    1d60: d0 b5 03 f6  			 mov r10, r0	
    1d64: d9 b5 03 f1  			 add r10, r9	
    1d68: d2 b5 5b f2  			 cmps r10, r2	wcz
    1d6c: 04 00 90 5d  		if_nz	 jmp #4
    1d70: 5c 00 90 fd  			 jmp #92
    1d74: d1 af 03 fb  			 rdlong r7, r1	
    1d78: d7 b3 03 f6  			 mov r9, r7	
    1d7c: 03 b2 67 f0  			 shl r9, #3	
    1d80: d2 b5 03 f6  			 mov r10, r2	
    1d84: d9 b5 03 f1  			 add r10, r9	
    1d88: d0 b5 5b f2  			 cmps r10, r0	wcz
    1d8c: 04 00 90 5d  		if_nz	 jmp #4
    1d90: 88 00 90 fd  			 jmp #136
    1d94: d2 a1 1b f2  			 cmp r0, r2	wcz
    1d98: 14 00 90 1d  	if_nc_and_nz	 jmp #20
    1d9c: d0 ab 03 f6  			 mov r5, r0	
    1da0: d0 a1 03 fb  			 rdlong r0, r0	
    1da4: 00 a0 5f f2  			 cmps r0, #0	wcz
    1da8: a0 ff 9f 5d  		if_nz	 jmp #-96
    1dac: d6 a1 03 f6  			 mov r0, r6	
    1db0: d2 a1 63 fc  			 wrlong r0, r2
    1db4: d5 a5 63 fc  			 wrlong r2, r5
    1db8: d4 a1 03 fb  			 rdlong r0, r4	
    1dbc: 10 1a c0 fd  			 calla #6672
    1dc0: 3c ff 9f fd  			 jmp #-196
    1dc4: 04 ac 07 f1  			 add r6, #4	
    1dc8: d6 a5 63 fc  			 wrlong r2, r6
    1dcc: 30 ff 9f fd  			 jmp #-208
    1dd0: d1 a3 03 fb  			 rdlong r1, r1	
    1dd4: d8 a3 03 f1  			 add r1, r8	
    1dd8: d7 a3 63 fc  			 wrlong r1, r7
    1ddc: d1 a7 03 f6  			 mov r3, r1	
    1de0: 03 a6 67 f0  			 shl r3, #3	
    1de4: d0 a5 03 f6  			 mov r2, r0	
    1de8: d3 a5 03 f1  			 add r2, r3	
    1dec: d0 a7 03 fb  			 rdlong r3, r0	
    1df0: d3 a5 5b f2  			 cmps r2, r3	wcz
    1df4: 08 ff 9f 5d  		if_nz	 jmp #-248
    1df8: d2 a7 03 f6  			 mov r3, r2	
    1dfc: 04 a6 07 f1  			 add r3, #4	
    1e00: d3 a7 03 fb  			 rdlong r3, r3	
    1e04: d1 a7 03 f1  			 add r3, r1	
    1e08: d7 a7 63 fc  			 wrlong r3, r7
    1e0c: d2 a3 03 fb  			 rdlong r1, r2	
    1e10: d0 a3 63 fc  			 wrlong r1, r0
    1e14: d2 01 68 fc  			 wrlong #0, r2
    1e18: e4 fe 9f fd  			 jmp #-284
    1e1c: d5 a5 63 fc  			 wrlong r2, r5
    1e20: d8 af 03 f1  			 add r7, r8	
    1e24: d0 a1 03 fb  			 rdlong r0, r0	
    1e28: d1 af 63 fc  			 wrlong r7, r1
    1e2c: d3 a1 63 fc  			 wrlong r0, r3
    1e30: cc fe 9f fd  			 jmp #-308

00001e34 <sbrk>:
    1e34: 28 04 64 fd  			 setq #2
    1e38: 61 a3 67 fc  			 wrlong r1, #353
    1e3c: 0f 00 00 ff  			 augs #15
    1e40: 54 a3 07 f6  			 mov r1, #340	
    1e44: d1 a5 03 fb  			 rdlong r2, r1	
    1e48: d2 a7 03 f6  			 mov r3, r2	
    1e4c: d0 a7 03 f1  			 add r3, r0	
    1e50: d1 a7 63 fc  			 wrlong r3, r1
    1e54: d2 df 03 f6  			 mov r31, r2	
    1e58: 28 04 64 fd  			 setq #2
    1e5c: 5f a3 07 fb  			 rdlong r1, #351	
    1e60: 2e 00 64 fd  			 reta	
