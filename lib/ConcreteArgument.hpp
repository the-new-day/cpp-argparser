#pragma once

#include "Argument.hpp"

#include <cstddef>

namespace ArgumentParser {

template<typename T>
class ConcreteArgument : public Argument {
public:
    ConcreteArgument(const ArgumentInfo& info,
                     const T& default_value,
                     T* store_value_to,
                     std::vector<T>* store_values_to);

    const ArgumentInfo& GetInfo() const override;
    const std::string& GetType() const override;
    ArgumentParsingStatus GetValueStatus() const override;

    std::vector<size_t> ParseArgument(const std::vector<std::string>& argv, size_t position) override;

    T GetValue() const;
    T GetValue(size_t index) const;

    size_t GetValuesSet() const;

protected:
    ArgumentInfo info_;
    ArgumentParsingStatus value_status_;

    T value_;
    T default_value_;

    T* store_value_to_;
    std::vector<T>* store_values_to_;

    size_t values_set_ = 0;

    void ParseValue(std::string_view value_string);
};

template <typename T>
std::vector<size_t> ConcreteArgument<T>::ParseArgument(const std::vector<std::string>& argv, size_t position) {
    std::vector<size_t> used_positions;

    if (store_values_to_->empty()) {
        value_status_ = ArgumentParsingStatus::kSuccess;
    }

    while (used_positions.empty() || info_.is_multi_value) {
        std::string_view value_string = argv[position];
        used_positions.push_back(position);

        if (value_string[0] == '-') {
            size_t equal_sign_index = value_string.find('=');

            if (equal_sign_index != std::string_view::npos) {
                value_string = value_string.substr(equal_sign_index + 1);
            } else {
                ++position;
                used_positions.push_back(position);
                value_string = argv[position];
            }
        }

        ParseValue(value_string);

        store_values_to_->push_back(value_);
        ++values_set_;

        if (info_.has_store_value) {
            *store_value_to_ = value_;
        }

        if (argv[position][0] == '-') {
            break;
        }
    }

    if (value_status_ != ArgumentParsingStatus::kInvalidArgument 
        && store_values_to_->size() < info_.minimum_values) {
        value_status_ = ArgumentParsingStatus::kInsufficientArguments;
    }

    return used_positions;
}

template<typename T>
ConcreteArgument<T>::ConcreteArgument(const ArgumentInfo& info,
                                      const T& default_value,
                                      T* store_value_to,
                                      std::vector<T>* store_values_to) 
    : info_(info),
      default_value_(default_value),
      store_value_to_(store_value_to),
      store_values_to_(store_values_to),
      value_status_(ArgumentParsingStatus::kNoArgument) {}

template<typename T>
T ConcreteArgument<T>::GetValue() const {
    return value_;
}

template<typename T>
T ConcreteArgument<T>::GetValue(size_t index) const {
    return store_values_to_->at(index);
}

template <typename T>
size_t ConcreteArgument<T>::GetValuesSet() const {
    return values_set_;
}

template<typename T>
const ArgumentInfo& ConcreteArgument<T>::GetInfo() const {
    return info_;
}

template<typename T>
const std::string& ConcreteArgument<T>::GetType() const {
    return info_.type;
}

template <typename T>
ArgumentParsingStatus ConcreteArgument<T>::GetValueStatus() const {
    return value_status_;
}

} // namespace ArgumentParser

