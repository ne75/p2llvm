# Feature Log
This is a quick reference for the various features being added as a running list, mostly to track what has been done and what still needs to be done.

## Backend (llvm)
- P2 target exists
- llvm IR programs can be compiled end encoded into an .o binary, or assembly can be printed. Assembly is printed in llvm's format and doesn't convert directly into a compilable Spin2 PASM block, but will likely work with llvm's assembler.
- For llvm instructions that don't map nicely to propeller, either pseudo instructions are used that are later expanded to a set of real instructions, or a libcall is generated to call a function in the standard propeller library
- Assembly parser for inline assembly exists. It doesn't yet support conditional flags (`if_z`, etc) or effect flags (`wz`, etc).

### TODO:
- Finish the Propeller instruction set to allow for inline assembly for any instructions that don't have C operations.
- expand libcalls for more complex operations (like atomic operations).

## Frontend (clang)
- P2 target exists and very simple job using the backend to create .o binaries for linking will run when compiling with `-c`.
- named registers are added for the 16 special registers, so statements like `OUTA |= 1 << pin` are possible (for compatibility with propgcc). however, this is usually slower than using the macros `outh()` and `outl()`, but that choice is up to the developer.

### TODO:
- create the linker job directly so no need to call ld.lld separetely for linking. Also add the libc and libp2 libraries one they have an installation parallel to the compiler binaries.

## Linker (lld)
- relocation is setup for various things needed by the backend. Typically this is either a 20 bit relocation for `call`s, 20 bit relocation for moving a large immediate with `augs/d` before it, or a 9 bit relocation for a libcall/cog based address.

### TODO:
- more generic relocation for cog-based data.

## Propeller Standard Libarary
- have re-usable startup code to start new cogs
- have a temporary `printf()` for debugging until the C library is finished.
- have basic uart setup implementation

### TODO:
- allow for a linker-generated stack location for cog0.
- implement easy access/setup for all instruction features.

## C Standard Library
- doesn't really exist yet.

### TODO:
- all of it.