#ifndef ASTEROID_HH_
#define ASTEROID_HH_
#include "gameentity.hh"
#include "shader.hh"
#include <memory>

class EntityTracker;
class Collision;
class Sprite;
class Asteroid : public GameEntity {
public:
    Asteroid(Box entbox, Box worldbox, EntityTracker &entities,
         int created_at, float rotation, float downward_speed);
    virtual ~Asteroid() {};

    virtual void update(int frame, int fps);
    virtual void draw(int frame);
    virtual void on_collision(int frame, Collision c);

private:
    static std::unique_ptr<Shader> shader_;

    void gen_sprite();

    Sprite pre_rotation_;
    float rotation_; // rad/s
    float downward_speed_;
    float health_;
};

#endif
