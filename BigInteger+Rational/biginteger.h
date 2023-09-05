#include <cstring>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <cmath>

class BigInteger;
BigInteger operator + (BigInteger a, const BigInteger& b);
BigInteger operator - (BigInteger a, const BigInteger& b);
BigInteger operator * (BigInteger a, const BigInteger& b);
BigInteger operator / (BigInteger a, const BigInteger& b);
BigInteger operator % (BigInteger a, const BigInteger& b);
bool operator > (const BigInteger& a, const BigInteger& b);
bool operator < (const BigInteger& a, const BigInteger& b);
bool operator >= (const BigInteger& a, const BigInteger& b);
bool operator <= (const BigInteger& a, const BigInteger& b);

class BigInteger {
protected:
  bool sg = true;
  static const long long base = 1e9;

  std::vector<long long> digits;
  friend class Rational;

public:
  BigInteger() {}

  BigInteger(const BigInteger& a): sg(a.sg), digits(a.digits) {}

  BigInteger(long long a) {
    if (a < 0) {
      sg = false;
      a *= -1;
    }
    if (a == 0) {
      digits.push_back(0);
    }
    while (a != 0) {
      digits.push_back(static_cast<long long>(a % base));
      a /= base;
    }
  }

  void DeleteZeroes() {
    while (digits.size() > 1 && digits.back() == 0) {
      digits.pop_back();
    }
  }

  std::vector<long long>& Getdigits() {
    return digits;
  }
  const std::vector<long long>& Getdigits() const {
    return digits;
  }

  void abs() {
    sg = true;
  }

  BigInteger(const std::string &str) {
    std::string substr;
    if (str[0] == '-') {
      sg = false;
      substr = str.substr(1);
    }
    else {
      sg = true;
      substr = str;
    }
    for (long long i = static_cast<long long>(substr.length()); i > 0; i -= 9) {
      if (i < 9) {
        digits.push_back(stoi(substr.substr(0, i)));
      }
      else {
        digits.push_back(stoi(substr.substr(i - 9, 9)));
      }
    }
    DeleteZeroes();
  }

  BigInteger& operator = (const BigInteger& other) {
    sg = other.sign();
    digits.resize(other.sz());
    for (long long i = 0; i < static_cast<long long>(other.sz()); ++i) {
      digits[i] = other.digits[i];
    }
    return *this;
  }

