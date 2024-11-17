#include "ArgParser.hpp"

#include <algorithm>
#include <numeric>

#include <iostream> // TODO: remove

namespace ArgumentParser {
    
ArgParser::ArgParser(const std::string& program_name) : program_name_(program_name) {
    help_description_types_ = {
        {typeid(int32_t).name(), "int"},
        {typeid(std::string).name(), "string"},
        {typeid(bool).name(), ""},
        {typeid(double).name(), "double"}
    };
}

ArgParser::~ArgParser() {
    for (auto* argument : arguments_) {
        delete argument;
    }

    for (auto* builder : argument_builders_) {
        delete builder;
    }
}

void ArgParser::RefreshParser() {
    for (auto* argument : arguments_) {
        argument->ClearValuesStorage();
        delete argument;
    }

    arguments_.clear();

    for (auto* builder : argument_builders_) {
        arguments_.push_back(builder->Build());
    }

    error_ = ArgumentParsingError{};
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
                continue;
            }

            names.push_back(short_names_to_long_.at(short_name));
        }
    }

    if (!is_long && names.size() < argument.length()) {
        if (short_names_to_long_.contains(argument[0])) {
            return {names[0]};
        }

        return {};
    }

    return names;
}

bool ArgParser::Parse(const std::vector<std::string>& argv) {
    RefreshParser();

    std::vector<size_t> unused_positions;

    for (size_t position = 1; position < argv.size(); ++position) {
        std::string_view argument = argv[position];
        std::string_view argument_name;

        if (argument == "--") {
            unused_positions.reserve(argv.size() - position - 1);

            for (size_t i = position + 1; i < argv.size(); ++i) {
                unused_positions.push_back(i);
            }

            break;
        }

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
                error_ = {argv[position], ArgumentParsingErrorType::kUnknownArgument, std::string(long_name)};
                return false;
            }

            size_t argument_index = arguments_indeces_.at(long_name);

            std::expected<size_t, ArgumentParsingError> current_used_positions 
                = arguments_[argument_index]->ParseArgument(argv, position);

            if (!current_used_positions.has_value()) {
                error_ = current_used_positions.error();
                error_.argument_name = long_name;
                error_.argument_string = argv[position];
                return false;
            }

            if (long_name == help_argument_name_) {
                need_help_ = true;
            }

            position += current_used_positions.value() - 1;
        }
    }

    ParsePositionalArguments(argv, unused_positions);

    if (need_help_) {
        HandleErrors();
        return true;
    }

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
        } else if (status == ArgumentStatus::kNoArgument) {
            error_.status = ArgumentParsingErrorType::kNoArgument;
        }
        
        error_.argument_name = argument->GetInfo().long_name;
        return false;
    }

    return true;
}

void ArgParser::AddHelp(char short_name, const std::string& long_name, const std::string& description) {
    AddFlag(short_name, long_name, description);
    help_argument_name_ = long_name;
}

void ArgParser::AddHelp(const std::string& long_name, const std::string& description) {
    AddHelp(kNoShortName, long_name, description);
}

bool ArgParser::Help() const {
    return need_help_;
}

std::string ArgParser::HelpDescription() const {
    std::string result = program_name_ + '\n';

    size_t max_argument_names_length = 0;

    for (const ArgumentBuilder* argument : argument_builders_) {
        size_t length = GetArgumentNamesDescription(argument->GetInfo()).length();
        if (length > max_argument_names_length) {
            max_argument_names_length = length;
        }
    }

    if (!help_argument_name_.empty()) {
        result += arguments_.at(arguments_indeces_.at(help_argument_name_))->GetInfo().description;
        result += '\n';
    }

    // TODO: print positional arguments before options
    // something like "Usage: app [OPTIONS] <arg1> <arg2> <arg3>"

    result += '\n';

    for (const ArgumentBuilder* argument : argument_builders_) {
        ArgumentInfo info = argument->GetInfo();
        if (info.is_positional) {
            continue;
        }

        result += GetArgumentDescription(info, max_argument_names_length);
        result += '\n';
    }

    return result;
}

std::string ArgParser::GetArgumentDescription(const ArgumentInfo& info,
                                              size_t max_argument_names_length) const {
    std::string result = GetArgumentNamesDescription(info);

    result.insert(result.end(), max_argument_names_length - result.length() + 2, ' ');
    result += info.description;

    std::string options = " [";

    bool is_first_option = true;

    // TODO: "repeated, ", "min values = " etc. - should be user-configurable

    if (info.is_multi_value) {
        options += "repeated, ";
        options += "min values = " + std::to_string(info.minimum_values);
        is_first_option = false;
    }

    if (info.has_default) {
        if (!is_first_option) {
            options += "; ";
        }

        options += "default = ";
    }

    options += ']';

    if (options.length() > 3) {
        result += options;
    }

    return result;
}

std::string ArgParser::GetArgumentNamesDescription(const ArgumentInfo& info) const {
    std::string result;
    if (info.short_name == kNoShortName) {
        result.insert(0, 4, ' ');
    } else {
        result = "-";
        result += info.short_name;
        result += ", ";
    }

    result += "--";
    result += info.long_name;

    if (help_description_types_.contains(info.type)) {
        result += "=<";
        result += help_description_types_.at(info.type);
        result += ">";
    }

    return result;
}

ArgumentParsingError ArgParser::GetError() const {
    return error_;
}

bool ArgParser::HasError() const {
    return error_.status != ArgumentParsingErrorType::kSuccess;
}

} // namespace ArgumentParser
