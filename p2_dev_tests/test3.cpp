/*
  This tests the creation of basic conditionals and branching

  compilation:
    ../build/bin/clang -target p2 -S -c test3.cpp -emit-llvm -o test3.ll
    ../build/bin/llc -march=p2 -filetype=asm test3.ll -o test3.s
    ../build/bin/llc -march=p2 -filetype=obj test3.ll -o test3.o
*/

int main() {
    int a = 3;

    if (a == 3) {
        a = 1;
    }
}