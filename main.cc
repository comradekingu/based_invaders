#include <stdlib.h>
#include <assert.h>
#include <allegro5/allegro5.h>
#include "game.hh"
#include "overlay.hh"
#include "config.hh"

main() {
    Config config("config.json");

    al_init();
    al_install_keyboard();
    al_init_font_addon();

    al_set_new_display_flags(ALLEGRO_PROGRAMMABLE_PIPELINE | ALLEGRO_OPENGL);
    ALLEGRO_DISPLAY *display = al_create_display(config["window_width"],
                                               config["window_height"]);

    ALLEGRO_TIMER *timer = al_create_timer(1.f / config["fps"]);
    ALLEGRO_EVENT_QUEUE *queue = al_create_event_queue();
    al_register_event_source(queue, al_get_keyboard_event_source());
    al_register_event_source(queue, al_get_timer_event_source(timer));

    al_start_timer(timer);
    Game game(
        Box {
            0,
            0,
            (float)(config["window_width"]),
            (float)(config["window_height"])
        },
        config
    );
    Overlay overlay;

    bool done = false;
    bool need_redraw = true;
    bool background = false;
    ALLEGRO_KEYBOARD_STATE key_state;
    while (!done) {
        ALLEGRO_EVENT event;
        al_wait_for_event(queue, &event);
        switch (event.type) {
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
        game.process_input(&key_state);
        if (game.shutdown())
            done = true;

        if (need_redraw) {
            al_set_target_backbuffer(display);
            al_clear_to_color(al_map_rgb_f(0, 0, 0));

            game.draw(display);
            overlay.draw(display, game);

            al_set_target_backbuffer(display);
            al_flip_display();

            need_redraw = false;
        }
    }; 

    return 0;
}
