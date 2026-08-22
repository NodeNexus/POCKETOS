#pragma once
namespace math_utils {
  template<typename T>
  T clamp(T val, T min_val, T max_val) {
    if (val < min_val) return min_val;
    if (val > max_val) return max_val;
    return val;
  }
  inline int abs_int(int a) {
    return a < 0 ? -a : a;
  }
  inline float lerp(float a, float b, float t) {
    return a + (b - a) * t;
  }
