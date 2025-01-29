#include <functional>
#include "lib/ArgParser.hpp"

#include <iostream>
#include <numeric>

struct Options {
    bool sum = false;
    bool mult = false;
};

int main(int argc, char** argv) {
    Options opt;
    std::vector<int> values;

    ArgumentParser::ArgParser parser("Program", "Program accumulate arguments");
    parser.AddArgument<int>("numbers").MultiValue(1).Positional().StoreValues(values);
    parser.AddFlag('s', "sum", "Sum arguments").StoreValue(opt.sum);
    parser.AddFlag('m', "mult", "Multiply arguments").StoreValue(opt.mult);
    parser.AddArgument<std::string>('n', "name", "Your name").Default("John Doe");
    parser.AddHelp('h', "help", "Show help and exit");

    if (!parser.Parse(argc, argv)) {
        std::cout << "Wrong argument" << std::endl;
        std::cout << parser.HelpDescription() << std::endl;
        return 1;
    }

    if (parser.Help()) {
        std::cout << parser.HelpDescription() << std::endl;
        return 0;
    }

    if (!opt.sum && !opt.mult) {
        std::cout << "No options have been chosen" << std::endl;
        std::cout << parser.HelpDescription();
        return 1;
    }

    std::cout << "Hello " << *parser.GetValue<std::string>("name") << '!' << std::endl;

    if (opt.sum) {
        std::cout << "Sum: " << std::accumulate(values.begin(), values.end(), 0) << std::endl;
    }
    
    if (opt.mult) {
        std::cout << "Product: " << std::accumulate(values.begin(), values.end(), 1, std::multiplies<int>()) << std::endl;
    }

    return 0;
}