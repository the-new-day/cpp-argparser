#include "ArgParser.hpp"

#include <algorithm>
#include <iostream>

namespace ArgumentParser {

ArgParser::~ArgParser() {
    for (auto* argument : arguments_) {
        delete argument;
    }

    for (auto* builder : argument_builders_) {
        delete builder;
    }
}

void ArgParser::SetArguments() {
    for (auto* argument : arguments_) {
        argument->ClearValuesStorage();
        delete argument;
    }

    arguments_.clear();

    for (auto* builder : argument_builders_) {
        arguments_.push_back(builder->Build());
    }
}

std::vector<std::string_view> ArgParser::GetLongNames(std::string_view argument) const {
    bool is_long = false;

    if (argument.starts_with("--")) {
        argument = argument.substr(2);
        is_long = true;
    } else {
        argument = argument.substr(1);
    }

    size_t equal_sign_index = argument.find('=');
    argument = argument.substr(0, equal_sign_index);

    std::vector<std::string_view> names;

    if (is_long) {
        names.push_back(argument);
    } else if (equal_sign_index != std::string_view::npos && argument.length() > 1) {
        return {};
    } else {
        for (const char short_name : argument) {
            if (!short_names_to_long_.contains(short_name)) {
                return {};
            }

            names.push_back(short_names_to_long_.at(short_name));
        }
    }

    return names;
}

bool ArgParser::Parse(const std::vector<std::string>& argv) {
    SetArguments();

    std::vector<size_t> unused_positions;

    for (size_t position = 1; position < argv.size() && argv[position] != "--"; ++position) {
        std::string_view argument = argv[position];
        std::string_view argument_name;

        if (argument[0] != '-' || argument.size() == 1) {
            unused_positions.push_back(position);
            continue;
        }

        std::vector<std::string_view> long_names = GetLongNames(argument);

        if (long_names.empty()) {
            error_ = {argv[position], ArgumentParsingErrorType::kUnknownArgument};
            return false;
        }

        for (const std::string_view long_name : long_names) {
            if (!arguments_indeces_.contains(long_name)) {
                error_ = {argv[position], ArgumentParsingErrorType::kUnknownArgument};
                return false;
            }

            size_t argument_index = arguments_indeces_.at(long_name);

            std::expected<size_t, ArgumentParsingError> current_used_positions 
                = arguments_[argument_index]->ParseArgument(argv, position);

            if (!current_used_positions.has_value()) {
                error_ = current_used_positions.error();
                return false;
            }

            position += current_used_positions.value() - 1;
        }
    }

    ParsePositionalArguments(argv, unused_positions);
    return HandleErrors();
}

void ArgParser::ParsePositionalArguments(const std::vector<std::string>& argv,
                                         const std::vector<size_t>& positions) {
    std::vector<size_t> positional_args_indeces;
    std::vector<std::string_view> positional_args;

    for (size_t i = 0; i < arguments_.size(); ++i) {
        if (arguments_[i]->GetInfo().is_positional) {
            positional_args_indeces.push_back(i);
        }
    }

    for (size_t i = 0; i < positions.size(); ++i) {
        positional_args.push_back(argv[positions[i]]);
    }

    if (positional_args_indeces.empty()) {
        if (!positional_args.empty()) {
            error_ = ArgumentParsingError{argv[positions[0]], ArgumentParsingErrorType::kUnknownArgument};
        }

        return;
    }

    for (size_t argument_index = 0, position = 0;
        position < positions.size() && argument_index < positional_args_indeces.size();
        ++argument_index, ++position) {
        if (arguments_[positional_args_indeces[argument_index]]->GetInfo().is_multi_value) {
            while (position < positions.size()) {
                std::expected<size_t, ArgumentParsingError> current_used_positions 
                    = arguments_[positional_args_indeces[argument_index]]->ParseArgument(argv, positions[position]);

                if (!current_used_positions.has_value()) {
                    error_ = current_used_positions.error();
                    return;
                }

                ++position;
            }

            return;
        }

        std::expected<size_t, ArgumentParsingError> current_used_positions 
            = arguments_[positional_args_indeces[argument_index]]->ParseArgument(argv, positions[position]);

        if (!current_used_positions.has_value()) {
            error_ = current_used_positions.error();
            return;
        }
    }
}

bool ArgParser::Parse(int argc, char **argv) {
    std::vector<std::string> new_argv;
    new_argv.reserve(argc);

    for (size_t i = 0; i < argc; ++i) {
        new_argv.push_back(argv[i]);
    }

    return Parse(new_argv);
}

bool ArgParser::HandleErrors() {
    if (error_.status != ArgumentParsingErrorType::kSuccess) {
        return false;
    }

    for (const Argument* argument : arguments_) {
        ArgumentStatus status = argument->GetValueStatus();
        if (status == ArgumentStatus::kSuccess) {
            continue;
        }
        
        error_.argument_name = argument->GetInfo().long_name;
        return false;
    }

    return true;
}

void ArgParser::AddHelp(char short_name, const std::string& long_name, const std::string& description) {
}

void ArgParser::AddHelp(const std::string& long_name, const std::string& description) {
}

bool ArgParser::Help() const {
    return false;
}

std::string ArgParser::HelpDescription() const {
    return std::string();
}

ArgumentParsingError ArgParser::GetError() const {
    return error_;
}

bool ArgParser::HasError() const {
    return error_.status != ArgumentParsingErrorType::kSuccess;
}

} // namespace ArgumentParser
