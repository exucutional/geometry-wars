#include "Objects.h"
#include "Geometry.h"
#include <algorithm>

Vector2d Object2d::get_position() const
{
  return pos_;
}

Vector2d Object2d::get_velocity() const
{
  return vel_;
}

float Object2d::get_angle() const
{
  return angle_;
}

void Object2d::set_position(const Vector2d& pos)
{
  Vector2d r = pos - pos_;
  pos_ = pos;
  std::transform(vertices_.begin(), vertices_.end(), vertices_.begin(),
    [&](Vector2d& v) { return v + r; });
}

void Object2d::set_velocity(const Vector2d& vel)
{
  vel_ = vel;
}

void Object2d::set_angle(float angle)
{
  angle_ = angle;
}

bool Object2d::is_intersect(const Object2d& object) const
{
  return Geometry::is_intersect(vertices_, object.vertices_);
}

void Object2d::set_color(uint32_t color)
{
  color_ = color;
}

void Object2d::set_rotate_speed(float speed)
{
  rotate_speed_ = speed;
}

uint32_t Object2d::get_color() const
{
  return color_;
}

void Object2d::add_vertex(const Vector2d& vertex)
{
  vertices_.push_back(vertex);
}

void Object2d::draw(uint32_t buffer[SCREEN_HEIGHT][SCREEN_WIDTH]) const
{
  for (size_t i = 1; i < vertices_.size(); ++i)
    Geometry::draw_line(buffer, vertices_[i - 1], vertices_[i], color_);

  Geometry::draw_line(buffer, vertices_.front(), vertices_.back(), color_);
}

void Object2d::rotate(const Vector2d& r, float angle)
{
  angle_ += angle;
  std::transform(vertices_.begin(), vertices_.end(), vertices_.begin(),
    [&](Vector2d& v) { return v.rotate(r, angle); });
}

void Object2d::update(float dt)
{
  Vector2d vel = get_velocity();
  Vector2d pos = get_position();
  rotate(pos, dt * rotate_speed_);
  set_position({ pos.x + dt * vel.x, pos.y + dt * vel.y });
}

const std::vector<Vector2d>& Object2d::get_vertices() const
{
  return vertices_;
}

float Object2d::get_rotate_speed() const
{
  return rotate_speed_;
}

void GameObject2d::update(float dt)
{
  Object2d::update(dt);
  Vector2d vel = get_velocity();
  dim_t decay = vel.get_magnitude() - dt * vel_decay_;
  Vector2d norm = vel.get_normalized();
  set_velocity({ norm.x * decay, norm.y * decay });
  if (!effects_.empty())
  {
    for (auto& e : effects_)
    {
      e.update(dt);
      if (e.is_ready())
        e.apply_effect();
    }
    auto fresh_eff_end = std::remove_if(effects_.begin(), effects_.end(),
      [&](Effect& e) { return e.is_ready(); });
    effects_.erase(fresh_eff_end, effects_.end());
    auto c = effects_.size();
  }
}

void GameObject2d::reset()
{
  set_color(COLOR::WHITE);
  set_rotate_speed(0);
  effects_.clear();
}

void GameObject2d::set_health(health_t health)
{
  health_ = health;
}

void GameObject2d::set_damage(health_t damage)
{
  damage_ = damage;
}

health_t GameObject2d::get_health() const
{
  return health_;
}

health_t GameObject2d::get_damage() const
{
  return damage_;
}

bool GameObject2d::is_dead() const
{
  return health_ <= 0;
}

bool GameObject2d::is_active() const
{
  return active_;
}

void GameObject2d::set_vel_decay(float decay)
{
  vel_decay_ = decay;
}

void GameObject2d::set_active(bool active)
{
  active_ = active;
}

void GameObject2d::add_effect(float delay, const std::function<void()>& effect)
{
  effects_.emplace_back(delay, effect);
}

Player::Player(const Vector2d& pos, const Vector2d& vel, health_t health)
  : GameObject2d(pos, vel, health, 0, true)
{
  dim_t size = 50;
  float R = size / sqrt(3);
  add_vertex({ pos.x, pos.y - R });
  add_vertex({ pos.x + size / 2, pos.y + R / 2 });
  add_vertex({ pos.x - size / 2, pos.y + R / 2 });
}

