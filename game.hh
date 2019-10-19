#ifndef GAME_HH_
#define GAME_HH_
#include "entitytracker.hh"
#include "box.hh"
#include "config.hh"
#include <allegro5/allegro5.h>

class Game {
public:
    enum State {
        SETUP,
        GAME,
        OVER        
    };

    Game(Box worldbox, Config &config);

    void process_input(ALLEGRO_KEYBOARD_STATE *state);

    void draw(ALLEGRO_DISPLAY *display);
    void update();

    const EntityTracker &entities() const {
        return entities_;
    }

    int frame() const {
        return frame_;
    }

    State state() const {
        return state_;
    }

    int score() const {
        return score_;
    }

    void reset();

    bool shutdown() const {
        return shut_me_down_;
    }

private:
    void spawn_asteroids();
    void init_scene();
    void spawn_ship();
    void update_game_state();

    void on_keypress(int keycode);

    void change_game_state(enum State new_state);
    int frames_since_prev_state() const {
        return frame_ - state_changed_at_;
    }

    Box worldbox_;
    EntityTracker entities_;

    enum State state_;
    int state_changed_at_;
    int frame_;

    int score_;
    bool shut_me_down_;

    Config &config_;
};

#endif
