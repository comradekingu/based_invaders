#ifndef SHADER_HH
#define SHADER_HH
#include <allegro5/allegro_opengl.h>

class Asset {};

class Shader : public Asset {
public:
    Shader(const char *vs, const char *fs);

    void use();
    static void reset();

private:
    ALLEGRO_SHADER *shader_;
};

#endif
