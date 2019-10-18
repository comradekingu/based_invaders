#include "game.hh"
#include "box.hh"
#include "ship.hh"
#include "asteroid.hh"
#include "background.hh"
#include "entitytracker.hh"
#include <assert.h>
#include <memory>
#include <cmath>

void Game::reset() {
    change_game_state(SETUP);
}

void Game::change_game_state(enum State new_state) {
    assert(state_ != new_state);

    switch (new_state) {
    case SETUP:
        entities_.clear_game_entities();
        score_ = 0;
        break;
    case GAME:
        spawn_ship();
        break;
    case OVER:
        break;
    default:
        break;
    }

    state_ = new_state;
    state_changed_at_ = frame_;
}

void Game::spawn_ship() {
    const float shipw = 100;
    const float shiph = 100;
    auto ship = std::make_unique<Ship>(
        Box{
            worldbox_.w / 2 - shipw / 2,
            worldbox_.h - shiph,
            shipw,
            shiph},
        worldbox_, entities_, 0, 1.2f
    );
    ship->add_collect_callback(
        [this](Entity &e) -> void {
            change_game_state(OVER);
        }
    );

    entities_.add(std::move(ship));
}

void Game::draw(ALLEGRO_DISPLAY *display) {
    entities_.draw(display, frame_);
}

void Game::update_game_state() {
    switch (state_) {
    case SETUP:
        if (frames_since_prev_state() > 30)
            change_game_state(GAME);
        break;
    case OVER:
    case GAME:
    default:
        break;
    }
}

void Game::update() {
    if (state_ == GAME) {
        spawn_asteroids();
        score_++;
    }

    entities_.update(frame_, fps_);
    update_game_state();
    
    frame_++;
}

float Game::difficulty() const {
    return fmin(score_ / 1000.f, 99.f) / 100.f;
}

void Game::spawn_asteroids() {
    const int initial_spawn_delay = 60;
    const int spawn_delay = initial_spawn_delay - (int)(difficulty()
                                       * .8f * initial_spawn_delay);
    if (frame_ % spawn_delay)
        return; 

    const float diameter = 100; 
    const int speed_variance = 50 + difficulty() * 80;
    const float speed = .4f + (rand() % speed_variance) / 10.f;
    const int rotation_variance = 10;
    const float rotation = (rand() % rotation_variance) / M_PI;

    auto asteroid = std::make_unique<Asteroid>(
        Box{
            (float)(rand() % ((int)(worldbox_.w) - (int)(diameter))),
            -diameter,
            diameter,
            diameter 
        }, worldbox_, entities_, frame_, rotation, speed
    );
    asteroid->add_collision_callback(
        [this](Entity &) -> void {
            score_ += 100;
        }
    );
    entities_.add(std::move(asteroid));
}

void Game::on_keypress(int keycode) {
    switch (keycode) {
    case ALLEGRO_KEY_Y:
        if (state_ == OVER)
            reset();
        break;
    case ALLEGRO_KEY_N:
        if (state_ == OVER)
            shut_me_down_ = true;
        break;
    case ALLEGRO_KEY_Q:
    case ALLEGRO_KEY_ESCAPE:
        shut_me_down_ = true;
        break;
    default:
        entities_.on_keypress(keycode);
        break;
    }
}

Game::Game(Box worldbox, int fps)
: worldbox_(worldbox), frame_(0), fps_(fps), shut_me_down_(false) {
    init_scene();
    reset();
}

void Game::init_scene() {
    entities_.add(
        std::make_unique<Background>(
            worldbox_, 0
        )
    );
}

void Game::process_input(ALLEGRO_KEYBOARD_STATE *state) {
    int interesting_keycodes[] = {
        ALLEGRO_KEY_RIGHT,
        ALLEGRO_KEY_LEFT,
        ALLEGRO_KEY_UP,
        ALLEGRO_KEY_Y,
        ALLEGRO_KEY_N,
        ALLEGRO_KEY_ESCAPE,
        ALLEGRO_KEY_Q
    };

    for (int kc : interesting_keycodes) {
        if (al_key_down(state, kc))
            on_keypress(kc);
    }
}
