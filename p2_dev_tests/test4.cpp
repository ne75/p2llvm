/*
  This tests more complex conditionals, booleans, loops, and the creation of the % operation, which uses pseudo instructions
  to then lower to P2 instructions

  compilation:
    ../build/bin/clang -target p2 -S -c test4.cpp -emit-llvm -o test4.ll
    ../build/bin/llc -march=p2 -filetype=asm -debug test4.ll -o test4.s
    ../build/bin/llc -march=p2 -filetype=obj -debug test4.ll -o test4.o
*/

int main() {
    // int a = 1;
    // int b = 3;
    // for (int i = 0; i < 25; i++) {
    //     if (i % b)
    //         a = i-b;
    // }

    // b = b << 3;
    // b = b >> 3;
    // b = a*b;
    // b = -b;

    bool a = false;
    bool b = !a;

    return 0;
}