  BigInteger& operator += (const BigInteger& other) {
    if (static_cast<long long>(sz()) == 1 and digits[0] == 0) {
      *this = other;
    }
    else if (sg == other.sg) {
    }

    int carry = 0;
    int prev_carry = 0;
    if (static_cast<long long>(sz()) == 1 and digits[0] == 0) {
      *this = other;
    }
    else if (sg == other.sg) {
      long long len = static_cast<long long>(std::max(digits.size() + 1, other.sz() + 1));
      for (long long i = 0; i < len; ++i) {
        if (i < static_cast<long long>(digits.size())) {
          if (i < static_cast<long long>(other.sz())) {
            prev_carry = carry;
            carry = (digits[i] + other[i] + prev_carry) / base;
            digits[i] = (digits[i] + other[i] + prev_carry) % base;
          }
          else {
            prev_carry = carry;
            carry = (digits[i] + prev_carry) / base;
            digits[i] = (digits[i] + prev_carry) % base;
          }
        }
        else {
          if (i < static_cast<long long>(other.sz())) {
            prev_carry = carry;
            carry = (other[i] + prev_carry) / base;
            digits.push_back((other[i] + prev_carry) % base);
          }
          else if (carry != 0) {
            digits.push_back(carry % base);
          }
        }
      }
    }
    else {
      bool flag = false;
      if (digits.size() < other.sz()) {
        flag = true;
      }
      else if (digits.size() > other.sz()) {
        flag = false;
      }
      else {
        int ex = 0;
        for (auto i = static_cast<long long>(sz() - 1); i >= 0 && ex == 0; --i) {
          if (digits[i] < other[i]) {
            flag = true;
            ex = 1;
          }
          else if (digits[i] > other[i]) {
            flag = false;
            ex = 1;
          }
        }
      }

      if (!flag) {
        for (long long i = 0; i < static_cast<long long>(digits.size()); ++i) {
          if (i < static_cast<long long>(other.sz())) {
            if ((digits[i] - other[i] + carry) < 0) {
              digits[i] = digits[i] - other[i] + carry + base;
              carry = -1;
            }
            else {
              digits[i] = digits[i] - other[i] + carry;
              carry = 0;
            }
          }
          else {
            if ((digits[i] + carry) < 0) {
              digits[i] = digits[i] + carry + base;
              carry = -1;
            }
            else {
              digits[i] = digits[i] + carry;
              carry = 0;
            }
          }
        }
      }

      else {
        sg *= -1;
        for (long long i = 0; i < static_cast<long long>(other.sz()); ++i) {
          if (i < static_cast<long long>(digits.size())) {
            if ((other[i] - digits[i] + carry) < 0) {
              digits[i] = other[i] - digits[i] + carry + base;
              carry = -1;
            }
            else {
              digits[i] = other[i] - digits[i] + carry;
              carry = 0;
            }
          }
          else {
            if ((other[i] + carry) < 0) {
              digits.push_back(other[i] + carry + base);
              carry = -1;
            }
            else {
              digits.push_back(other[i] + carry);
              carry = 0;
            }
          }
        }
      }
    }
    DeleteZeroes();
    if (static_cast<long long>(digits.size()) == 1 && digits[0] == 0) {
      sg = 0;
    }
    return *this;
  }

  BigInteger& operator -= (const BigInteger& other) {
    sg ^= true;
    *this += other;
    sg ^= true;
    return *this;
  }

  BigInteger& operator *= (const BigInteger& other) {
    if (!*this || !other) {
      return *this = 0;
    }
    long long carry;
    BigInteger res;
    for (long long i = 0; i < static_cast<long long>(other.sz()); ++i) {
      int num = other[i];
      BigInteger curr;
      carry = 0;
      for (long long k = 0; k < i; ++k) {
        curr.digits.push_back(0);
      }
      for (long long j = 0; j < static_cast<long long>(digits.size()); ++j) {
        long long help1 = (digits[j] * num + carry) % base;
        carry = (digits[j] * num + carry) / base;
        curr.digits.push_back(help1);
      }
      if (carry != 0) {
        curr.digits.push_back(carry);
      }
      res += curr;
    }
    res.sg = sg * other.sg;
    *this = res;
    DeleteZeroes();
    return *this;
  }

  BigInteger& operator /= (const BigInteger& other) {
    int real_sg = sg * other.sign();
    BigInteger divider = other;
    BigInteger ans;
    BigInteger divisible;
    divider.abs();
    if (static_cast<long long>(digits.size()) == 1 and digits[0] == 0) {
      *this = 0;
      return *this;
    }
    if (other.digits.size() == 1 && other.digits[0] == 1) { return *this; }
    for (size_t i = sz() - 1; i < sz(); --i) {
      if (!divisible) { divisible.digits.pop_back(); }
      divisible.digits.insert(divisible.digits.begin(), 1, digits[i]);
      if (divisible >= divider) {
        long long left = 0;
        long long right = base;
        while (left + 1 < right) {
          long long quotient = (left + right) / 2;
          if (divider * quotient <= divisible) { left = quotient; }
          else { right = quotient; }
        }
        ans.digits.push_back(left);
        divisible -= divider * left;
      }
      else { ans.digits.push_back(0); }
    }
    std::swap(digits, ans.digits);
    reverse(digits.begin(), digits.end());
    DeleteZeroes();
    sg = real_sg;
    return *this;
  }

  BigInteger& operator %= (const BigInteger& other) {
    *this -= (*this / other) * other;
    return *this;
  }

