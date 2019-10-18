#ifndef ENTITYTRACKER_HH_
#define ENTITYTRACKER_HH_
#include "gameentity.hh"
#include "vfxentity.hh"
#include "box.hh"
#include <memory>
#include <deque>
#include <allegro5/allegro5.h>

class EntityTracker {
public:
    typedef std::unique_ptr<GameEntity> GameEntityPtr;
    typedef std::unique_ptr<VFXEntity> VFXEntityPtr;
    typedef std::unique_ptr<Entity> EntityPtr;

    GameEntity &add(GameEntityPtr p) {
        game_entities_.push_back(std::move(p));
        return *game_entities_.back();
    }

    VFXEntity &add(VFXEntityPtr p) {
        vfx_entities_.push_back(std::move(p));
        return *vfx_entities_.back();
    }

    void draw(ALLEGRO_DISPLAY *display, int frame);
    void update(int frame, int fps);
    void clear_game_entities();

    void on_keypress(int keycode);

    int count() const;

private:
    template <typename Container>
    static void collect_entities(Container &entities);

    void check_collisions(int frame);

    std::deque<GameEntityPtr> game_entities_;
    std::deque<VFXEntityPtr> vfx_entities_;
};

template <typename Container>
void EntityTracker::collect_entities(Container &entities) {
    bool retry;
    do {
        retry = false;
        for (auto it = entities.begin(); it != entities.end(); it++) {
            if ((*it)->collectable()) {
                entities.erase(it);
                retry = true;
                break;
            }
        }
    } while(retry);
}

#endif
