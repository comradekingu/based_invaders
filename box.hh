#ifndef BOX_H_
#define BOX_H_
#include <utility>

struct Box {
    float x;
    float y;
    float w;
    float h;

    bool collides(const Box &b) const;
    bool collides(int x, int y) const;
    std::pair<int, int> local_coords(int x, int y) const;
    int area() const;
    Box scaled(float coeff) const;
};

#endif
