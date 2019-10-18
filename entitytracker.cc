#include "entitytracker.hh"
#include <allegro5/allegro5.h>

void EntityTracker::clear_game_entities() {
    game_entities_.clear();
}

void EntityTracker::clear_vfx_entities() {
    vfx_entities_.clear();
}

void EntityTracker::clear() {
    clear_game_entities();
    clear_vfx_entities();
}

int EntityTracker::count() const {
    return game_entities_.size() + vfx_entities_.size();
}

void EntityTracker::draw(ALLEGRO_DISPLAY *display, int frame) {
    for (auto &e : game_entities_) {
        al_set_target_backbuffer(display);
        e->draw(frame);
    }
    for (auto &e : vfx_entities_) {
        al_set_target_backbuffer(display);
        e->draw(frame);
    }
}

void EntityTracker::update(int frame, int fps) {
    for (auto &e : game_entities_)
        e->update(frame, fps);
    for (auto &e : vfx_entities_)
        e->update(frame, fps);

    check_collisions(frame);
    collect_entities(game_entities_);
    collect_entities(vfx_entities_);
}

void EntityTracker::on_keypress(int keycode) {
    for (auto &e : game_entities_)
        e->on_keypress(keycode);
}

void EntityTracker::check_collisions(int frame) {
    for (auto ea = game_entities_.begin(); ea != game_entities_.end();
                                                               ea++) {
        for (auto eb = game_entities_.begin(); eb != game_entities_.end();
                                                                   eb++) {
            if (ea == eb)
                continue;

            Collision c = (*ea)->collides(**eb);
            if (c) {
                (*ea)->on_collision(frame, c);
                (*eb)->on_collision(frame, c);
            }
        }
    }
}
