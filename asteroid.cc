#include "asteroid.hh"
#include "gameentity.hh"
#include "collision.hh"
#include "shader.hh"
#include "entitytracker.hh"
#include "explosion.hh"
#include <memory>
#include <cmath>
#include <allegro5/allegro5.h>
#include <allegro5/allegro_color.h>
#include <allegro5/allegro_opengl.h>
#include <allegro5/allegro_primitives.h>

std::unique_ptr<Shader> Asteroid::shader_;

Asteroid::Asteroid(Box entbox, Box worldbox, EntityTracker &entities,
                int created_at, float rotation, float downward_speed,
                                                     float hitpoints)
: GameEntity(entbox, worldbox, entities, 1, created_at), pre_rotation_(entbox),
                          rotation_(rotation), downward_speed_(downward_speed),
                                                        hitpoints_(hitpoints) {
    if (!shader_) {
        shader_ = std::make_unique<Shader>(
            "texture_vs.glsl",
            "asteroid_fs.glsl"
        );
    }

    gen_sprite();
}

void Asteroid::gen_sprite() {
    std::vector<float> vertices;
    int vertex_count_ = rand() % 6 + 5; 
    const float step = 2 * M_PI / (float)(vertex_count_);
    for (int i = 0; i < vertex_count_; i++) {
        float len = .2f + (rand() % 3) / 10.f;
        float angle = i * step;
        float x = (.5f + sin(angle) * len) * sprite_.w();
        float y = (.5f + cos(angle) * len) * sprite_.h();

        vertices.push_back(x);
        vertices.push_back(y);
    }

    int g = 25 + rand() % 125;
    ALLEGRO_COLOR color = al_map_rgb(g, g, g);

    al_set_target_bitmap(pre_rotation_.bitmap());
    al_clear_to_color(al_map_rgba_f(0, 0, 0, 0));
    al_draw_filled_polygon(vertices.data(), vertex_count_, color);
}

void Asteroid::draw(int frame) {
    shader_->use();

    al_set_shader_float("frame", (float)(frame));
    al_set_shader_float("created_at", (float)(created_at_));
    al_set_shader_float("speed", (float)(downward_speed_));
    sprite_.draw();

    Shader::reset();
}

void Asteroid::update(int frame, int fps) {
    sprite_.y() += downward_speed_;

    if (sprite_.y() > 0 && !sprite_.box().collides(worldbox_)) {
        entities_.add(
            std::make_unique<Explosion>(
                frame,
                800,
                sprite_.x(),
                sprite_.y() + 100.f,
                1200.f
            )
        );

        collect();
        return;
    }

    float angle = rotation_ * frame / (float)(fps);
    int cx = sprite_.w() / 2;
    int cy = sprite_.h() / 2;

    al_set_target_bitmap(sprite_.bitmap());
    al_clear_to_color(al_map_rgba_f(0, 0, 0, 0));
    al_draw_rotated_bitmap(pre_rotation_.bitmap(), cx, cy, cx, cy, angle, 0);
}

void Asteroid::on_collision(int frame, Collision c) {
    if (--hitpoints_ <= 0) {
        entities_.add(
            std::make_unique<Explosion>(
                frame,
                400,
                sprite_.x() + sprite_.w() / 2,
                sprite_.y() + sprite_.h() / 2,
                400.f
            )
        );
        entities_.add(
            std::make_unique<Explosion>(
                frame,
                50,
                sprite_.x() + sprite_.w() / 2,
                sprite_.y() + sprite_.h() / 2,
                400.f + (float)(rand() % 200)
            )
        );

        collect();
    }

    GameEntity::on_collision(frame, c);
}
