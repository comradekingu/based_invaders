#include "game.hh"
#include "box.hh"
#include "ship.hh"
#include "asteroid.hh"
#include "background.hh"
#include "entitytracker.hh"
#include <memory>
#include <cmath>

void Game::change_game_state(enum State new_state) {
    switch (new_state) {
    case SETUP:
        entities_.clear_game_entities();
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
        worldbox_, entities_, 0, 2.4f
    );
    ship->add_collision_callback(
        [this](Entity &) -> void {
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
        if (frames_since_prev_state() > 300)
            change_game_state(SETUP);
        break;
    case GAME:
    default:
        break;
    }
}

void Game::update() {
    if (state_ == GAME)
        spawn_asteroids();

    entities_.update(frame_, fps_);
    update_game_state();

    frame_++;
}

void Game::spawn_asteroids() {
    if (!(frame_ % 50)) {
        const float rotation = M_PI / (rand() % 30 + 1);
        const float diameter = 100;
        const float speed = .1f + (rand()%40) / 10.f;
        entities_.add(
            std::make_unique<Asteroid>(
                Box{
                    rand() % (int)(worldbox_.w) - diameter,
                    -diameter,
                    diameter,
                    diameter 
                }, worldbox_, entities_, frame_, rotation, speed
            )
        );
    }
}

void Game::on_keypress(int keycode) {
    entities_.on_keypress(keycode);
}

Game::Game(Box worldbox, int fps)
: worldbox_(worldbox), frame_(0), fps_(fps) {
    init_scene();
    change_game_state(SETUP);
}

void Game::init_scene() {
    entities_.add(
        std::make_unique<Background>(
            worldbox_, 0
        )
    );
}
