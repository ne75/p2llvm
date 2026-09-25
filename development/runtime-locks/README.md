# Stream hardware-lock ownership audit

25 September 2026

## Findings

Closing or replacing an open stream now returns its hardware lock. Failed opens
also return any lock acquired by the driver. Reopening three streams 32 times
on P2 hardware leaves 11 locks available while those streams are open and 14
after closing them. The old lifecycle code exhausted the native model's lock
pool under the same regression. `_locknew()` and its carry-based failure
reporting are unchanged.

The P2 has 16 hardware locks, numbered 0 through 15. Allocation reserves an ID;
acquiring ownership with LOCKTRY prevents another cog from taking that lock.
LOCKREL releases ownership, whereas LOCKRET returns the ID to the allocation
pool. These fixtures use the normal libp2 startup, which reserves two locks,
and open their own streams explicitly. They do not assume which IDs are free.

## Ownership contract and driver audit

`FILE._lock` is either an exclusively allocated per-stream ID or `-1` for no
lock. Stdio initializes the field before the driver's open callback. On success,
stdio retains that allocation through flushing and the driver's close callback,
then returns it and clears the stream. On failure, the driver must undo its own
partial heap/device setup; stdio returns the stream lock and frees the FILE slot.
The close callback is not called on a partially initialized failed open.

Drivers must not share their stream lock ID or return it themselves. This
contract is also documented beside the field in `libc/include/stdio.h`; the
FILE layout and hardware-lock API signatures are unchanged.

| Driver | Stream lock allocation | Close and failure handling |
|---|---|---|
| SimpleSerial | One ID per FILE | Allocation failure returns ENOLCK before UART setup; stdio returns the ID on close. |
| FdSerial | One ID per FILE, even when several FILEs share a device | Allocation failure occurs before heap/cog acquisition or increasing the device user count. Heap/cog failures unwind device resources; stdio returns the ID. Last-user teardown precedes lock return. |
| Memory | None | Keeps the invalid sentinel, including stack streams used for string formatting. Its close callback still writes the terminating byte. |
| SD | None | Keeps the invalid sentinel. Existing heap/FatFs cleanup remains the driver's responsibility; inspection found no hardware-lock allocation or return. |
| Legacy null driver | None; not in the current driver build | Opening and closing must retain the invalid sentinel. The generic lock-free fixture covers this ownership contract. |

The inactive `serial_exit.c` helper is also outside the current driver build.
It opens a temporary SimpleSerial stream and stops its cog without closing it.
Enabling this legacy exit path would require handling open failure and closing
that stream before stopping; it is recorded as follow-up work.

Invalid closes (NULL, an unused slot, or a driver pointer without open flags)
return EOF/EBADF without returning any lock. A repeated close cannot return an
ID that another stream has since allocated. Flush and driver-close errors still
perform cleanup; a flush failure is now also propagated as EOF.

`_unlock()` now ignores invalid IDs, matching `_lock()`. Otherwise flushing a
lock-free stream could pass `-1` to LOCKREL and release lock 15, because the
instruction selects the lock using the low four bits. The hardware fixture
holds every available lock and uses a second cog to detect an unintended release.
An owning cog's LOCKTRY is reentrant; its LOCKREL releases before reporting
status, so neither is an appropriate held-lock assertion from that same cog.
See the Parallax Propeller 2 Documentation v35, Rev B/C, pages 72–73.

## Related FdSerial teardown repair

`_coginit()` returns a raw hardware cog ID, but FdSerial's stop routine expects
an encoded handle: zero means stopped, and a running handle is the cog ID plus
one. Start now stores and returns that encoding. This accepts cog zero and
prevents stopping the cog immediately below the intended one. Direct callers
of `_FdSerial_start()` must treat its result as an encoded handle.

The isolated driver regression supplies raw cog IDs 0, 3 and 7, checks that
three streams share one device, repeatedly reopens those streams, and verifies
that only the final close stops and frees the device. It reported five cog
errors before this repair and zero afterward. Cog operations in this particular
fixture are mocked, so a faulty stop cannot stop the test's own cog.

## Validation and limits

| Check | Result |
|---|---|
| Native stdio lifecycle, O0/O2/Os | 3 PASS; real stdio sources with a native lock model |
| Integration suite | 29 PASS |
| Runner/protocol/oracle unit tests | 14 PASS |
| Fresh isolated libp2 and libc builds | PASS |
| `runtime-stdio-locks`, O0/O2/Os | 3 hardware PASS |
| `runtime-serial-locks`, O0/O2/Os | 3 hardware PASS |
| Existing `lock-pool` and `cog-and-locks`, O0/O2/Os | 6 hardware PASS |

The lifecycle fixture reopens read-only stdin and writable stdout/stderr, checks
flush/close ordering, and exercises failed opens before and after allocation,
lock exhaustion, close/flush errors, a missing replacement driver, invalid modes,
invalid/repeated closes, and a real memory stream. The serial fixture executes
the real SimpleSerial/FdSerial callbacks with real hardware locks; UART setup,
heap allocation, and cog operations are replaced with controlled test hooks.
It injects heap and cog-start failures and tests shared-device ownership.

All 12 final hardware runs completed without timeouts or retries on P2 version G.
The lifecycle contention probe and the existing cog test use real secondary
cogs. No SD media test or end-to-end FdSerial UART/worker validation is claimed.
The installed SDK was not modified.

The [checkpoint](checkpoint.json) identifies the compiler, archives, fixture
sources and each loaded image. Its local evidence archive contains observations,
command logs, images, build logs and integration results. It contains only generic
compiler/runtime tests. No private application was used for this change.

## Reproducing the checks

Build an isolated runtime with the cleanup compiler, then run the two new cases.
Select a dedicated bench device; the hardware command resets it and loads RAM.
The loader may select the serial port automatically when `--port` is omitted.

```sh
cmake -S libp2 -B build/stdio-lock-runtime -G Ninja -Dllvm="$PWD/build/phase0-llvm/bin"
cmake --build build/stdio-lock-runtime --target p2
python3 tests/run.py --suite integration
python3 -u tests/hardware/run.py --mode hardware --runtime-dir build/stdio-lock-runtime --loader /opt/p2llvm/bin/loadp2 --reset RTS --baud 2000000 --fifo 10000 --timeout 30 --case runtime-stdio-locks --case runtime-serial-locks
```

Use `--mode build` for a compilation-only check. Successful compilation is
reported as BUILT_NOT_RUN and does not establish hardware correctness.

## Review units and follow-up

| Commit | Focus |
|---|---|
| `380ac50` | Central stream-lock lifecycle, invalid unlock guard, shared native/hardware regression |
| `34339eb` | Early serial allocation failure, partial-open cleanup tests |
| `d655a7f` | FdSerial cog-handle encoding and shared-device teardown regression |

An unrelated backend issue appeared when the optimizer narrowed a test-control
global to i1: some global boolean loads fail instruction selection at O2/Os.
The serial fixture uses volatile full-width control fields to keep that issue
separate from lock ownership. A small public reproducer and the inactive legacy
exit-path issue are recorded in [future improvements](../future-improvements.md).
