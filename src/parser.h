#include <tao/pegtl.hpp>
#include "hoss_spice.h"
#include "hoss_grammar.hpp"
#include <string>

class Parser
{
public:
    Parser(HossSpice &engine);

    void parseLine(const std::string &input_line);
    void parseSimulation(const std::string &input_line);

private:
    HossSpice &engine;
};