#ifndef CONFIG_HH_
#define CONFIG_HH_
#include <json/json.h>
#include <fstream>

class Config {
public:
    Config(const char *filename);

    /*
     *  config items
     */

    // shots/second
    float ship_shooting_freq;

    // meters/second
    float ship_bullet_velocity;

    // meters/second
    float ship_forward_velocity;

    // radians/second
    float asteroid_angular_velocity_range;

    // newtons
    float ship_thrust;

    // kilograms
    float ship_mass;

    // number/second
    float asteroid_appearance_frequency;

    // number/second
    float asteroid_appearance_frequency_increase;

    int asteroid_hitpoint_range;
    int projectile_hitpoint_range;

private:
    void fill_items();
    void print_config();

    Json::Value root_;
};

#endif
