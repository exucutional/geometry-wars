#pragma once
#include <vector>
#include "Objects.h"

enum class Event
{
  RandomSpawnEnemiesTargetPlayer,
  FastSpawnEnemiesTargetPlayer,
  BurstEnemiesTargetPlayer,
};

class Game
{
  health_t player_health_ = 3;
  Vector2d player_init_pos_ = { SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2, };
  Vector2d player_init_vel_ = { 0, 0 };
  //dim_t player_size_ = 50 / sqrt(3);
  dim_t player_vel_ = 200;
  float player_vel_decay_ = 1000;
  float player_shoot_cooldown_ = 0.25;
  float player_shoot_cooldown_acc_ = 0;

  float enemy_spawn_rate_ = 10;
  health_t enemy_health_ = 3;
  health_t enemy_damage_ = 1;
  dim_t enemy_vel_ = 500;
  dim_t enemy_spawn_min_distance_ = 300;
  float enemy_random_spawn_cooldown_ = 1;
  float enemy_random_spawn_event_duration_ = 10;
  float enemy_fast_spawn_cooldown_ = 0.2;
  float enemy_fast_spawn_duration_ = 2;
  float enemy_fast_spawn_event_duration_ = 5;
  float enemy_burst_cooldown_ = 5;
  float enemy_burst_size_ = 20;
  float enemy_burst_rate_ = 0.1;
  float enemy_burst_event_duration_ = 10;
  float enemy_spawn_cooldown_acc_ = 0;
  float enemy_event_cooldown = 10;
  //dim_t enemy_size_ = 25;

  dim_t projectile_vel_ = 700;
  health_t projectile_health_ = 1;
  health_t projectile_damage_ = 1;
  //dim_t projectile_size_ = 5;

  dim_t health_size = 50;
  dim_t score_size_ = 30;
  Vector2d score_pos_ = { SCREEN_WIDTH - 50, 20 };

  Player player_;
  Score score_;
  Event event_ = Event::RandomSpawnEnemiesTargetPlayer;
  float event_time_ = 0;
  std::list<GameObject2d> particles_ = std::list<GameObject2d>();
  std::vector<Projectile> projectiles_ = std::vector<Projectile>(100);
  std::vector<Enemy> enemies_ = std::vector<Enemy>(100);
public:
  Game();
  void control(float dt);
  void update(float dt);
  void update_event(float dt);
  void draw(uint32_t buffer[SCREEN_HEIGHT][SCREEN_WIDTH]) const;
  void shoot();
  void reset();
  void spawn_enemy(const Vector2d& pos);
  void spawn_particle(const Vector2d& vertex1, const Vector2d& vertex2, const GameObject2d& obj,
    float life_time);
  void destroy_object(GameObject2d& obj, float destroy_time);
  template<typename T>
  T& spawn_object(std::vector<T>& container, const Vector2d& pos, const Vector2d& vel,
    health_t health, health_t damage, bool active);
};

template<typename T>
T& Game::spawn_object(std::vector<T>& container, const Vector2d& pos, const Vector2d& vel,
  health_t health, health_t damage, bool active)
{
  auto object = std::find_if(container.begin(), container.end(),
    [](const T& object) { return !object.is_active(); });
  if (object != std::end(container))
  {
    object->reset();
    object->set_position(pos);
    object->set_velocity(vel);
    object->set_health(health);
    object->set_damage(damage);
    object->set_active(active);
    return *object;
  }
  else
  {
    container.emplace_back(pos, vel, health, damage, active);
    return container.back();
  }
}
