#ifndef OVERLAY_HH_
#define OVERLAY_HH_
#include "game.hh"
#include <allegro5/allegro5.h>
#include <allegro5/allegro_font.h>

class Overlay {
public:
    Overlay();
    ~Overlay();

    void draw(ALLEGRO_DISPLAY *display, const Game &game);

private:
    void draw_score(const Game &game);
    void draw_prompt(const Game &game, const char *text, ALLEGRO_COLOR col);

    ALLEGRO_FONT *font_;
};

#endif
