#ifndef SPRITE_HH_
#define SPRITE_HH_
#include "collision.hh"
#include "box.hh"
#include <allegro5/allegro5.h>

class Sprite {
public:
    explicit Sprite(Box b);
    ~Sprite();

    ALLEGRO_BITMAP *bitmap() {
        return bitmap_;
    }

    void draw();

    Collision collides(const Sprite &s) const;

    float &x() {
        return box_.x;
    }

    float &y() {
        return box_.y;
    }

    float w() const {
        return box_.w;
    }

    float h() const {
        return box_.h;
    }

    Box box() const {
        return box_;
    }

private:
    Box box_;
    ALLEGRO_BITMAP *bitmap_;
};

#endif
