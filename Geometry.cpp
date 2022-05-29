#include "Geometry.h"
#include <cmath>
#include <vector>
#include <stdexcept>
#include <algorithm>

void Geometry::draw_rectangle(uint32_t buffer[SCREEN_HEIGHT][SCREEN_WIDTH],
  const Vector2d& pos, dim_t hl, dim_t hw, float angle, uint32_t color)
{
  Vector2d v1 = Vector2d(pos.x - hl, pos.y - hw).rotate(pos, angle);
  Vector2d v2 = Vector2d(pos.x + hl, pos.y - hw).rotate(pos, angle);
  Vector2d v3 = Vector2d(pos.x + hl, pos.y + hw).rotate(pos, angle);
  Vector2d v4 = Vector2d(pos.x - hl, pos.y + hw).rotate(pos, angle);
  draw_line(buffer, v1, v2, color);
  draw_line(buffer, v2, v3, color);
  draw_line(buffer, v3, v4, color);
  draw_line(buffer, v4, v1, color);
}

void Geometry::draw_fill_rectangle(uint32_t buffer[SCREEN_HEIGHT][SCREEN_WIDTH],
  const Vector2d& pos, dim_t h, dim_t w, uint32_t color)
{
  for (dim_t y = pos.y; y < pos.y + h; ++y)
  {
    for (dim_t x = pos.x; x < pos.x + w; ++x)
    {
      if (BORDER_CHECK(x, y))
        buffer[uint32_t(y)][uint32_t(x)] = color;
    }
  }
}

void Geometry::draw_triangle(uint32_t buffer[SCREEN_HEIGHT][SCREEN_WIDTH],
  const Vector2d& pos, dim_t size, float angle, uint32_t color)
{
  float R = size / sqrt(3);
  Vector2d v1 = Vector2d(pos.x, pos.y - R).rotate(pos, angle);
  Vector2d v2 = Vector2d(pos.x + size / 2, pos.y + R / 2).rotate(pos, angle);
  Vector2d v3 = Vector2d(pos.x - size / 2, pos.y + R / 2).rotate(pos, angle);
  draw_line(buffer, v1, v3, color);
  draw_line(buffer, v1, v2, color);
  draw_line(buffer, v2, v3, color);
}

void Geometry::draw_circle(uint32_t buffer[SCREEN_HEIGHT][SCREEN_WIDTH],
  const Vector2d& pos, dim_t r, uint32_t color)
{
  int rr = r * r;
  for (int y = 0; y < r; ++y)
  {
    int yy = y * y;
    for (int x = 0; x < r; ++x)
    {
      if (x * x + yy <= rr)
      {
        if (BORDER_CHECK(pos.x + x, pos.y + y))
          buffer[uint32_t(pos.y + y)][uint32_t(pos.x + x)] = color;
        if (BORDER_CHECK(pos.x + x, pos.y - y))
          buffer[uint32_t(pos.y - y)][uint32_t(pos.x + x)] = color;
        if (BORDER_CHECK(pos.x - x, pos.y + y))
          buffer[uint32_t(pos.y + y)][uint32_t(pos.x - x)] = color;
        if (BORDER_CHECK(pos.x - x, pos.y - y))
          buffer[uint32_t(pos.y - y)][uint32_t(pos.x - x)] = color;
      }
    }
  }
}

