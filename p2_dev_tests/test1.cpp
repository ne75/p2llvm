/*
  This tests basic setup of the backend to generate assembly for an empty program

  compilation:
    ../build/bin/clang -target p2 -S -c test1.cpp -emit-llvm -o test1.ll
    ../build/bin/llc -march=p2 -filetype=asm test1.ll -o test1.s
    ../build/bin/llc -march=p2 -filetype=obj test1.ll -o test1.o
*/

int main() {
    // int a = 5;
    // int b = 2;

    // int c = a + b;      // c = 7
    // //int d = b + 1;      // d = 3

    return 0;
}

