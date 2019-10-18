#include <assert.h>
#include <stdlib.h>
#include <deque>
#include <vector>
#include <memory>
#include <atomic>
#include <utility>
#include <cmath>
#include <iostream>
#include <functional>
#include <allegro5/allegro5.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_color.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_opengl.h>

#include "game.hh"

#define FPS 60

ALLEGRO_DISPLAY *display = NULL;

main() {
    const int dw = 800;
    const int dh = 600;

    al_init();
    al_set_config_value(al_get_system_config(), "trace", "level", "debug");
    al_install_keyboard();
    al_init_font_addon();

    al_set_new_display_flags(ALLEGRO_PROGRAMMABLE_PIPELINE | ALLEGRO_OPENGL);
    display = al_create_display(dw, dh);

    ALLEGRO_KEYBOARD_STATE key_state;
    ALLEGRO_TIMER *timer = al_create_timer(1.0 / FPS);
    ALLEGRO_EVENT_QUEUE *queue = al_create_event_queue();
    al_register_event_source(queue, al_get_keyboard_event_source());
    al_register_event_source(queue, al_get_timer_event_source(timer));

    ALLEGRO_FONT *font = al_create_builtin_font();
    ALLEGRO_COLOR white = al_map_rgb_f(1, 1, 1);

    al_start_timer(timer);
    Game game(Box{0, 0, dw, dh}, FPS);

    bool done = false;
    bool need_redraw = true;
    bool background = false;
    while (!done) {
        const int w = al_get_display_width(display);
        const int h = al_get_display_height(display);

        ALLEGRO_EVENT event;
        al_wait_for_event(queue, &event);
        switch (event.type) {
        case ALLEGRO_EVENT_KEY_DOWN:
            break;

        case ALLEGRO_EVENT_DISPLAY_CLOSE:
           done = true;
           break;

        case ALLEGRO_EVENT_DISPLAY_HALT_DRAWING:
           background = true;
           al_acknowledge_drawing_halt(event.display.source);
           break;
        case ALLEGRO_EVENT_DISPLAY_RESUME_DRAWING:
           background = false;
           al_acknowledge_drawing_resume(event.display.source);
           break;
        case ALLEGRO_EVENT_DISPLAY_RESIZE:
           al_acknowledge_resize(event.display.source);
           break;
        case ALLEGRO_EVENT_TIMER:
           game.update();
           need_redraw = true;
           break;
        }

        al_get_keyboard_state(&key_state);
        if (al_key_down(&key_state, ALLEGRO_KEY_RIGHT))
            game.on_keypress(ALLEGRO_KEY_RIGHT);
        if (al_key_down(&key_state, ALLEGRO_KEY_LEFT))
            game.on_keypress(ALLEGRO_KEY_LEFT);
        if (al_key_down(&key_state, ALLEGRO_KEY_UP))
            game.on_keypress(ALLEGRO_KEY_UP);

        if (need_redraw) {
            al_set_target_backbuffer(display);
            al_clear_to_color(al_map_rgb_f(0, 0, 0));

            game.draw(display);

            al_set_target_backbuffer(display);
            int entities = game.entities().count();
            al_draw_textf(font, white, 0, 0, 0, "frame: %d, entities: %d", game.frame(), entities);
            al_flip_display();
            need_redraw = false;
        }
    }; 

    return 0;
}
