/*
  This tests function calling and passing arguments to the function

  compilation:
    ../build/bin/clang -target p2 -S -c test5.cpp -emit-llvm -o test5.ll
    ../build/bin/llc -march=p2 -filetype=asm -debug test5.ll -o test5.s
    ../build/bin/llc -march=p2 -filetype=obj -debug test5.ll -o test5.o
*/

int sumsub(int a, int b, int c, int d, int e, int f) __attribute__((noinline));

int sumsub(int a, int b, int c, int d, int e, int f) {
    return a - b + c - d + e - f;
}

int total;

int main() {
    total = sumsub(1, 2, 3, 4, 5, 6);
    return 0;
}