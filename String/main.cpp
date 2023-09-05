#include "cmake-build-debug/string.h"


int main() {
  String s1 = "dfgh";
  char d = 'r';
  String s2;
  s1 += d;
  std::cout << s1;
}