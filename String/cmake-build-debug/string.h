#ifndef STRING_STRING_H
#define STRING_STRING_H

#include <cstring>
#include <iostream>

class String
{
private:
  size_t cap;
  size_t sz;
  char* arr;

  void swap(String& str) {
    std::swap(sz, str.sz);
    std::swap(cap, str.cap);
    std::swap(arr, str.arr);
  }

  void copy(size_t n) {
    cap = n;
    char *new_data = new char[cap + 1];
    strcpy(new_data, arr);
    delete[] arr;
    arr = new_data;
  }

public:

  String() : cap(0), sz(0), arr(new char[1]) {
    arr[0] = '\0';
  }

  String(size_t sz, char chr) : cap(sz), sz(sz), arr(new char[sz + 1]) {
    memset(arr, chr, sz);
    arr[sz] = '\0';
  }

  String(const String& str) : cap(str.cap), sz(str.sz), arr(new char[str.cap + 1]) {
    memcpy(arr, str.arr, sz + 1);
  }

  String(char const* str) : cap(strlen(str)), sz(cap), arr(new char[cap + 1]) {
    strcpy(arr, str);
  }

  String& operator = (String str) {
    swap(str);
    return *this;
  }

  String& operator += (const String& other) {
    if (other.sz + sz >= cap) {
      copy(cap + other.cap);
    }
    memcpy(arr + sz, other.arr, other.sz);
    sz += other.sz;
    return *this;
  }
  String& operator += (char chr) {
    if (sz == cap) {
      copy(cap * 2 + 1);
    }

    arr[sz] = chr;
    arr[++sz] = '\0';
    return *this;
  }

  void shrink_to_fit() {
    copy(sz);
  }

  bool empty() {
    return sz == 0;
  }

  void clear() {
    sz = 0;
  }

  char& operator[](size_t index) {
    return arr[index];
  }

  const char& operator[](size_t index) const {
    return arr[index];
  }

  char& front() {
    return *arr;
  }

  const char& front() const {
    return *arr;
  }

  char& back() {
    return arr[sz - 1];
  }

  const char& back() const {
    return arr[sz - 1];
  }

  size_t length() const {
    return sz;
  }

  size_t size() const {
    return sz;
  }

  size_t capacity() const {
    return cap;
  }

  char* data() {
    return arr;
  }

  const char* data() const {
    return arr;
  }

  size_t find(const String& a) const {
    for (size_t i = 0; i + a.sz <= sz; ++i) {
      size_t j = 0;
      while (j < a.sz && arr[i + j] == a.arr[j]) {
        ++j;
      }
      if (j == a.sz) {
        return i;
      }
    }
    return sz;
  }

  size_t rfind(const String& a) const {
    for (size_t i = sz - 1; i + 1 >= a.sz; --i) {
      size_t j = a.sz;
      while (j > 0 && arr[i - a.sz + j] == a[j - 1]) {
        --j;
      }
      if (j == 0) {
        return i - a.sz + 1;
      }
    }
    return sz;
  }

  void pop_back() {
    arr[--sz] = '\0';
  }

  void push_back(char chr) {
    *this += chr;
  }

  String substr(size_t start, int count) const {
    String sub = String(count, '\0');
    memcpy(sub.arr, arr + start, sz);
    return sub;
  }

  ~String() {
    delete[] arr;
  }
};

String operator + (const String& a, const String& b) {
  String result = a;
  result += b;
  return result;
}

String operator + (const String& a, char b) {
  String result = a;
  result += b;
  return result;
}

String operator + (char a, const String& b) {
  String ans = String(b.size() + 1, '\0');
  ans[0] = a;
  memcpy(ans.data() + 1, b.data(), b.size());
  return ans;
}

bool operator < (const String& a, const String& b) {
  return (memcmp(a.data(), b.data(), a.length()) < 0);
}

bool operator >= (const String& a, const String& b) {
  return !(a < b);
}

bool operator > (const String& a, const String& b) {
  return b < a;
}

bool operator <= (const String& a, const String& b) {
  return !(a > b);
}

bool operator == (const String& a, const String& b) {
  if (a.size() != b.size()) {
    return false;
  }
  return (memcmp(a.data(), b.data(), a.length()) == 0);
}

bool operator != (const String& a, const String& b) {
  return !(a == b);
}

std::ostream& operator << (std::ostream& out, const String& str) {
  for (size_t i = 0; i < str.size(); ++i) {
    out << str[i];
  }
  return out;
}

std::istream& operator >> (std::istream& in, String& str) {
  char c;
  in >> c;
  str.clear();
  str.push_back(c);
  in >> std::noskipws;
  while (in >> c && c != ' ' && c != '\n') {
    str.push_back(c);
  }
  return in >> std::skipws;
}

#endif //STRING_STRING_H