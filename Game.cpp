#include "Engine.h"
#include <stdlib.h>
#include <memory.h>
#include <algorithm>
#include <random>
#include "Geometry.h"
#include "Game.h"
//
//  You are free to modify this file
//

//  is_key_pressed(int button_vk_code) - check if a key is pressed,
//                                       use keycodes (VK_SPACE, VK_RIGHT, VK_LEFT, VK_UP, VK_DOWN, 'A', 'B')
//
//  get_cursor_x(), get_cursor_y() - get mouse cursor position
//  is_mouse_button_pressed(int button) - check if mouse button is pressed (0 - left button, 1 - right button)
//  clear_buffer() - set all pixels in buffer to 'black'
//  is_window_active() - returns true if window is active
//  schedule_quit_game() - quit game after act()

std::random_device rd{};
std::mt19937 gen{ rd() };
std::normal_distribution<> normal_rnd{ 0, 2 };
Game game = Game();

// initialize game data in this function
void initialize()
{}

// this function is called to update game data,
// dt - time elapsed since the previous update (in seconds)
void act(float dt)
{
  game.control(dt);
  game.update(dt);
  if (is_key_pressed(VK_ESCAPE))
    schedule_quit_game();
}

// fill buffer in this function
// uint32_t buffer[SCREEN_HEIGHT][SCREEN_WIDTH] - is an array of 32-bit colors (8 bits per R, G, B)
void draw()
{
  // clear backbuffer
  memset(buffer, 0, SCREEN_HEIGHT * SCREEN_WIDTH * sizeof(uint32_t));
  //Geometry::draw_circle(buffer, { SCREEN_HEIGHT/2, SCREEN_WIDTH/2 }, 100, COLOR::WHITE);
  //Geometry::draw_line(buffer, { 10, 10 }, { 1000, 100 }, COLOR::WHITE);
  //Geometry::draw_triangle(buffer, { 100, 100 }, 100, 45, COLOR::WHITE);
  game.draw(buffer);
}

// free game data in this function
void finalize()
{}

void Game::control(float dt)
{
  static bool mouse_rbutton_pressed = false;

  if (player_.is_dead())
    return;

  if (is_key_pressed(VK_LEFT))
    player_.set_velocity({ -player_vel_, player_.get_velocity().y });

  if (is_key_pressed(VK_RIGHT))
    player_.set_velocity({ player_vel_, player_.get_velocity().y });

  if (is_key_pressed(VK_UP))
    player_.set_velocity({ player_.get_velocity().x, -player_vel_ });

  if (is_key_pressed(VK_DOWN))
    player_.set_velocity({ player_.get_velocity().x, player_vel_ });

  if (is_mouse_button_pressed(1) & !mouse_rbutton_pressed)
  {
    Vector2d pos = { dim_t(get_cursor_x()), dim_t(get_cursor_y()) };
    //spawn_enemy(pos);
    mouse_rbutton_pressed = true;
  }

  if (!is_mouse_button_pressed(1))
  {
    mouse_rbutton_pressed = false;
  }

  if (is_mouse_button_pressed(0) & !player_shoot_cooldown_acc_)
  {
    player_shoot_cooldown_acc_ = player_shoot_cooldown_;
    shoot();
  }

  Vector2d player_pos = player_.get_position();
  Vector2d direction = { get_cursor_x() - player_pos.x , get_cursor_y() - player_pos.y };
  float angle = -atan(direction.x / direction.y);
  player_.rotate(player_pos, -player_.get_angle());
  player_.rotate(player_pos, direction.y < 0 ? angle : PI + angle);
}

Game::Game()
  : player_(player_init_pos_, player_init_vel_, player_health_),
  score_(score_pos_, score_size_)
{
  player_.set_vel_decay(player_vel_decay_);
}

void Game::draw(uint32_t buffer[SCREEN_HEIGHT][SCREEN_WIDTH]) const
{
  if (player_.is_active())
    player_.draw(buffer);

  score_.draw(buffer);
  for (const auto& enemy : enemies_)
  {
    if (enemy.is_active())
      enemy.draw(buffer);
  }
  for (const auto& projectile : projectiles_)
  {
    if (projectile.is_active())
      projectile.draw(buffer);
  }
  for (const auto& particle : particles_)
  {
    if (particle.is_active())
      particle.draw(buffer);
  }
  for (int i = 0; i < player_.get_health(); i++)
  {
    Geometry::draw_fill_rectangle(buffer, { dim_t(20 + 1.2 * i * health_size), 20 },
      health_size, health_size, COLOR::RED);
  }
}

