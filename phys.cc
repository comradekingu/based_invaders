#include "phys.hh"

namespace phys {

const float pixels_per_meter = 1.f;

float seconds_to_frames(float s, float fps) {
    return s * fps;
}

float frames_to_seconds(int frames, float fps) {
    return (float)(frames) / fps;
}

float frame_period(float frequency, float fps) {
    return fps / frequency;
}

float pixels_per_frame(float meters_per_second, int fps) {
    float pixels_per_second = meters_per_second * pixels_per_meter;
    return pixels_per_second / fps;
}

float pixels_per_frame_sq(float mass, float force, int fps) {
    float meters_per_second_sq = force / mass;
    return pixels_per_frame(meters_per_second_sq, fps);
}

}
