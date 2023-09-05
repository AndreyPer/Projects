#include <cstring>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <cmath>
#include <cstdlib>
#include <fstream>


namespace consts
{
  const double eps = 1e-6;
  const double grad = 180.0;
}
class Line;
struct Point {
  double x;
  double y;
  Point(){}
  Point (double a, double b) {
    x = a;
    y = b;
  }
  void scale(const Point& center, double k) {
    x = center.x + k * (x - center.x);
    y = center.y + k * (y - center.y);
  }

  void rotate(const Point& center, double a, double b) {
    x = center.x + a * (x - center.x) - b * (y - center.y);
    y = center.y + a * (y - center.y) + b * (x - center.x);
  }
  void reflect(const Line& axis);
};

bool similar(double a, double b) {
  return std::abs(a - b) < consts::eps;
}

Point operator - (const Point& first, const Point& second) {
  Point ans;
  ans.x = first.x - second.x;
  ans.y = first.y - second.y;
  return ans;
}

bool operator == (const Point& first, const Point& second) {
  return (similar(first.x, second.x)) && (similar(first.y, second.y));
}
bool operator != (const Point& first, const Point& second) {
  return !(first == second);
}

class Line {
private:
  double a, b, c;
public:
  Line(Point f, Point s) {
    a = f.y - s.y;
    b = s.x - f.x;
    c = f.x * s.y - s.x * f.y;
  }

  Line(double k, double shift) {
    a = -k;
    b = 1;
    c = -shift;
  }

  Line(Point p, double k) {
    a = -k;
    b = p.y - k * p.x;
    c = -b;
  }
  double _a() const {
    return a;
  }
  double _b() const {
    return b;
  }
  double _c() const {
    return c;
  }
};

double len_ab(const Line& axis) {
  return sqrt(axis._a() * axis._a() + axis._b() * axis._b());
}

double dist(const Point& a, const Point& b) {
  return sqrt((a.x - b.x) * (a.x - b.x) + (a.y - b.y) * (a.y - b.y));
}

double line_seg(const Point& a, const Point& b) {
  return sqrt(pow((a.x - b.x), 2) + pow((a.y - b.y), 2));
}

void Point::reflect(const Line& axis) {
  double m = len_ab(axis);
  double a1 = axis._a() / m;
  double b1 = axis._b() / m;
  double c1 = axis._c() / m;
  double d = a1 * x + b1 * y + c1;
  x -= 2.0 * a1 * d;
  y -= 2.0 * b1 * d;
}

bool operator == (const Line& first, const Line& second) {
  return ((similar(first._a() / second._a(), first._b() / second._b())) &&
          (similar(first._b() / second._b(), first._c() / second._c())));
}
bool operator != (const Line& first, const Line& second) {
  return !(first == second);
}
class Ellipse;
class Polygon;

// Abstract class
class Shape {
public:
  virtual double perimeter() const = 0; // pure virtual
  virtual double area() const = 0;
  virtual bool isEqualTo(const Shape& another) const = 0;
  virtual bool isCongruentTo(const Shape& another) const = 0;
  virtual bool isSimilarTo(const Shape& another) const = 0;
  virtual bool containsPoint(const Point& point) const = 0;
  virtual void rotate(const Point& center, double angle) = 0;
  virtual void reflect(const Point& center) = 0;
  virtual void reflect(const Line& axis) = 0;
  virtual void scale(const Point& center, double k) = 0;
  virtual ~Shape() = default;
};

class Polygon: public Shape {
protected:
  int cnt = 0;
  std::vector<Point> vertices;
public:
  Polygon() {}
  Polygon(std::vector<Point>& a) {
    for (size_t i = 0; i < a.size(); ++i) {
      vertices.push_back(a[i]);
      cnt++;
    }
  }

  template<class... Args>
  Polygon(const Point& p, Args... args) : Polygon(args...) {
    vertices.push_back(p);
    cnt++;
  }

  double perimeter() const override {
    double perimeter = 0.0;
    for (size_t i = 0; i < vertices.size() - 1; ++i) {
      size_t j = i + 1;
      perimeter += dist(vertices[i], vertices[j]);
    }
    perimeter += dist(vertices[vertices.size() - 1], vertices[0]);
    return perimeter;
  }
  double area() const override {
    double area = 0.0;
    size_t j = cnt - 1;
    for (int i = 0; i < cnt; i++) {
      area += (vertices[j].x + vertices[i].x) * (vertices[j].y - vertices[i].y);
      j = i;
    }
    return std::abs(area / 2.0);
  }

  int verticesCount() const {
    return cnt;
  }

  std::vector<Point> getVertices() {
    return vertices;
  }