void Game::update(float dt)
{
  player_.update(dt);
  update_event(dt);

  for (auto& projectile : projectiles_)
  {
    if (projectile.is_active())
    {
      projectile.update(dt);
      std::for_each(enemies_.begin(), enemies_.end(),
        [&](Enemy& enemy)
      {
        if (enemy.is_active()
          && !enemy.is_dead()
          && !projectile.is_enemy_affected(enemy)
          && projectile.is_intersect(enemy))
        {
          enemy.set_health(enemy.get_health() - projectile.get_damage());
          enemy.set_color(COLOR::RED);
          enemy.add_effect(0.1, [&]() { enemy.set_color(COLOR::WHITE); });
          projectile.set_health(projectile.get_health() - enemy.get_damage());
          projectile.add_affected_enemy(enemy);

          if (enemy.is_dead())
          {
            enemy.set_active(false);
            score_.set_score(score_.get_score() + 1);
            destroy_object(enemy, 1);
          }
        }
      });
    }
    if (projectile.is_dead())
      projectile.set_active(false);
  }

  for (auto& enemy : enemies_)
  {
    if (enemy.is_active())
    {
      enemy.update(dt);
      if (player_.is_active()
        && player_.is_damageable()
        && player_.is_intersect(enemy))
      {
        player_.set_health(player_.get_health() - enemy.get_damage());
        player_.set_god_mode(true);
        player_.set_color(COLOR::RED);
        player_.add_effect(0.5, [&]() { player_.set_color(COLOR::WHITE); });
        player_.add_effect(0.5, [&]() { player_.set_god_mode(false); });
        if (player_.is_dead())
        {
          player_.set_velocity(player_.get_velocity().get_normalized() * 10);
          destroy_object(player_, 5);
          player_.set_active(false);
          player_.add_effect(5, [&]() { game.reset(); });;
        }
      }
    }
  }

  for (auto& particle : particles_)
  {
    particle.update(dt);
  }

  auto dead_particle_start = std::remove_if(particles_.begin(), particles_.end(),
    [](const GameObject2d& particle) { return !particle.is_active(); });

  particles_.erase(dead_particle_start, particles_.end());

  if (player_shoot_cooldown_acc_ - dt > 0)
    player_shoot_cooldown_acc_ -= dt;
  else
    player_shoot_cooldown_acc_ = 0;
}

void Game::shoot()
{
  Vector2d player_pos = player_.get_position();
  Vector2d direction = { get_cursor_x() - player_pos.x , get_cursor_y() - player_pos.y };
  Vector2d vel = direction.get_normalized() * projectile_vel_;
  Projectile& projectile = spawn_object<Projectile>(projectiles_, player_pos, vel,
    projectile_health_, projectile_damage_, true);
}

void Game::reset()
{
  for (auto& projectile : projectiles_)
  {
    projectile.set_active(false);
  }
  for (auto& enemy : enemies_)
  {
    enemy.set_active(false);
  }
  player_.set_health(player_health_);
  player_.rotate(player_.get_position(), -player_.get_angle());
  player_.set_position(player_init_pos_);
  player_.set_velocity(player_init_vel_);
  player_.set_active(true);
  score_.set_score(0);
  event_ = Event::RandomSpawnEnemiesTargetPlayer;
  event_time_ = 0;
}

void Game::spawn_enemy(const Vector2d& pos)
{
  Enemy& object = spawn_object<Enemy>(enemies_, pos, { 0, 0 }, enemy_health_, enemy_damage_, true);
  float rotate_dir = (player_.get_position().x < pos.x) ? -1 : 1;
  object.set_rotate_speed(5 * rotate_dir);
  object.add_effect(1, [&, rotate_dir]() { object.set_rotate_speed(10 * rotate_dir); });
  object.add_effect(2, [&, rotate_dir]() { object.set_rotate_speed(15 * rotate_dir); });
  object.add_effect(3, [&, rotate_dir]() { object.set_rotate_speed(20 * rotate_dir); });
  object.add_effect(3,
    [&]()
  {
    Vector2d player_pos = player_.get_position();
    //Vector2d player_vel = player_.get_velocity();
    Vector2d enemy_pos = object.get_position();
    Vector2d direction = player_pos - enemy_pos;
    //float travel_time = direction.get_magnitude() / enemy_vel_;
    float random = normal_rnd(gen) * 50;
    Vector2d variation = direction.get_norm().get_normalized() * random;
    Vector2d new_direction = direction + variation;
    object.set_velocity(new_direction.get_normalized() * enemy_vel_);
  });
}

