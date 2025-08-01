#pragma once

#include <tao/pegtl.hpp>
#include "hoss_spice.h"
#include "hoss_grammar.hpp"
#include <string>

class Parser
{
public:
    // FIXME: consider explicitly passing the engine to the parser
    Parser(HossSpice &engine);

    void parseLine(const std::string &input_line);
    std::string trim(const std::string &s);
    void parseSimulation(const std::string &input_line);
    std::vector<hoss::parsed_component> expand_macro(const hoss::macro_call &call);

private:
    HossSpice &engine;
};