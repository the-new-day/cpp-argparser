#include <functional>
#include "lib/ArgParser.hpp"

#include <iostream>
#include <numeric>

struct Options {
    bool sum = false;
    bool mult = false;
};

int main(int argc, char** argv) {
    ArgumentParser::ArgParser parser("My Parser");
    bool flag3 ;
    parser.AddFlag('a', "flag1");
    parser.AddFlag('b', "flag2").Default(true);
    parser.AddFlag('c', "flag3").StoreValue(flag3);

    parser.Parse(argc, argv);

    std::cout << parser.GetValue<bool>("flag2") << std::endl;

    // Options opt;
    // std::vector<int> values;

    // ArgumentParser::ArgParser parser("Program");
    // parser.AddIntArgument("N").MultiValue(1).Positional().StoreValues(values);
    // parser.AddFlag("sum", "add args").StoreValue(opt.sum).Default(true);
    // parser.AddFlag("mult", "multiply args").StoreValue(opt.mult);
    // parser.AddHelp('h', "help", "Program accumulate arguments");

    // if (!parser.Parse(argc, argv)) {
    //     std::cout << (parser.GetError().argument_string) << std::endl;

    //     std::cout << "Wrong argument" << std::endl;
    //     std::cout << parser.HelpDescription() << std::endl;
    //     return 1;
    // }

    // if (parser.Help()) {
    //     std::cout << parser.HelpDescription() << std::endl;
    //     return 0;
    // }

    // if (opt.sum) {
    //     std::cout << "Sum: " << std::accumulate(values.begin(), values.end(), 0) << std::endl;
    // }
    
    // if (opt.mult) {
    //     std::cout << "Product: " << std::accumulate(values.begin(), values.end(), 1, std::multiplies<int>()) << std::endl;
    // } 
    
    // if (!opt.mult && !opt.sum) {
    //     std::cout << "No options was chosen" << std::endl;
    //     std::cout << parser.HelpDescription();
    //     return 1;
    // }

    return 0;

}
