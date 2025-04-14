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
    std::string block;
    bool in_macro = false;
    while (std::getline(file, line))
    {
        if (in_macro)
        {
            block += line + "\n";
            if (line.find('}') != std::string::npos)
            {
                // full macro block captured
                parser.parseLine(block);
                block.clear();
                in_macro = false;
            }
        } // if macro def is found, the full block is needed for the line
        else if (line.find("macro") == 0)
        {
            block = line + "\n";
            in_macro = true;
        }
        else
        {
            parser.parseLine(line);
        }
    }
    engine.run();
}
