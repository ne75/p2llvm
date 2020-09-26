# Feature Log

This is a quick reference for features that still needed to fully complete the toolchain

- the rest of the instructions
- grouping cog functions to be loaded together
- better testing of the c standard library
- support for large immediates (currently need to insert augs/augd explicitly)
- support for getting offsets to labels (for rep and similar instructions)
- instruction costs for the scheduler
- optimization passes for various things we can do (for example, a for loop of fixed iterations can be implemented using the rep instruction)
- implement cog-based code relocation in a more flexible way. Currently, any functions that should be in a cog for libcalls are expected to start at 0x100.
