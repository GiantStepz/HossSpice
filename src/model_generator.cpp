#include "model_generator.h"

ModelGenerator::ModelGenerator()
{
    default_models_ = {
        {"1N4148", ".model 1N4148 D (IS=2.52e-9 N=1.752 BV=100 IBV=0.1u)"},
        {"NMOS", ".model NMOS NMOS (LEVEL=1 VTO=0.7 KP=120u)"},
        {"PMOS", ".model PMOS PMOS (LEVEL=1 VTO=-0.7 KP=50u)"},
        {"QNPN", ".model QNPN NPN (IS=1e-15 BF=100 VAF=100)"},
        {"QPNP", ".model QPNP PNP (IS=1e-15 BF=100 VAF=100)"}};
};

std::string ModelGenerator::generateModelDef(const std::string &model_name)
{
    auto it = default_models_.find(model_name);
    if (it != default_models_.end())
    {
        return it->second + "\n";
    }
    else
    {
        std::cerr << "Warning: Model " << model_name << " not found in default models.\n";
        return "";
    }
}

std::string ModelGenerator::generateModelDefs(const std::vector<std::string> &models_used)
{
    std::string model_definitions = "* Automatically generated model definitions\n";
    for (const auto &model : models_used)
    {
        auto it = default_models_.find(model);
        if (it != default_models_.end())
        {
            model_definitions += it->second + "\n";
        }
        else
        {
            std::cerr << "Warning: Model " << model << " not found in default models.\n";
        }
    }
    return model_definitions;
}