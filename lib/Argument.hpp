#pragma once

#include <string>
#include <string_view>
#include <cstddef>
#include <vector>
#include <expected>

namespace ArgumentParser {

const char kNoShortName = 0;

enum class ArgumentStatus {
    kSuccess,
    kNoArgument,
    kInvalidArgument,
    kInsufficient
};

enum class ArgumentParsingErrorType {
    kInsufficent,
    kInvalidArgument,
    kUnknownArgument,
    kNoArgument,
    kSuccess
};

struct ArgumentParsingError {
    std::string argument_string{};
    ArgumentParsingErrorType status = ArgumentParsingErrorType::kSuccess;
    std::string argument_name{};
};

class Argument {
public:
    virtual const std::string& GetType() const = 0;
    virtual ArgumentStatus GetValueStatus() const = 0;
    virtual size_t GetValuesSet() const = 0;
    virtual const std::string& GetDefaultValueString() const = 0;
    virtual void SetDefaultValueString(const std::string& str) = 0;

    virtual const std::string& GetDescription() const = 0;
    virtual const std::string& GetLongName() const = 0;
    virtual char GetShortName() const = 0;

    virtual bool IsPositional() const = 0;
    virtual bool IsMultiValue() const = 0;
    virtual bool HasDefault() const = 0;
    virtual size_t GetMinimumValues() const = 0;

    virtual std::expected<size_t, ArgumentParsingError> ParseArgument(const std::vector<std::string>& argv,
                                                                      size_t position) = 0;

    virtual void Clear() = 0;
};

} // namespace ArgumentParser
