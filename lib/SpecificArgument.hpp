#pragma once

#include "Argument.hpp"

#include <cstddef>
#include <type_traits>
#include <iostream>
#include <expected>

namespace ArgumentParser {

template<typename T>
class SpecificArgument : public Argument {
public:
    SpecificArgument(const ArgumentInfo& info,
                     const T& default_value,
                     T* store_value_to,
                     std::vector<T>* store_values_to);

    const ArgumentInfo& GetInfo() const override;
    const std::string& GetType() const override;
    ArgumentStatus GetValueStatus() const override;
    size_t GetValuesSet() const override;

    std::expected<size_t, ArgumentParsingError> ParseArgument(const std::vector<std::string>& argv,
                                                              size_t position) override;

    T GetValue() const;
    T GetValue(size_t index) const;

    void ClearValuesStorage() override;

protected:
    ArgumentInfo info_;
    ArgumentStatus value_status_;

    T value_;
    T default_value_;

    T* store_value_to_;
    std::vector<T>* store_values_to_;

    size_t values_set_ = 0;

    ArgumentParsingErrorType ParseValue(std::string_view value_string);
};

template <typename T>
std::expected<size_t, ArgumentParsingError> SpecificArgument<T>::ParseArgument(
    const std::vector<std::string>& argv,
    size_t position) {
    if (store_values_to_->empty()) {
        value_status_ = ArgumentStatus::kSuccess;
    }

    bool is_value_being_parsed = false;

    if (info_.is_positional) {
        is_value_being_parsed = true;
    }

    size_t current_used_positions = 1;
    
    std::string_view value_string = argv[position];

    if (!is_value_being_parsed && !info_.is_positional) {
        if (value_string.starts_with("--")) {
            value_string = value_string.substr(2);
        } else if (value_string[0] == '-') {
            value_string = value_string.substr(1);
        }
    }

    size_t equal_sign_index = value_string.find('=');

    if (!info_.is_positional && equal_sign_index != std::string_view::npos) {
        value_string = value_string.substr(equal_sign_index + 1);
    } else if (std::is_same_v<bool, T>) {
        value_string = "";
    } else if (!is_value_being_parsed) {
        ++position;
        ++current_used_positions;
        value_string = argv[position];
    }

    ArgumentParsingErrorType parsing_result = ParseValue(value_string);

    if (parsing_result != ArgumentParsingErrorType::kSuccess) {
        return std::unexpected(ArgumentParsingError{argv[position], parsing_result});
    }

    store_values_to_->push_back(value_);
    ++values_set_;

    if (info_.has_store_value) {
        *store_value_to_ = value_;
    }

    if (value_status_ != ArgumentStatus::kInvalidArgument
        && store_values_to_->size() < info_.minimum_values) {
        value_status_ = ArgumentStatus::kInsufficient;
    }

    return current_used_positions;
}

template<typename T>
SpecificArgument<T>::SpecificArgument(const ArgumentInfo& info,
                                      const T& default_value,
                                      T* store_value_to,
                                      std::vector<T>* store_values_to) 
    : info_(info),
      default_value_(default_value),
      store_value_to_(store_value_to),
      store_values_to_(store_values_to),
      value_status_(ArgumentStatus::kNoArgument) {}

template<typename T>
T SpecificArgument<T>::GetValue() const {
    return value_;
}

template<typename T>
T SpecificArgument<T>::GetValue(size_t index) const {
    return store_values_to_->at(index);
}

template <typename T>
void SpecificArgument<T>::ClearValuesStorage() {
    store_values_to_->clear();
    values_set_ = 0;

    if (store_value_to_ != nullptr) {
        *store_value_to_ = default_value_;
    }
}

template <typename T>
size_t SpecificArgument<T>::GetValuesSet() const {
    return values_set_;
}

template<typename T>
const ArgumentInfo& SpecificArgument<T>::GetInfo() const {
    return info_;
}

template<typename T>
const std::string& SpecificArgument<T>::GetType() const {
    return info_.type;
}

template <typename T>
ArgumentStatus SpecificArgument<T>::GetValueStatus() const {
    return value_status_;
}

} // namespace ArgumentParser
