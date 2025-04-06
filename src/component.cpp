// // prototype component class

// #include "component.h"
// #include <string>

// Component::Component(const std::string &name, const std::string &ctype,
//                      const std::string &value, const std::string &nodes)
//     : name(name), ctype(ctype), value(value), nodes(nodes) {}

// std::string Component::toString() const
// {
//     std::string ctype_upper = ctype;
//     for (auto &ch : ctype_upper)
//     {
//         ch = std::toupper(ch);
//     }
//     return ctype_upper + " " + name + ": " + value + ", nodes: " + nodes;
// }
