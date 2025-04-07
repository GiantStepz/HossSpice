#include "parser.h"
#include <iostream>
#include <regex>

Parser::Parser(HossSpice &engine) : engine(engine) {}

void Parser::parseLine(const std::string &input_line)
{
    std::string line = input_line;
    line.erase(0, line.find_first_not_of(" \t\n\r"));
    line.erase(line.find_last_not_of(" \t\n\r") + 1);

    if (line.empty() || line[0] == '.' || line[0] == '*' || line[0] == '#')
        return;

    if (line.find("simulate") == 0)
    {
        parseSimulation(line);
        return;
    }

    try
    {
        hoss::parsed_component result;
        tao::pegtl::memory_input<> in(line, "input");

        tao::pegtl::parse<hoss::grammar, hoss::action>(in, result);
        // FIXME: ground value is being hardcoded. fix in grammar
        result.value.empty() ? result.value = "0" : result.value;
        // FIXME: voltage and ground nodes are being hardcoded. fix in grammar
        if (result.nodes.empty())
            result.nodes.push_back(result.name);
        engine.addComponent(result.name, result.type, result.value, result.nodes);
    }
    catch (const tao::pegtl::parse_error &e)
    {
        const auto p = e.positions.front();
        std::cerr << "Parsing error at " << p.line << ":" << p.byte_in_line << " - " << e.what() << "\n";
    }
}

void Parser::parseSimulation(const std::string &input_line)
{
    std::smatch match;
    if (std::regex_match(input_line, match, std::regex(R"(simulate\s+(\w+)\s+([\d\w.]+)\s+to\s+([\d\w.]+)\s+step\s+([\d\w.]+))")))
    {
        engine.sim = {
            {"type", match[1]},
            {"start", match[2]},
            {"end", match[3]},
            {"step", match[4]},
        };
    }
}
