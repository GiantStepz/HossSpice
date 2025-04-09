// prototype component class

#include "component.h"
#include <string>

    Component::Component(const std::string &name, const std::string &ctype,
    const std::string &value, const std::vector<std::string> &nodes)
    : name(name), ctype(ctype), value(value), nodes(nodes) {}

    std::string Component::toString() const
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
