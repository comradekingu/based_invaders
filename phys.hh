#ifndef PHYS_HH_
#define PHYS_HH_

namespace phys {

extern const float pixels_per_meter;

float seconds_to_frames(float s, float fps);
float frames_to_seconds(int frames, float fps);
float frame_period(float frequency, float fps);
float pixels_per_frame(float meters_per_second, int fps);
float pixels_per_frame_sq(float mass, float force, int fps);

}

#endif
