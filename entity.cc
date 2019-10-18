#include "entity.hh"

void Entity::collect() {
    for (auto &cb : on_collect_cbs_)
        cb(*this);

    collect_ = true;
}
