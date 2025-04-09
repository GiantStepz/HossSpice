#pragma once

#ifndef HOSS_ROOT
#define HOSS_ROOT "."
#endif

#include <string>
#include <vector>
#include <unordered_map>
#include "component.h"
#include "model_generator.h"

class HossSpice
{
public:
    std::vector<Component> components;
    std::unordered_map<std::string, std::string> netlist;
    std::unordered_map<std::string, std::string> sim;

    std::vector<std::string> models_used;
    ModelGenerator model_gen;

    void addComponent(const std::string &name, const std::string &ctype,
                      const std::string &value, const std::vector<std::string> &nodes);
    void run();

private:
    void generateNetlist(const std::string &filename);
};