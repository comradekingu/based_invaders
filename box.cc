#include "box.hh"

bool Box::collides(const Box &b) const {
    return x < b.x + b.w &&
           x + w > b.x &&
           y < b.y + b.h &&
           y + h > b.y;
}

bool Box::collides(int px, int py) const {
    return px > x && px < x + w &&
           py > y && py < y + h;
}

std::pair<int, int> Box::local_coords(int px, int py) const {
    return std::make_pair(
        px - x,
        py - y
    );
}

int Box::area() const {
    return w * h;
}

Box Box::scaled(float c) const {
    return Box{x * c, y * c, w * c, h * c};
}
