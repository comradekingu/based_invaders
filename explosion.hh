#ifndef EXPLOSION_HH_
#define EXPLOSION_HH_
#include "vfxentity.hh"
#include "shader.hh"
#include <memory>

class Explosion : public VFXEntity {
public:
    Explosion(int created_at, int lifetime, float cx, float cy,
                                              float maxradius);

    virtual void draw(int frame);
    virtual void update(int frame, int fps);

private:
    static std::unique_ptr<Shader> shader_;

    float cx_;
    float cy_;
    float maxradius_;
    int lifetime_;
};

#endif
