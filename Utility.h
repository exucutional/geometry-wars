#pragma once
#include<cmath>
#include <cstdint>

#define PI 3.14159265

typedef float dim_t;

class Vector2d
{
public:
  dim_t x;
  dim_t y;
  Vector2d() : x(0), y(0) {}
  Vector2d(dim_t x, dim_t y) : x(x), y(y) {}
  Vector2d(const Vector2d& vec) : x(vec.x), y(vec.y) {}
  Vector2d(Vector2d&& vec) noexcept : x(vec.x), y(vec.y) {}
  Vector2d& operator=(const Vector2d& vec) { return *this = Vector2d(vec); }
  Vector2d& operator=(Vector2d&& vec) noexcept { x = vec.x; y = vec.y; return *this; }
  Vector2d operator+ (const Vector2d& other) const { return { x + other.x, y + other.y }; }
  Vector2d operator- (const Vector2d& other) const { return { x - other.x, y - other.y }; }
  Vector2d operator* (dim_t value) const { return { x * value, y * value }; }
  dim_t operator* (const Vector2d& other) const { return x * other.x + y * other.y; }
  Vector2d operator/ (dim_t value) const { return { x / value, y / value }; }
  Vector2d rotate(const Vector2d& r, float angle) const
  {
    Vector2d r0 = Vector2d(x - r.x, y - r.y);
    dim_t r0_x = r0.x * cos(angle) - r0.y * sin(angle);
    dim_t r0_y = r0.x * sin(angle) + r0.y * cos(angle);
    return { r.x + r0_x, r.y + r0_y };
  }
  Vector2d get_normalized() const
  {
    dim_t mag = sqrt(x * x + y * y);
    if (mag != 0)
      return { x / mag, y / mag };

    return { 0, 0 };
  }
  Vector2d get_norm() const
  {
    return { y, -x };
  }
  dim_t get_magnitude() const
  {
    return sqrt(x * x + y * y);
  }
  ~Vector2d() {}
};
