#include "hoss_spice.h"
#include <cctype>
#include <algorithm>
#include <regex>
#include <optional>
#include <iostream>
#include <fstream>

void HossSpice::addComponent(const std::string &name, const std::string &ctype,
                             const std::string &value, const std::vector<std::string> &nodes)
{
    components.emplace_back(name, ctype, value, nodes);
    if (model_gen.default_models_.find(value) != model_gen.default_models_.end())
    {
        models_used.push_back(value);
    }
}
// FIXME: Should probably create netlisting class
void HossSpice::generateNetlist(const std::string &filename)
{
    std::ofstream netlist_file(filename);
    if (!netlist_file.is_open())
    {
        std::cerr << "Error opening netlist file: " << filename << "\n";
        return;
    }
    // generate model definitions - this isnt great and should be revised
    netlist_file << model_gen.generateModelDefs(models_used) << "\n";

    // FIXME: this is going to get long as more components are added. Optimize
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
        else if (comp.ctype == "inductor")
        {
            netlist_file << "L" << comp.name << " " << comp.nodes[0] << " " << comp.nodes[1] << " " << comp.value << "\n";
        }
        else if (comp.ctype == "current")
        {
            netlist_file << "I" << comp.name << " " << comp.nodes[0] << " " << comp.nodes[1] << " DC " << comp.value << "\n";
        }
        else if (comp.ctype == "diode")
        {
            netlist_file << "D" << comp.name << " " << comp.nodes[0] << " " << comp.nodes[1] << " " << comp.value << "\n";
        }
        else if (comp.ctype == "nmos" || comp.ctype == "pmos")
        {
            netlist_file << "M" << comp.name << " " << comp.nodes[0] << " " << comp.nodes[1] << " " << comp.nodes[2] << " " << comp.nodes[3] << " " << comp.value << "\n";
        }
        else if (comp.ctype == "npn" || comp.ctype == "pnp")
        {
            netlist_file << "Q" << comp.name << " " << comp.nodes[0] << " " << comp.nodes[1] << " " << comp.nodes[2] << " " << comp.value << "\n";
        }
        else
        {
            std::cerr << "Unknown component type: " << comp.ctype << "\n";
            continue;
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