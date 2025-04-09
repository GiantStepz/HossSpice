#pragma once

#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>

class ModelGenerator
{
    // look into using something like xgboost in the future
public:
    ModelGenerator();
    ~ModelGenerator() = default;

    std::unordered_map<std::string, std::string> default_models_;

    // generate string with default model definitions
    std::string generateModelDef(const std::string &model_name);
    std::string generateModelDefs(const std::vector<std::string> &models_used);
};