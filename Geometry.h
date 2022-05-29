#pragma once
#include "Engine.h"
#include "Utility.h"
#include <vector>

#define BORDER_CHECK(x, y) (x > 0 && x < SCREEN_WIDTH && y > 0 && y < SCREEN_HEIGHT)

enum COLOR
{
  RED = 0xff0000,
  GREEN = 0x00ff00,
  BLUE = 0x0000ff,
  WHITE = 0xffffff,
};

enum class DigitSegment
{
  a,    //    |-a-|
  b,    //    f   b
  c,    //    |-g-|
  d,    //    e   c 
  e,    //    |-d-|
  f,
  g,
};

class Geometry
{
public:
  static void draw_rectangle(uint32_t buffer[SCREEN_HEIGHT][SCREEN_WIDTH],
    const Vector2d& pos, dim_t hl, dim_t hw, float angle, uint32_t color);
  static void draw_fill_rectangle(uint32_t buffer[SCREEN_HEIGHT][SCREEN_WIDTH],
    const Vector2d& pos, dim_t h, dim_t w, uint32_t color);
  static void draw_triangle(uint32_t buffer[SCREEN_HEIGHT][SCREEN_WIDTH],
      const Vector2d& pos, dim_t size, float angle, uint32_t color);
  static void draw_circle(uint32_t buffer[SCREEN_HEIGHT][SCREEN_WIDTH],
    const Vector2d& pos, dim_t r, uint32_t color);
  static void draw_line(uint32_t buffer[SCREEN_HEIGHT][SCREEN_WIDTH],
      const Vector2d& pos1, const Vector2d& pos2, uint32_t color);
  static void draw_digit(uint32_t buffer[SCREEN_HEIGHT][SCREEN_WIDTH],
    const Vector2d& pos, uint32_t digit, dim_t size, uint32_t color);
  static void draw_segment(uint32_t buffer[SCREEN_HEIGHT][SCREEN_WIDTH],
  const Vector2d& pos, DigitSegment segment, dim_t size, uint32_t color);
  static Vector2d get_axis_projection(const std::vector<Vector2d>& vertices, const Vector2d& axis);
  static bool is_intersect(const std::vector<Vector2d>& vertices1,
    const std::vector<Vector2d>& vertices2);
};
