#include <iostream>
#include <cstdlib>
#include <cstdio>
int main() {
  long * abc = (long*)malloc(8);
  abc++;
  printf("%p\n",abc);
}