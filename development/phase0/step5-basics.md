# Basic hardware comparison

Cycles are median batch cycles / 64 calls (five samples). They include the indirect call, loop and timer overhead; the empty case shows that cost. Do not interpret these as isolated instruction latency. Lower is faster.

Frames are compiler-reported bytes for each kernel, including its fixed frame and register saves. They exclude nested calls and do not measure peak stack use. Memory fills include write completion in both profiles.

Requested clock: 160000000 Hz. Runtime libraries use Oz;
the optimization rows refer to the kernels and harness.

## O0: PASS / PASS

Firmware: 31236 → 35324 bytes.

| Case | Production cycles/call | Cleanup cycles/call | Change | Frame bytes P → C |
|---|---:|---:|---:|---:|
| empty | 275.1 | 355.6 | +29.3% | 12 → 16 |
| alu | 3915.1 | 4811.6 | +22.9% | 20 → 28 |
| div32 | 523.1 | 619.6 | +18.4% | 16 → 20 |
| mul64 | 795.1 | 875.6 | +10.1% | 40 → 64 |
| div64 | 1283.0 | 1427.6 | +11.3% | 64 → 104 |
| single | 2002.8 | 2091.5 | +4.4% | 20 → 28 |
| minimum | 543.5 | 659.6 | +21.4% | 28 → 40 |
| copy64 | 539.1 | 635.6 | +17.9% | 24 → 32 |
| copy1024 | 1011.1 | 1123.6 | +11.1% | 24 → 32 |
| copy_unaligned | 555.1 | 659.6 | +18.8% | 24 → 32 |
| fill8 | 563.1 | 699.6 | +24.2% | 24 → 32 |
| fill64 | 915.1 | 1027.6 | +12.3% | 24 → 32 |
| fill1024 | 6675.0 | 6771.6 | +1.4% | 24 → 32 |
| aggregate | 939.0 | 1091.6 | +16.3% | 68 → 80 |
| virtual | 679.0 | 839.6 | +23.7% | 24 → 32 |
| stack | 4444.1 | 6076.6 | +36.7% | 88 → 100 |

## O2: PASS / PASS

Firmware: 31444 → 32708 bytes.

| Case | Production cycles/call | Cleanup cycles/call | Change | Frame bytes P → C |
|---|---:|---:|---:|---:|
| empty | 103.6 | 103.7 | +0.0% | 4 → 4 |
| alu | 1303.6 | 1303.7 | +0.0% | 16 → 16 |
| div32 | 367.6 | 375.7 | +2.2% | 12 → 12 |
| mul64 | 559.6 | 559.7 | +0.0% | 28 → 28 |
| div64 | 863.6 | 887.7 | +2.8% | 44 → 44 |
| single | 1823.4 | 1791.5 | -1.7% | 12 → 12 |
| minimum | 263.6 | 263.7 | +0.0% | 16 → 16 |
| copy64 | 383.6 | 399.7 | +4.2% | 24 → 24 |
| copy1024 | 871.6 | 879.7 | +0.9% | 24 → 24 |
| copy_unaligned | 375.6 | 391.7 | +4.3% | 24 → 24 |
| fill8 | 399.6 | 415.7 | +4.0% | 20 → 20 |
| fill64 | 711.6 | 743.7 | +4.5% | 20 → 20 |
| fill1024 | 6487.6 | 6503.7 | +0.2% | 20 → 20 |
| aggregate | 647.6 | 647.7 | +0.0% | 52 → 60 |
| virtual | 399.6 | 395.7 | -1.0% | 20 → 20 |
| stack | 864.6 | 864.7 | +0.0% | 84 → 88 |

## Oz: PASS / PASS

Firmware: 29972 → 31236 bytes.

| Case | Production cycles/call | Cleanup cycles/call | Change | Frame bytes P → C |
|---|---:|---:|---:|---:|
| empty | 112.3 | 128.3 | +14.2% | 4 → 4 |
| alu | 1968.3 | 1976.3 | +0.4% | 20 → 20 |
| div32 | 384.3 | 392.3 | +2.1% | 12 → 12 |
| mul64 | 568.3 | 584.3 | +2.8% | 28 → 28 |
| div64 | 888.3 | 920.3 | +3.6% | 44 → 44 |
| single | 1840.1 | 1816.2 | -1.3% | 12 → 12 |
| minimum | 280.3 | 296.3 | +5.7% | 16 → 16 |
| copy64 | 400.3 | 416.3 | +4.0% | 24 → 24 |
| copy1024 | 888.3 | 904.3 | +1.8% | 24 → 24 |
| copy_unaligned | 392.3 | 416.3 | +6.1% | 24 → 24 |
| fill8 | 392.3 | 424.3 | +8.2% | 20 → 20 |
| fill64 | 728.3 | 760.3 | +4.4% | 20 → 20 |
| fill1024 | 6480.3 | 6512.3 | +0.5% | 20 → 20 |
| aggregate | 640.3 | 672.3 | +5.0% | 52 → 60 |
| virtual | 416.3 | 416.3 | +0.0% | 20 → 20 |
| stack | 2433.3 | 2458.3 | +1.0% | 84 → 88 |
