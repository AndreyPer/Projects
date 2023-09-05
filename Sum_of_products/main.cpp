#include <iostream>

long long counter(int** all, int* size, int* checker, int k, int argc, int prod) {
  if (k == argc) {
    return prod;
  }
  long long sum = 0;
  for (int i = 0; i < size[k]; ++i) {
    if (checker[i] == 0) {
      checker[i] = 1;
      sum += counter(all, size, checker, k + 1, argc, prod * all[k][i]);
      checker[i] = 0;
    }
  }
  return sum;
}
int main(int argc, char *argv[]) {
  int ma = 0;
  int** all = new int*[argc - 1]{};
  int* size = new int[argc - 1]{};
  for (int i = 0; i < argc - 1; ++i) {
    for (int k = 0; argv[i + 1][k]; ++k) {
      size[i] = 10 * size[i] + (argv[i + 1][k] - '0');
    }
    all[i] = new int[size[i]];
    for (int j = 0; j < size[i]; ++j) {
      std::cin >> all[i][j];
      if (j > ma) {
        ma = j;
      }
    }
  }
  int* checker = new int[ma + 1]{};
  std::cout << counter(all, size, checker, 0, argc - 1, 1) << '\n';
  for (int i = 0; i < argc - 1; ++i) {
    delete[] all[i];
  }
  delete[] all;
  delete[] size;
  delete[] checker;
}