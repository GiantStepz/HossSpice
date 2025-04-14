#include "parser.h"
#include <iostream>
#include <regex>
#include <unordered_set>

Parser::Parser(HossSpice &engine) : engine(engine) {}

static const std::unordered_set<std::string> global_nets = {
    "vdd", "vddh", "gnd", "vss", "avdd", "dvdd", "avddh", "dvddh", "vcc", "vref"};

void Parser::parseLine(const std::string &input_line)
{
    std::string line = trim(input_line);

    if (line.empty() || line[0] == '#')
        return;

    if (line.find("simulate") == 0)
    {
        parseSimulation(line);
        return;
    }

    tao::pegtl::memory_input<> in(line, "input");

    try
    {
        // check for macro definition
        if (line.rfind("macro", 0) == 0)
        {
            hoss::current_macro state;
            hoss::parsed_component comp;
            tao::pegtl::parse<hoss::macro_definition, hoss::macro_action>(in, state, comp);
            return;
        }
        // check for macro instantiation
        if (line.find('=') != std::string::npos && line.find('(') != std::string::npos)
        {
            hoss::macro_call call;
            tao::pegtl::parse<hoss::macro_instantiation, hoss::macro_instantiation_action>(in, call);

            auto expanded_components = expand_macro(call);
            for (const auto &comp : expanded_components)
            {
                engine.addComponent(comp.name, comp.type, comp.value, comp.nodes);
            }
            return;
        }

        // parse regular component line
        hoss::parsed_component result;
        tao::pegtl::parse<hoss::component_line, hoss::component_action>(in, result);
        // FIXME: ground value is being hardcoded. fix in grammar
        result.value.empty() ? result.value = "0V" : result.value;
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
    catch (const std::runtime_error &e)
    {
        std::cerr << "Runtime error: " << e.what() << "\n";
    }
}

std::string Parser::trim(const std::string &s)
{
    const auto begin = s.find_first_not_of(" \t\r\n");
    if (begin == std::string::npos)
        return "";
    const auto end = s.find_last_not_of(" \t\r\n");
    return s.substr(begin, end - begin + 1);
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

std::vector<hoss::parsed_component> Parser::expand_macro(const hoss::macro_call &call)
{
    std::vector<hoss::parsed_component> expanded;

    // step 1: lookup macro
    auto it = hoss::macro_table.find(call.macro_name);
    if (it == hoss::macro_table.end())
    {
        throw std::runtime_error("Macro not found: " + call.macro_name);
    }

    const hoss::parsed_macro &macro = it->second;

    if (macro.args.size() != call.call_args.size())
    {
        throw std::runtime_error("Argument count mismatch for macro " + call.macro_name);
    }

    // step 2: create substitution map
    std::unordered_map<std::string, std::string> net_map;
    for (size_t i = 0; i < macro.args.size(); ++i)
    {
        net_map[macro.args[i]] = call.call_args[i];
    }

    // step 3: expand macro body and parse raw macro body lines
    for (const auto &line : macro.raw_lines)
    {
        std::string trimmed_line = trim(line);
        if (trimmed_line.empty())
            continue;
        tao::pegtl::memory_input<> in(trimmed_line, "macro_body_line");
        hoss::parsed_component comp;

        try
        {
            tao::pegtl::parse<hoss::component_line, hoss::component_action>(in, comp);
        }
        catch (const tao::pegtl::parse_error &e)
        {
            const auto p = e.positions.front();
            std::cerr << "[ERROR] Failed to parse macro component: " << e.what()
                      << " at line " << p.line << ", column " << p.byte_in_line << "\n";
            continue; // skip this component
        }

        hoss::parsed_component new_comp;
        new_comp.name = call.instance_name + "_" + comp.name;
        new_comp.type = comp.type;
        new_comp.value = comp.value;

        for (const auto &node : comp.nodes)
        {
            if (net_map.count(node))
            {
                new_comp.nodes.push_back(net_map[node]);
            }
            else if (global_nets.count(node)) // i want to leave global nets alone
            {
                new_comp.nodes.push_back(node);
            }
            else
            {
                std::string renamed = call.instance_name + "_" + node;
                net_map[node] = renamed;
                new_comp.nodes.push_back(renamed);
            }
        }

        expanded.push_back(new_comp);
    }

    return expanded;
}
