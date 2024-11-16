#pragma once

#include "ArgumentBuilder.hpp"
#include "SpecificArgument.hpp"

#include <typeinfo>
#include <type_traits>

namespace ArgumentParser {

template<typename T>
class SpecificArgumentBuilder : public ArgumentBuilder {
public:
    SpecificArgumentBuilder() = delete;
    ~SpecificArgumentBuilder() override;

    SpecificArgumentBuilder(char short_name,
                            const std::string& long_name,
                            const std::string& description);

    const ArgumentInfo& GetInfo() const override;

    SpecificArgumentBuilder& Default(T default_value);
    SpecificArgumentBuilder& MultiValue(size_t min_values = 0);
    SpecificArgumentBuilder& Positional();
    SpecificArgumentBuilder& StoreValue(T& to);
    SpecificArgumentBuilder& StoreValues(std::vector<T>& to);

    Argument* Build() override;

private:
    ArgumentInfo info_;
    
    T default_value_{};
    T* store_value_to_ = nullptr;
    std::vector<T>* store_values_to_ = nullptr;

    std::vector<std::vector<T>*> temp_vectors_;
};

template<typename T>
Argument* SpecificArgumentBuilder<T>::Build() {
    if (std::is_same_v<bool, T>) {
        if (!info_.has_default) {
            info_.has_default = true;
            default_value_ = false;
        }
    }

    if (!info_.has_store_values) {
        store_values_to_ = new std::vector<T>;
        temp_vectors_.push_back(store_values_to_);
    }

    return new SpecificArgument<T>(info_, default_value_, store_value_to_, store_values_to_);
}

template<typename T>
SpecificArgumentBuilder<T>::~SpecificArgumentBuilder() {
    for (auto temp_vector : temp_vectors_) {
        delete temp_vector;
    }
}

template<typename T>
SpecificArgumentBuilder<T>::SpecificArgumentBuilder(char short_name,
                                                    const std::string& long_name,
                                                    const std::string& description) {
    info_.long_name = long_name;
    info_.short_name = short_name;
    info_.description = description;
    info_.type = typeid(T).name();
}

template <typename T>
const ArgumentInfo &SpecificArgumentBuilder<T>::GetInfo() const {
    return info_;
}

template<typename T>
SpecificArgumentBuilder<T>& SpecificArgumentBuilder<T>::Default(T default_value) {
    default_value_ = default_value;
    info_.has_default = true;
    return *this;
}

template<typename T>
SpecificArgumentBuilder<T>& SpecificArgumentBuilder<T>::MultiValue(size_t min_values) {
    info_.minimum_values = min_values;
    info_.is_multi_value = true;
    return *this;
}

template<typename T>
SpecificArgumentBuilder<T>& SpecificArgumentBuilder<T>::Positional() {
    info_.is_positional = true;
    return *this;
}

template<typename T>
SpecificArgumentBuilder<T>& SpecificArgumentBuilder<T>::StoreValue(T& to) {
    store_value_to_ = &to;
    info_.has_store_value = true;
    return *this;
}

template<typename T>
SpecificArgumentBuilder<T>& SpecificArgumentBuilder<T>::StoreValues(std::vector<T>& to) {
    store_values_to_ = &to;
    info_.has_store_values = true;
    return *this;
}

} // namespace ArgumentParser
