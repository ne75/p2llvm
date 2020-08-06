/*
  This tests the creation of basic math instructions and stack usage

  compilation:
    ../build/bin/clang -target p2 -S -c test2.cpp -emit-llvm -o test2.ll
    ../build/bin/llc -march=p2 -filetype=asm test2.ll -o test2.s
    ../build/bin/llc -march=p2 -filetype=obj test2.ll -o test2.o
*/

int main() {
    int a = 5;
    int b = 2;
    int c = a+b;
}

