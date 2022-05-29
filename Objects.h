#pragma once
#include "Utility.h"
#include "Engine.h"
#include "Geometry.h"
#include <vector>
#include <functional>
#include <list>

typedef int health_t;

class Object2d
{
  Vector2d pos_ = { 0, 0 };
  Vector2d vel_ = { 0, 0 };
  float angle_ = 0;
  float rotate_speed_ = 0;
  uint32_t color_ = COLOR::WHITE;
  std::vector<Vector2d> vertices_ = {};   // Vertices of geometric figure
public:
  Object2d() {}
  Object2d(const Vector2d& pos, const Vector2d& vel) : pos_(pos), vel_(vel) {}

  virtual void draw(uint32_t buffer[SCREEN_HEIGHT][SCREEN_WIDTH]) const;
  virtual void update(float dt);
  virtual void reset() {}

  void set_position(const Vector2d& pos);
  void set_velocity(const Vector2d& vel);
  void set_angle(float angle);
  void set_color(uint32_t color);
  void set_rotate_speed(float speed);
  void add_vertex(const Vector2d& vertex);
  void rotate(const Vector2d& r, float angle);

  Vector2d get_position() const;
  Vector2d get_velocity() const;
  float get_angle() const;
  uint32_t get_color() const;
  float get_rotate_speed() const;
  const std::vector<Vector2d>& get_vertices() const;

  bool is_intersect(const Object2d& object) const;
  virtual ~Object2d() {};
};

class Effect
{
  float delay_ = 0;
  float timer_ = 0;
  std::function<void()> effect_;
public:
  Effect(float delay, const std::function<void()>& effect)
    : delay_(delay), effect_(effect)
  {}
  void update(float dt);
  void apply_effect() const;
  bool is_ready() const;
};

class GameObject2d : public Object2d
{
  health_t health_ = 0;
  float vel_decay_ = 0;
  bool active_ = false;
  health_t damage_ = 0;
  std::list<Effect> effects_ = {};
public:
  GameObject2d() {}
  GameObject2d(const Vector2d& pos, const Vector2d& vel, health_t health, health_t damage, bool active)
    : Object2d(pos, vel), health_(health), active_(active), damage_(damage)
  {}

  //virtual void draw(uint32_t buffer[SCREEN_HEIGHT][SCREEN_WIDTH]) const override {};
  virtual void update(float dt) override;
  virtual void reset() override;

  void set_vel_decay(float decay);
  void set_active(bool active);
  void set_health(health_t health);
  void set_damage(health_t damage);
  void add_effect(float delay, const std::function<void()>& effect);

  health_t get_health() const;
  health_t get_damage() const;

  bool is_dead() const;
  bool is_active() const;
  ~GameObject2d() {}
};

class Player : public GameObject2d
{
  bool god_mode_ = false;
public:
  Player(const Vector2d& pos, const Vector2d& vel, health_t health);
  void update(float dt) override;
  void set_god_mode(bool god_mode);
  bool is_damageable() const;
};

class Enemy : public GameObject2d
{
  void init_vertices();
public:
  Enemy();
  Enemy(const Vector2d& pos, const Vector2d& vel, health_t health, health_t damage, bool active);
  void update(float dt) override;
};

class Projectile : public GameObject2d
{
  std::vector<const Enemy*> affected_enemies_ = std::vector<const Enemy*>();
  void init_vertices();
public:
  Projectile();
  Projectile(const Vector2d& pos, const Vector2d& vel, health_t health, health_t damage, bool active);

  void draw(uint32_t buffer[SCREEN_HEIGHT][SCREEN_WIDTH]) const override;
  void reset() override;
  void update(float dt) override;
  void add_affected_enemy(const Enemy& enemy);
  void clear_affected_enemies();
  bool is_enemy_affected(const Enemy& enemy) const;
};

class Score : public Object2d
{
  uint32_t score_ = 0;
  dim_t size_ = 20;
public:
  Score(const Vector2d& pos, uint32_t size) : Object2d(pos, { 0, 0 }), size_(size) {}
  virtual void draw(uint32_t buffer[SCREEN_HEIGHT][SCREEN_WIDTH]) const override;
  virtual void update(float dt) override {}

  void set_score(uint32_t score);

  uint32_t get_score() const;
};
