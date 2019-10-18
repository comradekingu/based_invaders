#include "background.hh"
#include <memory>
#include <allegro5/allegro5.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_color.h>
#include <allegro5/allegro_opengl.h>

std::unique_ptr<Shader> Background::shader_;

Background::Background(Box worldbox, int created_at)
: VFXEntity(created_at), worldbox_(worldbox) {
    if (!shader_) {
        shader_ = std::make_unique<Shader>(
            "generic_vs.glsl",
            "background_fs.glsl"
        );
    }
}

void Background::draw(int frame) {
    shader_->use();

    float dimensions[2] = { worldbox_.w, worldbox_.h };
    al_set_shader_float_vector("dimensions", 2, &dimensions[0], 1);

    ALLEGRO_COLOR color;
    al_draw_filled_rectangle(0, 0, worldbox_.w, worldbox_.h, color);

    Shader::reset();
}