void Geometry::draw_line(uint32_t buffer[SCREEN_HEIGHT][SCREEN_WIDTH],
  const Vector2d& pos1, const Vector2d& pos2, uint32_t color)
{
  dim_t xerr = 0;
  dim_t yerr = 0;
  dim_t dx = pos2.x - pos1.x;
  dim_t x_dir = (dx > 0) ? 1 : -1;
  dim_t abs_dx = (dx > 0) ? dx : -dx;
  dim_t dy = pos2.y - pos1.y;
  dim_t y_dir = (dy > 0) ? 1 : -1;
  dim_t abs_dy = (dy > 0) ? dy : -dy;
  dim_t max_dx_dy = (abs_dx > abs_dy) ? abs_dx : abs_dy;
  dim_t x = pos1.x;
  dim_t y = pos1.y;
  if (BORDER_CHECK(x, y))
    buffer[uint32_t(y)][uint32_t(x)] = color;
  for (dim_t i = 0; i < max_dx_dy; ++i)
  {
    xerr += abs_dx;
    if (xerr >= max_dx_dy)
    {
      xerr -= max_dx_dy;
      x += x_dir;
    }
    yerr += abs_dy;
    if (yerr >= max_dx_dy)
    {
      yerr -= max_dx_dy;
      y += y_dir;
    }
    if (BORDER_CHECK(x, y))
      buffer[uint32_t(y)][uint32_t(x)] = color;
  }
}

void Geometry::draw_digit(uint32_t buffer[SCREEN_HEIGHT][SCREEN_WIDTH],
  const Vector2d& pos, uint32_t digit, dim_t size, uint32_t color)
{
  switch (digit)
  {
  case 0:
    draw_segment(buffer, pos, DigitSegment::a, size, color);
    draw_segment(buffer, pos, DigitSegment::b, size, color);
    draw_segment(buffer, pos, DigitSegment::c, size, color);
    draw_segment(buffer, pos, DigitSegment::d, size, color);
    draw_segment(buffer, pos, DigitSegment::e, size, color);
    draw_segment(buffer, pos, DigitSegment::f, size, color);
    break;
  case 1:
    draw_segment(buffer, pos, DigitSegment::b, size, color);
    draw_segment(buffer, pos, DigitSegment::c, size, color);
    break;
  case 2:
    draw_segment(buffer, pos, DigitSegment::a, size, color);
    draw_segment(buffer, pos, DigitSegment::b, size, color);
    draw_segment(buffer, pos, DigitSegment::g, size, color);
    draw_segment(buffer, pos, DigitSegment::e, size, color);
    draw_segment(buffer, pos, DigitSegment::d, size, color);
    break;
  case 3:
    draw_segment(buffer, pos, DigitSegment::a, size, color);
    draw_segment(buffer, pos, DigitSegment::b, size, color);
    draw_segment(buffer, pos, DigitSegment::g, size, color);
    draw_segment(buffer, pos, DigitSegment::c, size, color);
    draw_segment(buffer, pos, DigitSegment::d, size, color);
    break;
  case 4:
    draw_segment(buffer, pos, DigitSegment::f, size, color);
    draw_segment(buffer, pos, DigitSegment::g, size, color);
    draw_segment(buffer, pos, DigitSegment::b, size, color);
    draw_segment(buffer, pos, DigitSegment::c, size, color);
    break;
  case 5:
    draw_segment(buffer, pos, DigitSegment::a, size, color);
    draw_segment(buffer, pos, DigitSegment::f, size, color);
    draw_segment(buffer, pos, DigitSegment::g, size, color);
    draw_segment(buffer, pos, DigitSegment::c, size, color);
    draw_segment(buffer, pos, DigitSegment::d, size, color);
    break;
  case 6:
    draw_segment(buffer, pos, DigitSegment::a, size, color);
    draw_segment(buffer, pos, DigitSegment::f, size, color);
    draw_segment(buffer, pos, DigitSegment::g, size, color);
    draw_segment(buffer, pos, DigitSegment::c, size, color);
    draw_segment(buffer, pos, DigitSegment::d, size, color);
    draw_segment(buffer, pos, DigitSegment::e, size, color);
    break;
  case 7:
    draw_segment(buffer, pos, DigitSegment::a, size, color);
    draw_segment(buffer, pos, DigitSegment::b, size, color);
    draw_segment(buffer, pos, DigitSegment::c, size, color);
    break;
  case 8:
    draw_segment(buffer, pos, DigitSegment::a, size, color);
    draw_segment(buffer, pos, DigitSegment::b, size, color);
    draw_segment(buffer, pos, DigitSegment::c, size, color);
    draw_segment(buffer, pos, DigitSegment::d, size, color);
    draw_segment(buffer, pos, DigitSegment::e, size, color);
    draw_segment(buffer, pos, DigitSegment::f, size, color);
    draw_segment(buffer, pos, DigitSegment::g, size, color);
    break;
  case 9:
    draw_segment(buffer, pos, DigitSegment::a, size, color);
    draw_segment(buffer, pos, DigitSegment::b, size, color);
    draw_segment(buffer, pos, DigitSegment::c, size, color);
    draw_segment(buffer, pos, DigitSegment::d, size, color);
    draw_segment(buffer, pos, DigitSegment::f, size, color);
    draw_segment(buffer, pos, DigitSegment::g, size, color);
    break;
  default:
    throw std::invalid_argument("Digit must be in range [0, 9]");
    break;
  }
}