void Player::set_god_mode(bool god_mode)
{
  god_mode_ = god_mode;
}

bool Player::is_damageable() const
{
  return !god_mode_;
}

void Player::update(float dt)
{
  Vector2d vel = get_velocity();
  for (const auto& vertex : get_vertices())
  {
    Vector2d vertex_next_pos = vertex + vel * dt;
    if (vertex_next_pos.x < 0 && vel.x < 0)
      vel.x = 0;
    if (vertex_next_pos.x > SCREEN_WIDTH && vel.x > 0)
      vel.x = 0;
    if (vertex_next_pos.y < 0 && vel.y < 0)
      vel.y = 0;
    if (vertex_next_pos.y > SCREEN_HEIGHT && vel.y > 0)
      vel.y = 0;
  }
  set_velocity(vel);
  GameObject2d::update(dt);
}

void Projectile::draw(uint32_t buffer[SCREEN_HEIGHT][SCREEN_WIDTH]) const
{
  Geometry::draw_circle(buffer, get_position(), 5, get_color());
}

Enemy::Enemy()
{
  init_vertices();
}

Enemy::Enemy(const Vector2d& pos, const Vector2d& vel,
  health_t health, health_t damage, bool active)
  : GameObject2d(pos, vel, health, damage, active)
{
  init_vertices();
}

void Enemy::init_vertices()
{
  dim_t size = 15;
  Vector2d pos = get_position();
  add_vertex({ pos.x - size, pos.y - size });
  add_vertex({ pos.x + size, pos.y - size });
  add_vertex({ pos.x + size, pos.y + size });
  add_vertex({ pos.x - size, pos.y + size });
}

void Enemy::update(float dt)
{
  GameObject2d::update(dt);
  Vector2d pos = get_position();
  if (!BORDER_CHECK(pos.x, pos.y))
    set_active(false);
}

Projectile::Projectile()
{
  affected_enemies_.reserve(10);
  init_vertices();
}

Projectile::Projectile(const Vector2d& pos, const Vector2d& vel,
  health_t health, health_t damage, bool active)
  : GameObject2d(pos, vel, health, damage, active)
{
  affected_enemies_.reserve(10);
  init_vertices();
}

void Projectile::init_vertices()
{
  dim_t size = 5;
  Vector2d pos = get_position();
  add_vertex({ pos.x - size, pos.y - size });
  add_vertex({ pos.x + size, pos.y - size });
  add_vertex({ pos.x + size, pos.y + size });
  add_vertex({ pos.x - size, pos.y + size });
}

void Projectile::update(float dt)
{
  GameObject2d::update(dt);
  Vector2d pos = get_position();
  if (!BORDER_CHECK(pos.x, pos.y))
    set_active(false);
}

void Projectile::reset()
{
  clear_affected_enemies();
}

void Projectile::add_affected_enemy(const Enemy& enemy)
{
  affected_enemies_.push_back(&enemy);
}

void Projectile::clear_affected_enemies()
{
  affected_enemies_.clear();
}

bool Projectile::is_enemy_affected(const Enemy& enemy) const
{
  return std::any_of(affected_enemies_.cbegin(), affected_enemies_.cend(),
    [&](const Enemy* e) { return e == &enemy; });
}


void Effect::update(float dt)
{
  timer_ += dt;
}

bool Effect::is_ready() const
{
  return timer_ >= delay_;
}

void Effect::apply_effect() const
{
  effect_();
}

void Score::draw(uint32_t buffer[SCREEN_HEIGHT][SCREEN_WIDTH]) const
{
  uint32_t factor = 1;
  uint32_t i = 0;
  Vector2d pos = get_position();
  do
  {
    uint32_t digit = score_ % (factor * 10) / factor;
    Geometry::draw_digit(buffer, { dim_t(pos.x - 1.5 * size_ * i++), pos.y },
      digit, size_, get_color());
    factor *= 10;
  } while (score_ / factor != 0);
}

void Score::set_score(uint32_t score)
{
  score_ = score;
}

uint32_t Score::get_score() const
{
  return score_;
}
