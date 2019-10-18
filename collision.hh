#ifndef COLLISION_H_
#define COLLISION_H_

struct Collision {
    Collision()
    : collided(false), x(0), y(0) {}

    Collision(int x, int y)
    : collided(true), x(x), y(y) {}

    bool collided;
    int x;
    int y;

    operator bool() const {
        return collided;
    }
};

#endif
