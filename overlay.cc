#include "overlay.hh"
#include "game.hh"
#include <cmath>
#include <allegro5/allegro5.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_color.h>
#include <allegro5/allegro_font.h>

Overlay::Overlay() {
    font_ = al_create_builtin_font();
}

Overlay::~Overlay() {
}

static float pulse(int frame) {
    return .3f * sin(M_PI * (frame % 30) / 30.f) + .7f;
}

static ALLEGRO_COLOR redpulse(int frame) {
    return al_map_rgb_f(
        pulse(frame),
        .0f,
        .0f
    );
}

static ALLEGRO_COLOR graypulse(int frame) {
    return al_map_rgb_f(
        pulse(frame),
        pulse(frame),
        pulse(frame)
    );
}

static ALLEGRO_COLOR bluepulse(int frame) {
    return al_map_rgb_f(
        .0f,
        .0f,
        pulse((int)(frame / 3.0f))
    );
}

void Overlay::draw_score(const Game &game) {
    ALLEGRO_COLOR white = al_map_rgb_f(1, 1, 1);
    al_draw_textf(font_, white, 0, 10, 0, "score: %d", game.score());
}

void Overlay::draw_prompt(const Game &game, const char *text,
                                         ALLEGRO_COLOR col) {
    al_draw_text(font_, col, 0, 0, 0, text);
}

void Overlay::draw(ALLEGRO_DISPLAY *display, const Game &game) {
    al_set_target_backbuffer(display);
    draw_score(game);

    switch (game.state()) {
    case Game::SETUP:
        draw_prompt(game, "Prepare...", bluepulse(game.frame()));
        break;
    case Game::OVER:
        draw_prompt(game, "Again? (y/n)", redpulse(game.frame()));
        break;
    case Game::GAME: {
            ALLEGRO_COLOR white = al_map_rgb_f(1, 1, 1);
            draw_prompt(game, "KEYBOARD: LEFT, RIGHT - MOVE AROUND, UP - FIRE",
                                                                        white);
        }
        break;
    default:
        break;
    }
}
