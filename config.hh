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

private:
    void fill_items(std::string config);
    void fill_defaults();
    void print_config();

    std::set<std::string> keys() const;

    std::unordered_map<std::string, double> config_;
    std::unordered_map<std::string, double> defaults_;
};

#endif