  BigInteger& operator ++ () { // префиксный инкремент
    *this += 1;
    return *this;
  }

  BigInteger operator ++ (int) { // постфиксный инкремент
    BigInteger copy = *this;
    ++*this;
    return copy;
  }

  BigInteger& operator -- () { // префиксный инкремент
    *this -= 1;
    return *this;
  }

  BigInteger operator -- (int) { // постфиксный инкремент
    BigInteger copy = *this;
    --*this;
    return copy;
  }

  const BigInteger operator - () const {
    BigInteger copy = *this;
    if (copy) {
      copy.sg *= -1;
    }
    return copy;
  }

  size_t sz() const {
    return digits.size();
  }

  int sign() const {
    return sg;
  }

  long long& operator[](size_t index) {
    return digits[index];
  }

  const long long& operator[](size_t index) const {
    return digits[index];
  }

  explicit operator bool() const {
    if (static_cast<long long>(digits.size()) == 1 and digits[0] == 0) {
      return false;
    }
    return true;
  }

  std::string toString() const {
    std::string bigint;
    if (!sg && !(static_cast<long long>(digits.size()) == 1 and digits[0] == 0)) {
      bigint += '-';
    }
    bigint += std::to_string(digits[static_cast<long long>(sz() - 1)]);
    for (int i = static_cast<long long>(sz() - 2); i >= 0; --i) {
      std::string num = std::to_string(digits[i]);
      for (size_t j = 0; j < 9 - num.size(); ++j) {
        bigint.push_back('0');
      }
      bigint += num;
    }
    return bigint;
  }
};

BigInteger operator + (BigInteger a, const BigInteger& b) {
  return a += b;
}

BigInteger operator - (BigInteger a, const BigInteger& b) {
  return a -= b;
}

BigInteger operator * (BigInteger a, const BigInteger& b) {
  return a *= b;
}

BigInteger operator / (BigInteger a, const BigInteger& b) {
  return a /= b;
}

BigInteger operator % (BigInteger a, const BigInteger& b) {
  return a %= b;
}

bool operator < (const BigInteger& a, const BigInteger& b) {
  if (static_cast<long long>(a.sz()) == 1 and a[0] == 0 && static_cast<long long>(b.sz()) == 1 and b[0] == 0) { return false; }
  if ((a.sign() == false && b.sign() == true)) { return true; }
  if ((a.sign() == true && b.sign() == false)) { return false; }
  if (a.sz() < b.sz()) { return a.sign(); }
  if (a.sz() > b.sz()) { return a.sign() == false; }
  for (int i = static_cast<long long>(a.sz() - 1); i >= 0; --i) {
    if (a[i] < b[i]) {
      return a.sign();
    }
    else if (a[i] > b[i]) {
      return a.sign() == false;
    }
  }
  return false;
}

bool operator > (const BigInteger& a, const BigInteger& b) {
  return b < a;
}

bool operator >= (const BigInteger& a, const BigInteger& b) {
  return !(a < b);
}

bool operator <= (const BigInteger& a, const BigInteger& b) {
  return !(a > b);
}

bool operator == (const BigInteger& a, const BigInteger& b) {
  return !(a < b || a > b);
}

bool operator != (const BigInteger& a, const BigInteger& b)
{
  return !(a == b);
}

BigInteger operator""_bi(unsigned long long x)
{
  return BigInteger(x);
}


BigInteger operator""_bi(const char* x)
{
  std::string str(x);
  return BigInteger(str);
}

std::istream& operator >> (std::istream& in, BigInteger& bi) {
  std::string str;
  in >> str;
  bi = BigInteger(str);
  return in;
}

std::ostream& operator << (std::ostream& out, const BigInteger& bi) {
  return out << bi.toString();
}


class Rational;
Rational operator + (Rational a, const Rational& b);
Rational operator - (Rational a, const Rational& b);
Rational operator * (Rational a, const Rational& b);
Rational operator / (Rational a, const Rational& b);
bool operator > (const Rational& a, const Rational& b);
bool operator < (const Rational& a, const Rational& b);
bool operator >= (const Rational& a, const Rational& b);
bool operator <= (const Rational& a, const Rational& b);

