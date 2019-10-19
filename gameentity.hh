#ifndef GAMEENTITY_HH_
#define GAMEENTITY_HH_
#include "entity.hh"
#include "box.hh"
#include "collision.hh"
#include "sprite.hh"

class EntityTracker;
class GameEntity : public Entity {
public:
    Collision collides(const GameEntity &e) {
        if (collision_group_ == e.collision_group_)
            return Collision(); // nothing happened

        return sprite_.collides(e.sprite_);
    }

    void add_collision_callback(Callback cb) {
        on_collision_cbs_.push_back(cb);
    }

    virtual void on_keypress(int keycode) {};
    virtual void on_collision(int frame, Collision c);
    virtual ~GameEntity() {}

protected:
    GameEntity(Box entbox, Box worldbox, EntityTracker &entities,
                             int collision_group, int created_at)
    : Entity(created_at), sprite_(entbox), worldbox_(worldbox),
      entities_(entities), collision_group_(collision_group) {}

    Sprite sprite_;
    Box worldbox_;
    EntityTracker &entities_;
    int collision_group_;

    std::vector<Callback> on_collision_cbs_;
};

#endif
