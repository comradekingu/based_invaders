#include "ship.hh"
#include "gameentity.hh"
#include "collision.hh"
#include "shader.hh"
#include "entitytracker.hh"
#include "projectile.hh"
#include "explosion.hh"
#include <memory>
#include <cmath>
#include <allegro5/allegro5.h>
#include <allegro5/allegro_color.h>
#include <allegro5/allegro_primitives.h>

std::unique_ptr<Shader> Ship::shader_;

Ship::Ship(Box entbox, Box worldbox, EntityTracker &entities, int created_at,
                                                                 float accel)
: GameEntity(entbox, worldbox, entities, 2, created_at), accel_(accel) {
    if (!shader_) {
        shader_ = std::make_unique<Shader>(
            "texture_vs.glsl",
            "ship_fs.glsl"
        );
    }

    ALLEGRO_COLOR white = al_map_rgb_f(1, 1, 1);
    al_set_target_bitmap(sprite_.bitmap());
    al_clear_to_color(al_map_rgba_f(0, 0, 0, 0));

    Box ship = entbox.scaled(.5f);
    float x1 = 0 + .25f * entbox.w;
    float y1 = ship.h + .25f * entbox.h;
    float x2 = ship.w / 2 + .25f * entbox.w;
    float y2 = 0 + .25f * entbox.h;
    float x3 = ship.w + .25f * entbox.w;
    float y3 = ship.h + .25f * entbox.h;
    al_draw_filled_triangle(x1, y1, x2, y2, x3, y3, white);
}

void Ship::draw(int frame) {
    shader_->use();
    al_set_shader_float("frame", (float)(frame));
    al_set_shader_float("created_at", (float)(created_at_));
    al_set_shader_float("projectile_cooldown", (float)(frame
                - last_projectile_frame_) / fire_cooldown_);
    sprite_.draw();
    Shader::reset();
}

void Ship::on_keypress(int keycode) {
    switch (keycode) {
    case ALLEGRO_KEY_LEFT:
        next_frame_dv_ = -accel_;
        break;
    case ALLEGRO_KEY_RIGHT:
        next_frame_dv_ = accel_;
        break;
    case ALLEGRO_KEY_UP:
        fire_projectile_ = true;
        break;
    }
}

void Ship::update(int frame, int) {
    if (sprite_.x() < 0 || sprite_.x() > worldbox_.w - sprite_.w()) {
        horz_speed_ = -horz_speed_ / 2.f;
        next_frame_dv_ = .0f;

        if (sprite_.x() < 0)
            sprite_.x() = 0;
        else if (sprite_.x() > worldbox_.w - sprite_.w())
            sprite_.x() = worldbox_.w - sprite_.w();
    }

    if (next_frame_dv_ != .0f) {
        horz_speed_ += next_frame_dv_;
        next_frame_dv_ = .0f;
    } else {
        if (horz_speed_ >= accel_)
            horz_speed_ -= accel_;
        else if (horz_speed_ <= accel_)
            horz_speed_ += accel_;

        if (fabs(horz_speed_) <= accel_)
            horz_speed_ = .0f;
    }

    if (fire_projectile_) {
        fire_projectile_ = false;
        if (fabs(last_projectile_frame_ - frame) > fire_cooldown_) {
            last_projectile_frame_ = frame;

            Box prbox{
                sprite_.x() + sprite_.w() / 2,
                sprite_.y() - 7,
                3,
                5
            };

            entities_.add(
                std::make_unique<Projectile>(
                    prbox, worldbox_, entities_, frame, 12.f
                )
            );
        }
    }

    sprite_.x() += horz_speed_;
}

void Ship::on_collision(int frame, Collision c) {
        entities_.add(
            std::make_unique<Explosion>(
                frame,
                50,
                sprite_.x() + sprite_.w() / 2,
                sprite_.y() + sprite_.h() / 2,
                4800.f
            )
        );
        entities_.add(
            std::make_unique<Explosion>(
                frame,
                1000,
                sprite_.x() + sprite_.w() / 2,
                sprite_.y() + sprite_.h() / 2,
                3600.f
            )
        );

        collect();
}
