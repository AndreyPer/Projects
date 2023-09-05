#include "geometry.h"
#include <iostream>
using namespace std;

int main() {
  const int ax = -2, ay = 2, bx = 1, by = 2,
          cx = 3, cy = -1, dx = -1, dy = -2,
          ex = 1, ey = -1, fx = 6, fy = 1;

  Point a(ax, ay);
  Point b(bx, by);
  Point c(cx, cy);
  Point d(dx, dy);
  Point e(ex, ey);

  Line ae(a, e);
  Line ea(e, a);
  Line line1(3, 5);
  Line line2(c, -1.5);

  //testing destructor
  for (size_t i = 0; i < 1000000; ++i) {
    Shape *square = new Square(a, e);
    delete square;
  }

  Point f(fx, fy);
  Polygon abfcd(a, b, f, c, d);
  std::vector<Point> vec = {c, f, b, a, d};

  Polygon cfbad(vec);
  if (abfcd != cfbad) {
    std::cerr << "Test 2 failed. (polygons operator ==)\n";
    return 1;
  }

  if (!abfcd.isConvex()) {
    std::cerr << "Test 3 failed. (convexity of polygons)\n";
    return 1;
  }

  Polygon bfced(b, f, c, e, d);
  if (bfced.isConvex()) {
    std::cerr << "Test 4 failed. (convexity of polygons)\n";
    return 1;
  }
  Triangle abd(a, b, d);
  Polygon abfced(a, b, f, c, e, d);
  if ((abd.area() + bfced.area() - abfced.area()) > 1e-6) {
    std::cerr << "Test 5 failed. (area of polygons)\n";
    return 1;
  }
  if (abd.isSimilarTo(abfced)) {
    std::cerr << "Test 6 failed. (similarity of polygons)\n";
    return 1;
  }
  if (abfced.isCongruentTo(abd)) {
    std::cerr << "Test 7 failed. (congruency of polygons)\n";
    return 1;
  }

  Polygon newAbfced = abfced;
  newAbfced.rotate(Point(0, 0), 50);
  newAbfced.scale(Point(0, 0), 3);


  if ((9 * abfced.area() - newAbfced.area()) > 1e-6) {
    std::cerr << "Test 8 failed. (after rotation and scaling by k, area must increase in k^2 times)\n";
    return 1;
  }
  if ((3 * abfced.perimeter() - newAbfced.perimeter()) > 1e-6) {
    std::cerr << "Test 9 failed. (after rotation and scaling by k, perimeter must increase also in k times)\n";
    return 1;
  }
  auto ve = newAbfced.getVertices();
  std::reverse(ve.begin(), ve.end());
  std::rotate(ve.begin(), ve.begin() + 3, ve.end());
  newAbfced = Polygon(ve);
  if (!abfced.isSimilarTo(newAbfced)) {
    std::cerr << "Test 10 failed. (similarity of polygons)\n";
    return 1;
  }
  size_t sz = newAbfced.verticesCount();
  newAbfced.scale(a, 1. / 3);
  newAbfced.reflect(line1);
  if (!abfced.isCongruentTo(newAbfced)) {
    std::cerr << "Test 11 failed. (transformations or congruency of polygons)\n";
    return 1;
  }
  if (newAbfced == abfced) {
    std::cerr << "Test 12 failed. (transformations or equality of polygons)\n";
    return 1;
  }

  Point k(3, 1);
  Polygon bfkce(c, k, f, b, e);

  Rectangle rec_ae1 = Rectangle(e, a, 1);
  Square sq_ae = Square(a, e);

  const Shape &rec_ae1_shape = rec_ae1;
  const Shape &sq_ae_shape = sq_ae;
  size_t sx = rec_ae1.verticesCount();
  if (!(rec_ae1_shape == sq_ae_shape) || rec_ae1_shape != sq_ae_shape) {
    std::cerr << "Test 12.5 failed. (Square and rectangle may be equal, even if their static type is Shape)\n";
    return 1;
  }

  abd = Triangle(d, b, a);
  {
    bool ok = true;
    Circle incircle = abd.inscribedCircle();
    Circle circumcircle = abd.circumscribedCircle();
    Point inc = incircle.center();
    Point circ = circumcircle.center();
    double r = incircle.radius(), R = circumcircle.radius();
    // Euler theorem
    double t = (sqrt(pow((inc.x - circ.x), 2) + pow((inc.y - circ.y), 2)) - sqrt(R * R - 2 * R * r));
    ok = (sqrt(pow((inc.x - circ.x), 2) + pow((inc.y - circ.y), 2)) - sqrt(R * R - 2 * R * r)) <= 1e-6;
    if (!ok) {
      std::cerr << "Test 17.0 failed. (https://en.wikipedia.org/wiki/Euler's_theorem_in_geometry)\n";
      return 1;
    }
  }
  Circle eulerCircle = abd.ninePointsCircle();
  Line eulerLine = abd.EulerLine();
  Point q = abd.centroid();
  Point w = abd.ninePointsCircle().center();
  Line ser = Line(eulerCircle.center(), abd.centroid());
  bool ok = Line(eulerCircle.center(), abd.centroid()) == eulerLine;
  if (!ok) {
    std::cerr << "Test 17.4 failed. (centroid and nine-points circle center lie on Euler line)\n";
    return 1;
  }
  // Описанная окружность есть образ окружности девяти точек
  // относительно гомотетии с центром в ортоцентре и коэффициентом 2
  /*Circle anotherCircle = eulerCircle;
  anotherCircle.scale(orc, 2);
  ok = circumcircle == anotherCircle;
  if (!ok) {
    std::cerr << "Test 17.3 failed. (homothety of nine-points circle wrt orthocenter with k=2 gives circumcircle)\n";
    return 1;
  }

  // прямая Эйлера проходит через центроид и центр окружности девяти точек
  ok = Line(eulerCircle.center(), abd.centroid()) == eulerLine;

  if (!ok) {
    std::cerr << "Test 17.4 failed. (centroid and nine-points circle center lie on Euler line)\n";
    return 1;
  }
  */
}