  bool isEqualTo(const Shape& another) const override {
    const Polygon* other = dynamic_cast<const Polygon*>(&another);
    if (other == nullptr) {
      return false;
    }
    if (cnt != other->cnt) {
      return false;
    }
    else {
      size_t i = 0;
      std::vector<std::pair<double, double>> first;
      std::vector<std::pair<double, double>> second;
      bool flag = true;
      while (i < vertices.size()) {
        std::pair p1 = std::make_pair(vertices[i].x, vertices[i].y);
        std::pair p2 = std::make_pair(other->vertices[i].x, other->vertices[i].y);
        first.push_back(p1);
        second.push_back(p2);
        ++i;
      }
      std::sort(first.begin(), first.end());
      std::sort(second.begin(), second.end());
      i = 0;
      while (i < vertices.size() && flag) {
        if (first[i] != second[i]) {
          flag = false;
        }
        ++i;
      }
      return flag;
    }
  }

  double Angle(const Point& a, const Point& b, const Point& c) const {
    Point v1 = a - b;
    Point v2 = b - c;
    double denominator = sqrt((v1.x * v1.x + v1.y * v1.y) * (v2.x * v2.x + v2.y * v2.y));
    double cosinus = (v1.x * v2.x + v1.y * v2.y) / denominator;
    return cosinus;
  }

  bool checker (const Polygon* other, int destination) const {
    for (int i = 0; i < cnt; ++i) {
      int j = 0;
      int e = i;
      bool flag1 = true;
      while (abs(j) < cnt && flag1) {
        double angle1 = Angle(vertices[e % cnt], vertices[(e + destination) % cnt],
                              vertices[(e + destination * 2) % cnt]);
        double angle2 = Angle(other->vertices[j % cnt], other->vertices[(j + 1) % cnt],
                              other->vertices[(j + 2) % cnt]);
        bool aa1 = similar(dist(vertices[e % cnt], vertices[(e + destination) % cnt]),
                           dist(other->vertices[j % cnt], other->vertices[(j + 1) % cnt]));
        bool bb1 = similar(dist(vertices[(e + destination) % cnt], vertices[(e + destination * 2) % cnt]),
                           dist(other->vertices[(j + 1) % cnt], other->vertices[(j + 2) % cnt]));
        flag1 = (similar(angle1, angle2) && aa1 && bb1);
        e += destination;
        j += destination;
      }
      if (abs(j) == cnt) {
        return true;
      }
    }
    return true;
  }

  bool isCongruentTo(const Shape& another) const override {
    const Polygon* other = dynamic_cast<const Polygon*>(&another);
    if (other == nullptr) {
      return false;
    }
    if (!(similar(area(), another.area())) || cnt != other->cnt) {
      return false;
    }
    if (checker(other, 1) || checker(other, -1)) {
      return true;
    }
    return false;
  }

  bool isSimilarTo(const Shape& another) const override {
    const Polygon* other = dynamic_cast<const Polygon*>(&another);
    if (other == nullptr) {
      return false;
    }
    Polygon copy = *other;
    if (vertices.size() != other->vertices.size()) {
      return false;
    }
    Point o;
    o.x = 0;
    o.y = 0;
    copy.scale(o, perimeter() / other->perimeter());
    return isCongruentTo(copy);
  }

  bool containsPoint(const Point& point) const override {
    bool flag = false;
    size_t j = vertices.size() - 1;
    for (size_t i = 0; i < vertices.size(); i++) {
      if ((((vertices[i].y < point.y && vertices[j].y >= point.y) or
            (vertices[j].y < point.y && vertices[i].y >= point.y))) &&
          ((vertices[i].x + (point.y - vertices[i].y) / (vertices[j].y - vertices[i].y) *
                            (vertices[j].x - vertices[i].x) < point.x))) {
        flag = !flag;
      }
      j = i;
    }
    return flag;
  }

  bool isConvex() {
    size_t i = 0;
    bool flag = true;
    while (i < vertices.size() - 2 && flag) {
      Point l1 = vertices[i + 1] - vertices[i];
      Point l2 = vertices[i + 2] - vertices[i + 1];
      flag = (l1.x * l2.y - l1.y * l2.x) > 0;
      ++i;
    }
    if (i == vertices.size() - 2) {
      Point l1 = vertices[i + 1] - vertices[i];
      Point l2 = vertices[0] - vertices[i + 1];
      flag = (l1.x * l2.y - l1.y * l2.x) > 0;
      ++i;
    }
    if (flag) {
      Point l1 = vertices[0] - vertices[i];
      Point l2 = vertices[1] - vertices[0];
      flag = (l1.x * l2.y - l1.y * l2.x) > 0;
    }
    return flag;
  }

