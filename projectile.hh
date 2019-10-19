#ifndef PROJECTILE_HH_
#define PROJECTILE_HH_
#include "box.hh"
#include "gameentity.hh"
#include "collision.hh"
#include "shader.hh"
#include <memory>

class Projectile : public GameEntity {
public:
    Projectile(Box entbox, Box worldbox, EntityTracker &entities,
                   int created_at, float accel, float hitpoints);

    virtual void update(int frame, int fps);
    virtual void draw(int frame);
    virtual void on_collision(int frame, Collision c);

private:
    static std::unique_ptr<Shader> shader_;

    float vert_speed_;
    float hitpoints_;
};

#endif