class Rational {
private:
  int _sg = 1;
  const int base = 1e9;
  std::pair<BigInteger, BigInteger> fraction;
public:
  Rational() {}

  Rational(const Rational& t): _sg(t._sg), fraction(t.fraction){}

  Rational(const BigInteger& a, const BigInteger& b)
  {
    _sg = a.sign() * b.sign();
    BigInteger x = a;
    BigInteger y = b;
    x.abs();
    y.abs();
    reduction(x, y);
    fraction = std::make_pair(x, y);
  }

  Rational(const BigInteger& a)
  {
    _sg = a.sign();
    BigInteger x = a;
    x.abs();
    fraction = std::make_pair(x, 1);
  }
  Rational(long long a)
  {
    if (a == 0) {
      _sg = 0;
    }
    else if (a < 0) {
      _sg = -1;
      a *= -1;
    }
    else {
      _sg = 1;
    }
    BigInteger numerator;
    while (a != 0) {
      numerator.digits.push_back(static_cast<int>(a % base));
      a /= base;
    }
    fraction = std::make_pair(numerator, 1);
  }

  Rational(long long a, long long b)
  {
    if (a == 0) {
      _sg = 0;
    }
    else if (a * b < 0) {
      _sg = -1;
    }
    else {
      _sg = 1;
    }
    if (a < 0) {
      a *= -1;
    }
    if (b < 0) {
      b *= -1;
    }
    BigInteger numerator;
    BigInteger denominator;
    while (a != 0) {
      numerator.digits.push_back(static_cast<int>(a % base));
      a /= base;
    }
    while (b != 0) {
      denominator.digits.push_back(static_cast<int>(b % base));
      b /= base;
    }
    reduction(numerator, denominator);
    fraction = std::make_pair(numerator, denominator);
  }

  BigInteger gcd1(BigInteger& a, BigInteger& b) {
    if (!b) {
      return a;
    } else {
      return gcd(b, a %= b);
    }
  }
  BigInteger gcd(BigInteger a, BigInteger b) {
    a.abs();
    b.abs();
    return gcd1(a, b);
  }

  void reduction (BigInteger& a, BigInteger& b) {
    BigInteger c = gcd(a, b);
    a /= c;
    b /= c;
  }

  int _sign() const {
    return _sg;
  }

  std::pair<BigInteger, BigInteger> fr() {
    return fraction;
  }

  Rational& operator = (const Rational& other) {
    _sg = other._sign();
    fraction.first.digits.resize(other.fraction.first.sz());
    fraction.second.digits.resize(other.fraction.second.sz());
    for (long long i = 0; i < static_cast<long long>(other.fraction.first.sz()); ++i) {
      fraction.first.digits[i] = other.fraction.first.digits[i];
    }
    for (long long i = 0; i < static_cast<long long>(other.fraction.second.sz()); ++i) {
      fraction.second.digits[i] = other.fraction.second.digits[i];
    }
    return *this;
  }

  Rational& operator += (const Rational& other) {
    if (_sg == 0) {
      *this = other;
      return *this;
    }
    else if (other._sg == 0) {
      return *this;
    }
    else if (_sg == other._sg) {
      _sg = _sg * other._sg;
      fraction.first = fraction.first * other.fraction.second + fraction.second * other.fraction.first;
    }
    else if (_sg == 1 && other._sg == -1) {
      fraction.first = fraction.first * other.fraction.second - fraction.second * other.fraction.first;
      _sg = fraction.first.sg;
    }
    else {
      fraction.first = fraction.second * other.fraction.first - fraction.first * other.fraction.second;
      _sg = fraction.first.sg;
    }
    fraction.second = fraction.second * other.fraction.second;
    fraction.first.sg *= fraction.first.sg;
    reduction(fraction.first, fraction.second);
    return *this;
  }

