#ifndef SHIP_HH_
#define SHIP_HH_
#include "gameentity.hh"
#include "shader.hh"
#include <memory>

class EntityTracker;
class Collision;
class Sprite;
class Ship : public GameEntity {
public:
    Ship(Box shipbox, Box worldbox, EntityTracker &entities, int created_at,
                                                               float accel);

    virtual void update(int frame, int fps);
    virtual void draw(int frame);
    virtual void on_keypress(int keycode);
    virtual void on_collision(int frame, Collision c);

private:
    static std::unique_ptr<Shader> shader_;

    float accel_;
    int fire_cooldown_ = 10;

    float horz_speed_ = 0;
    float next_frame_dv_ = 0;
    bool fire_projectile_ = 0;
    int last_projectile_frame_ = 0;
};

#endif
