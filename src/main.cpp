#include <iostream>
#include <fstream>
#include "hoss_spice.h"
#include "parser.h"

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        std::cerr << "Usage: ./hoss <input_file.hsp>\n";
        return 1;
    }

    const std::string input_file = argv[1];

    std::ifstream file(input_file);
    if (!file.is_open())
    {
        std::cerr << "Error opening file: " << input_file << "\n";
        return 1;
    }

    HossSpice engine;
    Parser parser(engine);
    std::string line;
    while (std::getline(file, line))
    {
        parser.parseLine(line);
    }
    engine.run();
}
