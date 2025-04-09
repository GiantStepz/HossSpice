#pragma once

#include <string>
#include <vector>
#include <sstream>

class Component
{
public:
    std::string name;
    std::string ctype;
    std::string value;
    std::vector<std::string> nodes;

    Component(const std::string &name, const std::string &ctype,
              const std::string &value, const std::vector<std::string> &nodes);

    ~Component() = default;

    std::string toString() const;
};