  void rotate(const Point& center, double angle) override {
    double rad = M_PI / consts::grad;
    angle *= rad;
    for (size_t i = 0; i < vertices.size(); i++) {
      double x_ = vertices[i].x - center.x;
      double y_ = vertices[i].y - center.y;
      vertices[i].x = center.x + (x_ * cos(angle) - y_ * sin(angle));
      vertices[i].y = center.y + (x_ * sin(angle) + y_ * cos(angle));
    }
  }

  void reflect(const Point& center) override {
    for (size_t i = 0; i < vertices.size(); ++i) {
      vertices[i].rotate(center, vertices[i].x, vertices[i].y);
    }
  }

  void reflect(const Line& axis) override {
    for (size_t i = 0; i < vertices.size(); ++i) {
      vertices[i].reflect(axis);
    }
  }

  void scale(const Point& center, double k) override {
    for (size_t i = 0; i < vertices.size(); ++i) {
      vertices[i].scale(center, k);
    }
  }

  ~Polygon() = default;
};

class Ellipse: public Shape {
protected:
  std::pair<Point,Point> f;
  double a, b, c, e;
public:
  Ellipse(){};
  Ellipse(const Point& f1, const Point& f2, double l) {
    f.first = f1;
    f.second = f2;
    a = l / 2;
    c = dist(f1, f2) / 2.0;
    b = sqrt((pow(a, 2) - pow(c, 2)));
    e = c / a;
  }

  std::pair<Point,Point> focuses() {
    return f;
  }

  std::pair<Line, Line> directrices() {
    Line l1(0, 0), l2(0, 0);
    return std::make_pair(l1, l2);
  }

  double eccentricity() const {
    return e;
  }

  Point center() const {
    Point o;
    o.x = (f.first.x + f.second.x) / 2.0;
    o.y = (f.first.y + f.second.y) / 2.0;
    return o;
  }

  double perimeter() const override {
    return M_PI * (3 * (a + b) - (sqrt((3 * a + b) * (a + 3 * b))));
  }
  double area() const override {
    return M_PI * a * b;
  }

  bool isEqualTo(const Shape& another) const override {
    const Ellipse* other = dynamic_cast<const Ellipse*>(&another);
    if (other == nullptr) {
      return false;
    }
    return (similar(f.first.x, other->f.first.x) &&
            similar(f.first.y, other->f.first.y) &&
            similar(f.second.x, other->f.second.x) &&
            similar(f.second.y, other->f.second.y) &&
            similar(a, other->a));
    return another.isEqualTo(*this);
  }

  bool isCongruentTo(const Shape& another) const override {
    const Ellipse* other = dynamic_cast<const Ellipse*>(&another);
    if (other == nullptr) {
      return false;
    }
    return (a == other->a && b == other->b);
  }

  bool isSimilarTo(const Shape& another) const override {
    const Ellipse* other = dynamic_cast<const Ellipse*>(&another);
    if (other == nullptr) {
      return false;
    }
    return (e == other->e);
  }

  bool containsPoint(const Point& point) const override {
    return ((pow((point.x - center().x), 2) / (a * a) +
             pow((point.y - center().y), 2) / (b * b)) <= 1 + consts::eps);
  }

  void rotate(const Point& center, double angle) override {
    f.first.rotate(center, cos(angle), sin(angle));
    f.second.rotate(center, cos(angle), sin(angle));
  }

  void reflect(const Point& center) override {
    f.first.rotate(center, -1, 0);
    f.second.rotate(center, -1, 0);
  }

  void reflect(const Line& axis) override {
    f.first.reflect(axis);
    f.second.reflect(axis);
  }

  void scale(const Point& center, double k) override {
    f.first.scale(center, k);
    f.second.scale(center, k);
    a *= std::abs(k);
  }

  ~Ellipse() = default;
};


class Circle: public Ellipse {
public:
  Circle() {};

  Circle(const Point& o, double r) : Ellipse(o, o, 2.0 * r) {}

  double radius() const {
    return a;
  }

  double perimeter() const override {
    return 2 * M_PI * a;
  }
  double area() const override {
    return M_PI * a * a;
  }

  bool containsPoint(const Point& point) const override {
    return line_seg(f.first, point) <= a + consts::eps;
  }

  ~Circle() = default;
};

class Rectangle: public Polygon {
public:
  Rectangle() {}
  Rectangle(const Point& point1, const Point& point3, double k) {
    if (k < 1) {
      k = 1 / k;
    }
    double angle = atan(k);
    k = sqrt(k * k + 1);
    Point point2 = point3;
    point2.rotate(point1, cos(angle) / k, sin(angle) / k);
    Point o;
    o.x = (point1.x + point3.x) / 2.0;
    o.y = (point1.y + point3.y) / 2.0;
    Point point4 = point2;
    point4.rotate(o, -1, 0);
    vertices = {point1, point2, point3, point4};
    cnt = 4;
  }