void Game::destroy_object(GameObject2d& obj, float destroy_time)
{
  auto& vertices = obj.get_vertices();
  for (size_t i = 1; i < vertices.size(); ++i)
  {
    spawn_particle(vertices[i], vertices[i - 1], obj, destroy_time);
  }
  spawn_particle(vertices.front(), vertices.back(), obj, destroy_time);
}

void Game::spawn_particle(const Vector2d& vertex1, const Vector2d& vertex2, const GameObject2d& obj,
  float life_time)
{
  Vector2d random_vel = { dim_t(rand() % 100), dim_t(rand() % 100) };
  Vector2d pos = obj.get_position();
  Vector2d centre = (vertex1 + vertex2) / 2;
  Vector2d momentum = centre - pos;
  Vector2d rotate_vel = (vertex1 - vertex2) * 0.1 * obj.get_rotate_speed();
  float momentum_weight = 5;
  particles_.emplace_front(centre,
    obj.get_velocity() + momentum.get_normalized() * momentum_weight + rotate_vel, 0, 0, true);
  GameObject2d& particle = particles_.front();
  particle.add_vertex(vertex1);
  particle.add_vertex(vertex2);
  particle.set_rotate_speed(obj.get_rotate_speed() / 8);
  particle.add_effect(life_time, [&]() { particle.set_active(false); });
}

void Game::update_event(float dt)
{
  std::uniform_int_distribution<int> uniform_rnd_screen_width{ 0, SCREEN_WIDTH };
  std::uniform_int_distribution<int> uniform_rnd_screen_height{ 0, SCREEN_HEIGHT };
  Vector2d player_pos = player_.get_position();
  Vector2d rnd_pos = { dim_t(uniform_rnd_screen_width(gen)), dim_t(uniform_rnd_screen_height(gen)) };
  while ((rnd_pos - player_pos).get_magnitude() < enemy_spawn_min_distance_)
  {
    rnd_pos = { dim_t(uniform_rnd_screen_width(gen)), dim_t(uniform_rnd_screen_height(gen)) };
  }
  event_time_ += dt;
  enemy_spawn_cooldown_acc_ += dt;
  switch (event_)
  {
  case Event::RandomSpawnEnemiesTargetPlayer:
    if (enemy_spawn_cooldown_acc_ > enemy_random_spawn_cooldown_)
    {
      spawn_enemy(rnd_pos);
      enemy_spawn_cooldown_acc_ = 0;
    }
    if (event_time_ > enemy_random_spawn_event_duration_)
    {
      event_time_ = 0;
      event_ = Event::FastSpawnEnemiesTargetPlayer;
    }
    break;
  case Event::FastSpawnEnemiesTargetPlayer:
    if (enemy_spawn_cooldown_acc_ > enemy_fast_spawn_cooldown_
      && event_time_ < enemy_fast_spawn_duration_)
    {
      spawn_enemy(rnd_pos);
      enemy_spawn_cooldown_acc_ = 0;
    }
    if (event_time_ > enemy_fast_spawn_event_duration_)
    {
      event_time_ = 0;
      event_ = Event::BurstEnemiesTargetPlayer;
    }
    break;
  case Event::BurstEnemiesTargetPlayer:
    if (enemy_spawn_cooldown_acc_ > enemy_burst_cooldown_)
    {
      for (int i = 0; i < enemy_burst_size_; ++i)
      {
        player_.add_effect(enemy_burst_rate_ * i, [&, rnd_pos]() { spawn_enemy(rnd_pos); });
      }
      enemy_spawn_cooldown_acc_ = 0;
    }
    if (event_time_ > enemy_burst_event_duration_)
    {
      event_time_ = 0;
      event_ = Event::RandomSpawnEnemiesTargetPlayer;
    }
    break;
  }
}