  Rational& operator -= (const Rational& other) {
    _sg *= -1;
    *this += other;
    _sg *= -1;
    return *this;
  }

  Rational& operator *= (const Rational& other) {
    _sg *= other._sg;
    fraction.first *= other.fraction.first;
    fraction.second *= other.fraction.second;
    reduction(fraction.first, fraction.second);
    return *this;
  }

  Rational& operator /= (const Rational& other) {
    _sg *= other._sg;
    fraction.first *= other.fraction.second;
    fraction.second *= other.fraction.first;
    reduction(fraction.first, fraction.second);
    return *this;
  }

  const Rational operator - () const {
    Rational copy = *this;
    if (fraction.first) {
      copy._sg *= -1;
    }
    return copy;
  }

  std::string toString() const {
    std::string str;
    if (!fraction.first) {
      str.push_back('0');
      return str;
    }
    if (_sg == -1) {
      str.push_back('-');
    }
    if (static_cast<long long>(fraction.second.sz()) == 1 && fraction.second.digits[0] == 1) {
      str += fraction.first.toString();
    }
    else {
      str += fraction.first.toString();
      str.push_back('/');
      str += fraction.second.toString();
    }
    return str;
  }

  std::string asDecimal(size_t precision = 0) {
    BigInteger multiplier = 1;
    std::string decimal;
    decimal += (fraction.first / fraction.second).toString();
    BigInteger mod = fraction.first % fraction.second;
    mod.abs();
    if (precision == 0) {
      return decimal;
    }
    for (size_t i = 0; i < precision / 9; ++i) {
      mod *= base;
    }
    for (size_t i = 0; i < precision % 9; ++i) {
      mod *= 10;
    }
    mod /= fraction.second;
    if (decimal == "0" && _sg < 0) {
      decimal = "-0";
    }
    std::string ans = decimal + '.';
    for (int i = 0; i < (int)precision - (int)mod.toString().size(); ++i) {
      ans += '0';
    }
    return ans + mod.toString();
  }

  explicit operator double() const {
    Rational curr = *this;
    return std::stod(curr.asDecimal(20));
  }
};

Rational operator + (Rational a, const Rational& b) {
  return a += b;
}

Rational operator - (Rational a, const Rational& b) {
  return a -= b;
}

Rational operator * (Rational a, const Rational& b) {
  return a *= b;
}

Rational operator / (Rational a, const Rational& b) {
  return a /= b;
}

bool operator < (const Rational& a, const Rational& b) {
  Rational x = a;
  Rational y = b;
  x.fr().first *= y.fr().second;

  if ((x._sign() == 0 && y._sign() == 0)) {
    return false;
  }
  else if ((x._sign() <= 0 && y._sign() >= 0) || (x.fr().first.sz() < y.fr().first.sz())) {
    return true;
  }
  else if ((x._sign() >= 0 && y._sign() <= 0) || (x.fr().first.sz() > y.fr().first.sz())) {
    return false;
  }
  else {
    for (int i = static_cast<long long>(x.fr().first.sz() - 1); i >= 0; --i) {
      if ((x.fr().first[i] < y.fr().first[i] && a._sign() > 0) || (x.fr().first[i] > y.fr().first[i] && a._sign() < 0)) {
        return true;
      }
      else if ((x.fr().first[i] > y.fr().first[i] && a._sign() > 0) || (x.fr().first[i] < y.fr().first[i] && a._sign() < 0)) {
        return false;
      }
    }
    return false;
  }
}

bool operator > (const Rational& a, const Rational& b) {
  return b < a;
}

bool operator >= (const Rational& a, const Rational& b) {
  return !(a > b);
}

bool operator <= (const Rational& a, const Rational& b) {
  return !(a > b);
}

bool operator == (const Rational& a, const Rational& b) {
  return !(a < b || a > b);
}

bool operator != (const Rational& a, const Rational& b) {
  return !(a == b);
}