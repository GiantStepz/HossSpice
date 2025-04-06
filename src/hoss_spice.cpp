#include "hoss_spice.h"
#include <cctype>
#include <algorithm>
#include <regex>
#include <optional>
#include <iostream>
#include <fstream>

void HossSpice::parseLine(const std::string &input_line)
{
    std::string line = std::regex_replace(input_line, std::regex("^\\s+|\\s+$"), "");

    if (line.empty() || line[0] == '.' || line[0] == '*' || line[0] == '#')
        return;

    if (line.find("simulate") == 0)
    {
        parseSimulation(line);
        return;
    }

    std::smatch match;
    if (line.find("voltage") != std::string::npos)
    {
        // std::cout << "Found voltage source: " << line << "\n";
        if (std::regex_match(line, match, std::regex(R"((\w+)\s*=\s*voltage\s+([^\s]+))")))
        {
            // std::cout << "Adding voltage source: " << match[1] << " = " << match[2] << "\n";
            addComponent(match[1], "voltage", match[2], {match[1]});
        }
    }
    else if (line.find("ground") != std::string::npos)
    {
        if (std::regex_match(line, match, std::regex(R"((\w+)\s*=\s*ground)")))
        {
            addComponent(match[1], "ground", "0V", {match[1]});
        }
    }
    else if (std::regex_match(line, match, std::regex(R"((\w+)\s*=\s*(\w+)\s+([^\s]+)\s+between\s+(\w+)\s+and\s+(\w+))")))
    {
        addComponent(match[1], match[2], match[3], {match[4], match[5]});
    }
}

void HossSpice::addComponent(const std::string &name, const std::string &ctype,
                             const std::string &value, const std::vector<std::string> &nodes)
{
    components.emplace_back(name, ctype, value, nodes);
}

void HossSpice::parseSimulation(const std::string &line)
{
    // std::cout << "Parsing simulation line: " << line << "\n";
    std::smatch match;
    if (std::regex_match(line, match, std::regex(R"(simulate\s+(\w+)\s+([\d\w.]+)\s+to\s+([\d\w.]+)\s+step\s+([\d\w.]+))")))
    {
        // std::cout << "Found simulation parameters: " << match[1] << ", " << match[2] << ", " << match[3] << ", " << match[4] << "\n";
        sim = {
            {"type", match[1]},
            {"start", match[2]},
            {"end", match[3]},
            {"step", match[4]},
        };
    }
}

void HossSpice::generateNetlist(const std::string &filename)
{
    std::ofstream netlist_file(filename);
    if (!netlist_file.is_open())
    {
        std::cerr << "Error opening netlist file: " << filename << "\n";
        return;
    }

    netlist_file << "* HossSpice Generated Netlist\n";
    for (const auto &comp : components)
    {
        if (comp.ctype == "voltage")
        {
            netlist_file << "V" << comp.name << " " << comp.nodes[0] << " 0 DC " << comp.value << "\n";
        }
        else if (comp.ctype == "ground")
        {
            continue; // ground always node 0
        }
        else if (comp.ctype == "resistor")
        {
            netlist_file << "R" << comp.name << " " << comp.nodes[0] << " " << comp.nodes[1] << " " << comp.value << "\n";
        }
        else if (comp.ctype == "capacitor")
        {
            netlist_file << "C" << comp.name << " " << comp.nodes[0] << " " << comp.nodes[1] << " " << comp.value << "\n";
        }
    }
    if (sim["type"] == "transient")
    {
        netlist_file << ".tran " << sim["step"] << " " << sim["end"] << "\n";
    }
    netlist_file << ".end\n";

    netlist_file.close();
    std::cout << "\n[+] Netlist written to: " << filename << "\n";
}

void HossSpice::run()
{
    const std::string output_dir = HOSS_ROOT "/output";
    if (system(("mkdir -p " + output_dir).c_str()) != 0)
    {
        std::cerr << "Error creating output directory: " << output_dir << "\n";
        return;
    }
    const std::string output_file = output_dir + "/netlist.cir";

    std::cout << "== HossSpice Netlist ==\n";
    for (const auto &comp : components)
    {
        std::cout << comp.toString() << "\n";
    }

    std::cout << "\n== Simulation Settings ==\n";
    for (const auto &[k, v] : sim)
    {
        std::cout << k << ": " << v << "\n";
    }
    generateNetlist(output_file);
}