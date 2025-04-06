#pragma once

#ifndef HOSS_ROOT
#define HOSS_ROOT "."
#endif

#include <string>
#include <vector>
#include <unordered_map>
#include "component.h"

class HossSpice
{
public:
    void parseLine(const std::string &input_line);
    void run();

private:
    std::vector<Component> components;
    std::unordered_map<std::string, std::string> netlist;
    std::unordered_map<std::string, std::string> sim;

    void addComponent(const std::string &name, const std::string &ctype,
                      const std::string &value, const std::vector<std::string> &nodes);

    void parseSimulation(const std::string &line);
    void generateNetlist(const std::string &filename);
};