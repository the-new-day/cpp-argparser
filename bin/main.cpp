#include <functional>
#include "lib/ArgParser.hpp"

#include <iostream>
#include <numeric>
#include <type_traits>
#include <algorithm>

#include "uint239_t/number.h"

using namespace ArgumentParser;

bool IsNumber(std::string_view str) {
    if (str[0] == '-') {
        str = str.substr(1);
    }

    return std::find_if_not(str.begin(), str.end(), isdigit) == str.end();
}

template<>
std::expected<uint239_t, ArgumentParsingErrorType> ArgumentParser::ParseValue<uint239_t>(std::string_view value_string) {
    if (!IsNumber(value_string)) {
        return std::unexpected(ArgumentParsingErrorType::kInvalidArgument);
    }

    return FromString(value_string.data(), 0);
}

struct Options {
    bool sum = false;
    bool mult = false;
};

int main(int argc, char** argv) {
    Options opt;
    std::vector<uint239_t> values;

    ArgumentParser::ArgParser parser("Program", "Program accumulate arguments");
    parser.AddArgument<uint239_t>("N").MultiValue(1).Positional().StoreValues(values);
    parser.AddFlag("sum", "add args").StoreValue(opt.sum);
    parser.AddFlag("mult", "multiply args").StoreValue(opt.mult);
    parser.AddHelp('h', "help", "Display help and exit");

    if (!parser.Parse(argc, argv)) {
        std::cout << (parser.GetError().argument_string) << std::endl;

        std::cout << "Wrong argument" << std::endl;
        std::cout << parser.HelpDescription() << std::endl;
        return 1;
    }

    if (parser.Help()) {
        std::cout << parser.HelpDescription() << std::endl;
        return 0;
    }

    if (opt.sum) {
        uint239_t sum = FromInt(0, 0);

        for (const uint239_t& number : values) {
            sum = sum + number;
        }

        std::cout << "Sum: " << sum << std::endl;
    }
    
    if (opt.mult) {
        uint239_t product = values[0];

        for (size_t i = 1; i < values.size(); ++i) {
            product = product * values[i];
        }

        std::cout << "Product: " << product << std::endl;
    } 
    
    if (!opt.mult && !opt.sum) {
        std::cout << "No options was chosen" << std::endl;
        std::cout << parser.HelpDescription();
        return 1;
    }

    return 0;

}