  Point center() const {
    Point o;
    o.x = (vertices[0].x + vertices[2].x) / 2.0;
    o.y = (vertices[0].y + vertices[2].y) / 2.0;
    return o;
  }

  std::pair<Line, Line> diagonals() {
    return std::make_pair(Line(vertices[0], vertices[2]), Line(vertices[1], vertices[3]));
  }

  ~Rectangle() = default;
};

class Square: public Rectangle {
public:
  Square() {}

  Square(const Point& point1, const Point& point3) : Rectangle(point1, point3, 1) {}

  Circle circumscribedCircle() {
    Point o = center();
    double r = line_seg(vertices[0], vertices[2]) / 2.0;
    return Circle(o, r);
  }

  Circle inscribedCircle() {
    Point o = center();
    double r = line_seg(vertices[0], vertices[1]) / 2.0;
    return Circle(o, r);
  }

  double perimeter() const override {
    return 4 * line_seg(vertices[0], vertices[1]);
  }

  double area() const override {
    return pow(vertices[0].x - vertices[1].x, 2) + pow(vertices[0].y - vertices[1].y, 2);
  }

  ~Square() = default;
};

class Triangle: public Polygon {
protected:
  double a, b, c;
public:
  Triangle() {}

  Triangle(const Point& point1, const Point& point2, const Point& point3) {
    vertices = {point1, point2, point3};
    c = line_seg(vertices[0], vertices[1]);
    a = line_seg(vertices[1], vertices[2]);
    b = line_seg(vertices[2], vertices[0]);
  }

  double perimeter() const override {
    return a + b + c;
  }

  double area() const override {
    return sqrt(((a + b + c) / 2.0) * ((a + b + c) / 2.0 - a) * ((a + b + c) / 2.0 - b) * ((a + b + c) / 2.0 - c));
  }

  Point orthocenter() {
    Point o;
    Point f = vertices[0], s = vertices[1], t = vertices[2];
    o.x = (f.y * f.y + s.x * t.x) * (t.y - s.y) + (s.y * s.y + f.x * t.x) * (f.y - t.y) +
          (t.y * t.y + f.x * s.x) * (s.y - f.y) / (f.x * (s.y - t.y) + s.x * (t.y - f.y) + t.x * (f.y - s.y));
    o.y = (f.x * f.x + s.y * t.y) * (t.x - s.x) +
          (s.x * s.x + f.y * t.y) * (f.x - t.x) +
          (t.x * t.x + f.y * s.y) * (s.x - f.x) /
          (f.y * (s.x - t.x) + s.y * (t.x - f.x) +
           t.y * (f.x - s.x));
    return o;
  }

  Circle circumscribedCircle() { // опис
    double r = a * b * c / (4 * area());
    Point o;
    Point f = vertices[0], s = vertices[1], t = vertices[2];
    double d = 2 * (f.x * (s.y - t.y) + s.x * (t.y - f.y) + t.x * (f.y - s.y));
    o.x = ((f.x * f.x + f.y * f.y) * (s.y - t.y) + (s.x * s.x + s.y * s.y) * (t.y - f.y) +
           (t.x * t.x + t.y * t.y) * (f.y - s.y)) / d;
    o.y = ((f.x * f.x + f.y * f.y) * (t.x - s.x) + (s.x * s.x + s.y * s.y) * (f.x - t.x) +
           (t.x * t.x + t.y * t.y) * (s.x - f.x)) / d;
    return Circle(o, r);
  }

  Circle inscribedCircle() { // впис
    double r = (2 * area()) / (a + b + c);
    Point o;
    o.x = (a * vertices[0].x + b * vertices[1].x + c * vertices[2].x) / (a + b + c);
    o.y = (a * vertices[0].y + b * vertices[1].y + c * vertices[2].y) / (a + b + c);
    return Circle(o, r);
  }

  Point centroid() {
    Point o;
    o.x = (vertices[0].x + vertices[1].x + vertices[2].x) / 3.0;
    o.y = (vertices[0].y + vertices[1].y + vertices[2].y) / 3.0;
    return o;
  }

  Circle ninePointsCircle() {
    double r = circumscribedCircle().radius() / 2.0;
    Point o;
    o.x = (orthocenter().x + circumscribedCircle().center().x) / 2.0;
    o.y = (orthocenter().y + circumscribedCircle().center().y) / 2.0;
    return Circle(o, r);
  }

  Line EulerLine() {
    return Line(centroid(), ninePointsCircle().center());
  }

  ~Triangle() = default;
};

bool operator==(const Shape& a, const Shape& b) {
  return a.isEqualTo(b);
}

bool operator!=(const Shape& a, const Shape& b) {
  return !a.isEqualTo(b);
}