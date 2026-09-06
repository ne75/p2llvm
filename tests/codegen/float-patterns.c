// RUN: %clang -O0 -mllvm -verify-machineinstrs -c %s -o %t.O0.o
// RUN: %clang -O2 -mllvm -verify-machineinstrs -c %s -o %t.O2.o
float single(float a, float b) { return a * b + a / b - b; }
double twice(double a, double b) { return a * b + a / b - b; }
int compare_double(double a, double b) { return a < b; }
long long convert(double x) { return (long long)x; }
double convert_back(long long x) { return (double)x; }
