#include "gameentity.hh"
#include "entity.hh"

void GameEntity::collect() {
    for (auto &cb : on_collision_cbs_)
        cb(*this);

    Entity::collect();
}

