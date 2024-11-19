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
std::optional<uint239_t> ArgumentParser::ParseValue<uint239_t>(std::string_view value_string) {
    if (!IsNumber(value_string)) {
        return std::nullopt;
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
    parser.AddArgument<uint239_t>("N").MultiValue(2).Positional().StoreValues(values);
    parser.AddFlag('s', "sum", "add args").StoreValue(opt.sum);
    parser.AddFlag('m', "mult", "multiply args").StoreValue(opt.mult);
    parser.AddHelp('h', "help", "display help and exit");

    if (!parser.Parse(argc, argv)) {
        auto error = parser.GetError();

        if (error.status == ParsingErrorType::kUnknownArgument) {
            std::cerr << "Unknown argument: " << error.argument_string << std::endl;
            return 1;
        }

        std::cerr << "An error occured while parsing the following argument: " 
            << (error.argument_name) << std::endl;

        if (error.status == ParsingErrorType::kInvalidArgument) {
            std::cerr << "Unable to parse: " << error.argument_string << std::endl;
        } else if (error.status == ParsingErrorType::kNoArgument) {
            std::cerr << "No value was specified" << std::endl;
        } else if (error.status == ParsingErrorType::kInsufficent) {
            std::cerr << "Not enough values were specified" << std::endl;
        }

        return 1;
    }

    if (parser.Help()) {
        std::cout << parser.HelpDescription() << std::endl;
        return 0;
    }

    std::cout << "You set " << *parser.GetValuesSet("sum") << " values." << std::endl;

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
