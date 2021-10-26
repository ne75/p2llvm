# P2 LLVM Test Suite

This directory contains a set of tests that are used as unit and regression tests for the P2 backend in the LLVM compiler suite. The overall structure is a series of bite-sized programs to test one specific feature. They are meant to run in order, as some test cases will assume previous tests completed successfully (although the failure of a later test case can indicate a gap in an earlier one).

This test suite DOES NOT test (at least not yet) that the output of the compiler is optimal. 

## Structure

The `src/` directory contains all the tests. Most are single-file tests, except for specific test that will test linking object together, as needed. 

Each test is named `t[x].cpp`, where `x` is the test number, starting at `t0`. If a test requires multiple files, they are named `t[x]_[y].cpp`, where `y` is the file number for each file within the test. 

The `ref/` directory contains reference outputs corresponding to each test. Each one is a YAML file containing a single key for the expected string to be read over the terminal. It's left as a YAML for future expansion. Each reference output is named `t[x].yaml` to match the test it is in reference to. Since all tests output to the terminal, there's an expectation that `printf()` and UART output always work. Therefore, the first test is a simple test that UART output actually works.

CMake is used to manage configuration of all tests. 

## Running Tests

Usage: `test.py -p <port> -t [tests...]`. 
- `port` is the port an attached P2 is located. 
- `tests` is an optional list of tests to run, in order. If omitted, all tests are run

`test.py` will run either all tests, or the specific list of tests. A test is run by compiling it, loading it onto the P2, capturing its output, and comparing it to a reference output. 

Each test is run twice: once compiled with no optimization and once with full `-Os` optimization. This is to ensure optimization doesn't break anything and doesn't hide any problems that would exist without it.

Some tests seem reduntant, but are included so that if that feature seems to be broken, there's a very simple function to review the assembly/LLVM debug output to make sure it is functioing correctly 

## Tests

Each test's output should start with a `$` and end with a `~`. This is how the test runner will know that the test output is complete and not waiting for more

### Features That Should be Tested:
- Arithmetic: 
    - addition, subtraction, multiplcation, division of integers
    - evaluation of booleans/equality
    - boolean algebra
    - unary operators
- Data structures
    - structs
    - various data size types (char, short, int, etc) are wrapped correctly
    - arrays
    - classes
        - accessing static and non-static members
        - virtual functions and subclasses
    - volatile variables
- Conditionals:
    - testing equality, inequality, greater than, less than, etc and branching
    - branching based on a condition
    - loops/etc
    - switch statements
- Global Data
    - placing data in global space works the same as stack space
- Functions
    - Calling a function with no arguments and no return
    - Calling a function with no arguments and a return value
    - Calling a function with <4 arguments
    - Calling a function with >4 arguments
    - Calling a function with stack-based arguments 
    - Calling a function with global arguments
    - Calling a function with calling a function with refernce and pointer arguments
    - Calling a function with a pointer/reference to a stack-based arguments
    - Calling a function with a pointer/reference to a global argument
    - Calling a function with mixed argument sources (stack, global, pointer, etc)
    - Recrusion
    - Variadic functions
- P2 Features
    - Setting i/o pin states
    - Waiting (not sure how to do this one yet)
    - Starting hub mode cogs (also not sure how to do it, other than have cogs take turns printing?)
    - Starting cog mode cogs
    - Streamer (just test that data can be streamed slowly to pins or something by running the streamer and checking in register)
- Misc.
    - dynamic memory allocation/heap usage
    - linking external objects
 
This is a list of all tests and the feature(s) they each test.

- `t0`: This tests basic compilation and loading. It has no output, and is successful if the program loads without failure
- `t1`: This test that serial output works as expected.
- `t2`: Arithmetic. Test basic arithmetic, basically all normal operators
- `t3`: Storing/accessing data in structs on the stack
- 't4`: Storing/accessing data in arrays
- `t5`: Storing/accessing various data sizes 
- `t6`: Statically constructing classes on the stack, storing/accessing static/non-static data
- `t7`: Subclassing and virtual functions
- `t8`: Evaluating conditions and executing if/else statements
- `t9`: Loops (for, while, do-while)
- `t10`: Switch statements
- `t11`: Staticly constructing classes in global space, storing/accessing members
- `t12`: Functions: no argument and no returns
- `t13`: Functions: no arguments and a return value
- `t14`: Functions: <4 arguments
- `t15`: Functions: >4 arguments
- `t16`: Functions: stack-based arguments (including byval)
- `t17`: Functions: global arguments (including byval)
- `t18`: Functions: pointer/reference to a stack-based arguments
- `t19`: Functions: pointer/reference to a global argument
- `t20`: Functions: mixed argument sources (stack, global, pointer, etc)
- `t21`: Recursion
- `t22`: Variadic functions, with a struct in the argument list
- `t23`: Setting/reading P2 hardware registers
- `t24`: Waiting by a fixed amount
- `t25`: Starting hub-mode cogs
- `t26`: Starting cog-mode cogs
- `t27`: Streamer functions (not sure how to test this yet)
- `t28`: Locks
- `t29`: Default values in function arguments
- `t30`: Large memory offsets
- `t31`: Returning structs by value
- `t32`-`t49`: Reserved for future use
- `t50`: Dyanmic memory allocation (using `new`)
- `t51`: Linking external objects
