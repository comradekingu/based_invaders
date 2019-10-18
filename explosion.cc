#include "explosion.hh"
#include "vfxentity.hh"
#include "shader.hh"
#include <memory>
#include <allegro5/allegro5.h>
#include <allegro5/allegro_color.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_opengl.h>

std::unique_ptr<Shader> Explosion::shader_;

Explosion::Explosion(int created_at, int lifetime, float cx, float cy,
                                                      float maxradius)
: VFXEntity(created_at), cx_(cx), cy_(cy), maxradius_(maxradius),
                                            lifetime_(lifetime) {
    if (!shader_) {
        shader_ = std::make_unique<Shader>(
            "generic_vs.glsl",
            "explosion_fs.glsl"
        );
    }
}

void Explosion::draw(int frame) {
    shader_->use();

    float explosion_center[2] = { cx_, cy_ };
    al_set_shader_float_vector("explosion_center", 2, &explosion_center[0], 1);
    al_set_shader_float("progress", (float)(frame - created_at_) / lifetime_);
    al_set_shader_float("maxradius", maxradius_);

    ALLEGRO_COLOR color;
    al_draw_filled_rectangle(cx_ - maxradius_, cy_ - maxradius_,
                     cx_ + maxradius_, cy_ + maxradius_, color);

    Shader::reset();
}

void Explosion::update(int frame, int) {
    if (frame > created_at_ + lifetime_)
        collect();
}
