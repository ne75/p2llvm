# P2 toolchain use and support

Use this repository's P2-enabled Clang, LLD and runtime together. The cleanup
series remains on LLVM 14; upgrading LLVM is a separate migration, not a
prerequisite for using the cleaned backend.

## Selecting an SDK

An installed SDK has this layout:

```text
sdk/bin/clang, clang++, ld.lld, llvm-*
sdk/libc/include/       sdk/libc/lib/libc.a
sdk/libp2/include/      sdk/libp2/lib/libp2.a, libp2db.a
sdk/p2.ld              sdk/p2_debug.ld
```

The default SDK is the directory above the compiler's `bin/`. `--sysroot` selects
another SDK for headers, libraries and the default linker script together.
There is no fallback to an unrelated `/opt` installation. A development compiler
build does not itself supply installed runtime headers and archives.

```sh
/path/to/p2/bin/clang --target=p2 --sysroot=/path/to/sdk -O2 -c main.c -o main.o
/path/to/p2/bin/clang --target=p2 --sysroot=/path/to/sdk main.o -o main.elf
```

Use `-###` to inspect the selected frontend and linker commands. `--target=p2`
selects the backend explicitly; a host Clang cannot substitute for a P2-enabled
build. The driver supplies `-fno-jump-tables` and `-fno-rtti`. For C++, also use
`-fno-exceptions` unless you supply and validate a separate exception runtime.

## Link behavior

Normal links use LLD, the SDK's `p2.ld`, and `libc.a` plus `libp2.a` with
whole-archive retention. Startup and runtime LUT members depend on that retention.
The whole-archive region ends after the default archives. `-mp2db` additionally
selects `libp2db.a` and `p2_debug.ld`.

| Option | Behavior |
|---|---|
| `-L/path` | Search this directory before SDK library directories |
| `-T script`, `-Wl,-T,script`, `-Xlinker --script=script` | Replace the default layout script |
| `-nodefaultlibs` | Omit default archives; keep the default script unless overridden |
| `-nostdlib` | Omit default archives and the default script |
| `-r` | Produce a relocatable link, with no default archives or script |
| `-nostartfiles` | Diagnosed when default archives are enabled; startup cannot yet be separated from the whole runtime |

`-nostdlib` is the custom-startup path: supply your own archives, entry and layout.
Explicit `-L` overrides are intentional; keep headers and libraries compatible.
The stock linker scripts enforce a 512-long LUT budget. Reducing ordinary HUB
sections with `--gc-sections` does not discard retained LUT members.

## Supported and bounded behavior

The executable suite covers integer and floating arithmetic, fixed local frames,
argument/return lowering, aggregates, variadic calls, function pointers, inline
assembly, and selected runtime/multicog behavior. C++ cases include virtual
calls, constructors/destructors, default arguments, static members and array
new/delete. This is tested language behavior, not a complete hosted C++ library.
The SDK does not ship a full C++ standard library or an exception unwinder.

Dynamic `alloca`, VLAs, LLVM stacksave/stackrestore, and TLS are unsupported.
Over-aligned fixed locals are supported, with known alignment overhead. See the
[ABI](Propeller%202%20ABI.md) for stack, return-register and COG entry contracts,
and [inline assembly guide](P2%20Inline%20Assembly.md) for operand declarations.

`cogtext`/`cogcache` alone do not load or group resident functions. Direct calls
into the initialized runtime LUT have relocation support. General indirect LUT
calls and multi-function COG residency require future work. Some peripheral
instructions lack executable fixtures; the [coverage guide](../tests/hardware/README.md)
distinguishes encoding checks from observed hardware behavior.

## Making double use explicit

P2 double operations use software helpers. Use `f` literals and float interfaces
when single precision is intended. To catch common implicit conversions:

```sh
-Werror=double-promotion -Werror=implicit-float-conversion
```

For example, `float + 1.0` and implicit `long long` to `double` conversion are
diagnosed; `1.0f` or an explicit cast communicates intent. These flags are not a
ban on all implicit double use: exact `int` to `double` conversions can pass.
An explicit cast also does not guarantee cheap generated code.

Omitting a conversion helper from an archive can catch some accidental use at
link time, but cannot distinguish explicit and implicit source conversions and
can be bypassed by optimization. A comprehensive implicit-double diagnostic and
an opt-in runtime policy remain [future work](../development/future-improvements.md).
Do not infer permission to use doubles from availability of a runtime helper.

## Validation

See the [testing guide](../tests/README.md) for host, build and hardware runs.
The SDK selection regression performs real links with temporary synthetic SDKs;
it does not access hardware. Executable semantics and cycle measurements require
the hardware fixtures. Development checkpoints belong under `development/`.
Private application checks must use temporary directories outside this public
repository; keep only generic reproductions here.
