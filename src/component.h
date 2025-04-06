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
              const std::string &value, const std::vector<std::string> &nodes)
        : name(name), ctype(ctype), value(value), nodes(nodes) {}

    ~Component() = default;

    std::string toString() const
    {
        std::ostringstream oss;

        std::string ctype_upper = ctype;
        for (auto &ch : ctype_upper)
        {
            ch = std::toupper(ch);
        }
        oss << ctype_upper << " " << name << ": " << value << ", nodes: [";
        for (int i = 0; i < nodes.size(); ++i)
        {
            oss << nodes[i];
            if (i != nodes.size() - 1)
            {
                oss << ", ";
            }
        }
        oss << "]";
        return oss.str();
    }
};
