#pragma once
namespace geom {
  struct Point {
    int x;
    int y;
    Point() : x(0), y(0) {}
    Point(int x, int y) : x(x), y(y) {}
  };
  struct Size {
    int width;
    int height;
    Size() : width(0), height(0) {}
    Size(int w, int h) : width(w), height(h) {}
  };
  struct Rect {
    int x, y;
    int w, h;
    Rect() : x(0), y(0), w(0), h(0) {}
    Rect(int x, int y, int w, int h) : x(x), y(y), w(w), h(h) {}
    bool contains(const Point& p) const {
      return p.x >= x && p.x < x + w && p.y >= y && p.y < y + h;
    }
