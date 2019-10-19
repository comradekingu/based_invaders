#include "config.hh"
#include <json/json.h>
#include <stdlib.h>
#include <fstream>
#include <iostream>

Config::Config(const char *filename) {
    std::ifstream ifs;
    ifs.open(filename);

    Json::CharReaderBuilder builder;
    JSONCPP_STRING errs;
    if (!parseFromStream(builder, ifs, &root_, &errs)) {
        std::cerr << "Couldn't read from config file." << std::endl;
        exit(1);
    }

    fill_items();
    print_config();
}

void Config::fill_items() {
    ship_shooting_freq = root_.get("ship_shooting_freq", 5.f).asFloat();
    ship_bullet_velocity = root_.get("ship_bullet_velocity", 800.f).asFloat();
    ship_forward_velocity = root_.get("ship_forward_velocity", 80.f).asFloat();
    asteroid_angular_velocity_range = root_.get(
        "asteroid_angular_velocity_range", 6.28f).asFloat();
    ship_thrust = root_.get("ship_thrust", 240000.f).asFloat();
    ship_mass = root_.get("ship_mass", 10000.f).asFloat();
    asteroid_appearance_frequency = root_.get(
        "asteroid_appearance_frequency", .03f).asFloat();
    asteroid_appearance_frequency_increase = root_.get(
        "asteroid_appearance_frequency_increase", .01f).asFloat();
    asteroid_hitpoint_range = root_.get("asteroid_hitpoint_range", 8).asInt();
    projectile_hitpoint_range = root_.get("projectile_hitpoint_range",
                                                          16).asInt();
}

void Config::print_config() {
    std::cout << "ship_shooting_freq = " << ship_shooting_freq << std::endl;

    std::cout << "ship_bullet_velocity = " << ship_bullet_velocity << std::endl;

    std::cout << "ship_forward_velocity = " << ship_forward_velocity << std::endl;

    std::cout << "asteroid_angular_velocity_range = "
        << asteroid_angular_velocity_range << std::endl;

    std::cout << "ship_thrust = " << ship_thrust << std::endl;

    std::cout << "ship_mass = " << ship_mass << std::endl;

    std::cout << "asteroid_appearance_frequency = "
        << asteroid_appearance_frequency << std::endl;

    std::cout << "asteroid_appearance_frequency_increase = "
        << asteroid_appearance_frequency_increase << std::endl;

    std::cout << "asteroid_hitpoint_range = " << asteroid_hitpoint_range
                                                           << std::endl;

    std::cout << "projectile_hitpoint_range = " << projectile_hitpoint_range
                                                               << std::endl;
}
