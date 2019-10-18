#include "shader.hh"
#include <iostream>
#include <allegro5/allegro5.h>
#include <allegro5/allegro_opengl.h>

Shader::Shader(const char *vs, const char *fs) {
    shader_ = al_create_shader(ALLEGRO_SHADER_AUTO);
    if (!al_attach_shader_source_file(shader_, ALLEGRO_VERTEX_SHADER, vs)) {
        std::cerr << al_get_shader_log(shader_) << std::endl;
        exit(1);
    }

    if (!al_attach_shader_source_file(shader_, ALLEGRO_PIXEL_SHADER, fs)) {
        std::cerr << al_get_shader_log(shader_) << std::endl;
        exit(1);
    }

    if (!al_build_shader(shader_)) {
        std::cerr << al_get_shader_log(shader_) << std::endl;
        exit(1);
    }
}

void Shader::use() {
    assert(shader_ != NULL);
    al_use_shader(shader_);
}

void Shader::reset() {
    al_use_shader(NULL);
}

