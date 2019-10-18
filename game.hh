#ifndef GAME_HH_
#define GAME_HH_
#include "entitytracker.hh"
#include "box.hh"
#include <allegro5/allegro5.h>

class Game {
public:
    enum State {
        SETUP,
        GAME,
        OVER        
    };

    Game(Box worldbox, int fps);

    void draw(ALLEGRO_DISPLAY *display);
    void update();
    void on_keypress(int keycode);

    const EntityTracker &entities() const {
        return entities_;
    }

    int frame() const {
        return frame_;
    }

    State state() const {
        return state_;
    }

private:
    void spawn_asteroids();
    void init_scene();
    void spawn_ship();
    void update_game_state();

    void change_game_state(enum State new_state);
    int frames_since_prev_state() const {
        return frame_ - state_changed_at_;
    }

    Box worldbox_;
    EntityTracker entities_;

    enum State state_;
    int state_changed_at_;
    int frame_;
    const int fps_;
};

#endif
