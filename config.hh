#ifndef CONFIG_HH_
#define CONFIG_HH_
#include <json-c/json.h>
#include <string>
#include <unordered_map>
#include <set>

class Config {
public:
    Config(const char *filename);

    double operator[](const std::string &key) const;

    /*
     *  config items
     */

    /*
    // pixels
    int window_width;

    // pixels 
    int window_height;

    int fps;

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
    */

private:
    void fill_items(std::string config);
    void fill_defaults();
    void print_config();

    std::set<std::string> keys() const;

    std::unordered_map<std::string, double> config_;
    std::unordered_map<std::string, double> defaults_;
};

#endif
