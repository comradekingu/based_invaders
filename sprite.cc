#include "sprite.hh"
#include "box.hh"
#include "collision.hh"
#include <allegro5/allegro5.h>

void Sprite::draw() {
    al_draw_bitmap(bitmap_, (int)(x()), (int)(y()), 0);
}

Collision Sprite::collides(const Sprite &s) const {
    Collision retv;

    if (!box_.collides(s.box_))
        return retv;

    // pixel sweep optimization
    if (s.box_.area() < box_.area())
        return s.collides(*this);

    ALLEGRO_LOCKED_REGION *sa_lk = al_lock_bitmap(bitmap_,
                           ALLEGRO_PIXEL_FORMAT_ABGR_8888,
                                   ALLEGRO_LOCK_READWRITE);
    ALLEGRO_LOCKED_REGION *sb_lk = al_lock_bitmap(s.bitmap_,
                             ALLEGRO_PIXEL_FORMAT_ABGR_8888,
                                     ALLEGRO_LOCK_READWRITE);
    unsigned char *sa_data = (unsigned char *)(sa_lk->data);
    unsigned char *sb_data = (unsigned char *)(sb_lk->data);
    int sap = sa_lk->pitch;
    int sbp = sb_lk->pitch;

    // for every point in the sprite
    for (int px = box_.x; px < box_.x + box_.w; px++) {
        for (int py = box_.y; py < box_.y + box_.h; py++) {
            if (s.box_.collides(px, py)) {
                auto la = box_.local_coords(px, py);
                int lax = la.first;
                int lay = la.second;

                auto lb = s.box_.local_coords(px, py);
                int lbx = lb.first;
                int lby = lb.second; 

                int alpha_a = sa_data[lay * sap + lax * 4];
                int alpha_b = sb_data[lby * sbp + lbx * 4];

                if (alpha_a && alpha_b) {
                    retv.collided = true;
                    retv.x = px;
                    retv.y = py; 
                    goto cleanup;
                }
            }
        }
    }

    // C-style allegro interface
cleanup:
    al_unlock_bitmap(bitmap_);
    al_unlock_bitmap(s.bitmap_);
    return retv;
}

Sprite::Sprite(Box b)
: box_(b) {
    bitmap_ = al_create_bitmap(b.w, b.h);
    assert(bitmap_ != NULL);
}

Sprite::~Sprite() {
    if (bitmap_ != NULL)
        al_destroy_bitmap(bitmap_);
}
