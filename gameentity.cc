#include "gameentity.hh"
#include "entity.hh"

void GameEntity::on_collision(int frame, Collision c) {
    for (auto &cb : on_collision_cbs_)
        cb(*this);
}
