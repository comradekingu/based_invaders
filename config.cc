#include "config.hh"
#include <assert.h>
#include <json-c/json.h>
#include <stdlib.h>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <unordered_map>
#include <set>

Config::Config(const char *filename) {
    std::ifstream ifs(filename);
    std::stringstream contents;
    contents << ifs.rdbuf();

    fill_items(contents.str()); 
    fill_defaults();
    print_config();
}

void Config::fill_defaults() {
    defaults_["window_width"] = 800;
    defaults_["window_height"] = 600;
    defaults_["fps"] = 60;
    defaults_["ship_shooting_freq"] = 5;
    defaults_["ship_bullet_velocity"] = 800;
    defaults_["ship_forward_velocity"] = 80;
    defaults_["asteroid_angular_velocity_range"] = 6.28;
    defaults_["ship_thrust"] = 240000;
    defaults_["ship_mass"] = 10000;
    defaults_["asteroid_appearance_frequency"] = .03;
    defaults_["asteroid_hitpoint_range"] = 8;
    defaults_["projectile_hitpoint_range"] = 16;
}

void Config::fill_items(std::string config) {
    json_object *dict = json_tokener_parse(config.c_str());
    assert(json_object_get_type(dict) == json_type_object);

    json_object_object_foreach(dict, key, val) {
        if (json_object_get_type(val) == json_type_int) {
            config_[key] = json_object_get_int(val);
        } else if (json_object_get_type(val) == json_type_double) {
            config_[key] = json_object_get_double(val);
        } else {
            std::cerr << "Config: wrong value type for: " << key << std::endl;
            exit(1);
        }
    }
}

std::set<std::string> Config::keys() const {
    std::set<std::string> rkeys;
    for (const auto &kv : config_)
        rkeys.insert(kv.first);
    for (const auto &kv : defaults_)
        rkeys.insert(kv.first);
    return rkeys;
}

void Config::print_config() {
    auto kset = keys();
    for (const auto &key : kset)
        std::cout << key << ": " << (*this)[key] << std::endl;
}

double Config::operator[](const std::string &key) const {
    auto it_config = config_.find(key);
    if (it_config != config_.end())
        return it_config->second;

    auto it_default = defaults_.find(key);
    if (it_default != config_.end())
        return it_default->second;

    std::cerr << "No default value for: " << key << std::endl;
    exit(1);
    return .0;
}