void Geometry::draw_segment(uint32_t buffer[SCREEN_HEIGHT][SCREEN_WIDTH],
  const Vector2d& pos, DigitSegment segment, dim_t size, uint32_t color)
{
  switch (segment)
  {
  case DigitSegment::a:
    draw_line(buffer, pos, { pos.x - size, pos.y }, color);
    break;
  case DigitSegment::b:
    draw_line(buffer, pos, { pos.x, pos.y + size }, color);
    break;
  case DigitSegment::c:
    draw_line(buffer, { pos.x, pos.y + size }, { pos.x , pos.y + 2 * size }, color);
    break;
  case DigitSegment::d:
    draw_line(buffer, { pos.x , pos.y + 2 * size }, { pos.x - size, pos.y + 2 * size }, color);
    break;
  case DigitSegment::e:
    draw_line(buffer, { pos.x - size, pos.y + size }, { pos.x - size, pos.y + 2 * size }, color);
    break;
  case DigitSegment::f:
    draw_line(buffer, { pos.x - size , pos.y }, { pos.x - size, pos.y + size }, color);
    break;
  case DigitSegment::g:
    draw_line(buffer, { pos.x , pos.y + size }, { pos.x - size, pos.y + size }, color);
    break;
  }
}

Vector2d Geometry::get_axis_projection(const std::vector<Vector2d>& vertices, const Vector2d& axis)
{
  std::vector<dim_t> projections = std::vector<dim_t>(vertices.size());
  std::transform(vertices.cbegin(), vertices.cend(), projections.begin(),
    [&](const Vector2d& v) { return v * axis; });
  const auto minmax = std::minmax_element(projections.cbegin(), projections.cend());
  return { *minmax.first, *minmax.second };
}

bool Geometry::is_intersect(const std::vector<Vector2d>& vertices1,
  const std::vector<Vector2d>& vertices2)
{
  std::vector<Vector2d> perpendicularAxes = {};
  perpendicularAxes.reserve(vertices1.size() + vertices2.size());
  for (size_t i = 1; i < vertices1.size(); ++i)
    perpendicularAxes.emplace_back((vertices1[i] - vertices1[i - 1]).get_normalized().get_norm());

  perpendicularAxes.emplace_back((vertices1.front() - vertices1.back()).get_normalized().get_norm());
  for (size_t i = 1; i < vertices2.size(); ++i)
    perpendicularAxes.emplace_back((vertices2[i] - vertices2[i - 1]).get_normalized().get_norm());

  perpendicularAxes.emplace_back((vertices2.front() - vertices2.back()).get_normalized().get_norm());

  bool intersect = std::all_of(perpendicularAxes.cbegin(), perpendicularAxes.cend(),
    [&](const Vector2d& axis)
  {
    Vector2d project1 = get_axis_projection(vertices1, axis);
    Vector2d project2 = get_axis_projection(vertices2, axis);
    return project1.x <= project2.y && project1.y >= project2.x;
  });

  return intersect;
}
