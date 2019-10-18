#include "projectile.hh"
#include "gameentity.hh"
#include "entitytracker.hh"
#include "collision.hh"
#include "shader.hh"
#include "explosion.hh"
#include <stdlib.h>
#include <memory>
#include <allegro5/allegro5.h>
#include <allegro5/allegro_color.h>
#include <allegro5/allegro_primitives.h>

std::unique_ptr<Shader> Projectile::shader_;

Projectile::Projectile(Box entbox, Box worldbox, EntityTracker &entities,
                                             int created_at, float speed)
: GameEntity(entbox, worldbox, entities, 3, created_at), vert_speed_(speed),
                                                  health_(rand() % 16 + 1) {
    if (!shader_) {
        shader_ = std::make_unique<Shader>(
            "texture_vs.glsl",
            "projectile_fs.glsl"
        );
    }

    ALLEGRO_COLOR white = al_map_rgb_f(1, 1, 1);
    al_set_target_bitmap(sprite_.bitmap());
    al_clear_to_color(al_map_rgba_f(0, 0, 0, 0));
    al_draw_filled_rectangle(0, 0, entbox.w, entbox.h, white);
}

void Projectile::update(int frame, int) {
    sprite_.y() -= vert_speed_;

    if (!sprite_.box().collides(worldbox_)) {
        collect();
        return;
    }
}

void Projectile::draw(int frame) {
    shader_->use();
    al_set_shader_float("frame", (float)(frame));
    al_set_shader_float("created_at", (float)(created_at_));
    sprite_.draw();
    Shader::reset();
}

void Projectile::on_collision(int frame, Collision c) {
    entities_.add(
        std::make_unique<Explosion>(
            frame,
            200 + rand() % 100,
            c.x,
            c.y,
            200.f
        )
    );

    if (health_-- <= 0)
        collect();
}
