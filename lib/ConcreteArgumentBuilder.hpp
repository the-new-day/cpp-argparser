#pragma once

#include "ArgumentBuilder.hpp"
#include "ConcreteArgument.hpp"

#include <typeinfo>

namespace ArgumentParser {

template<typename T>
class ConcreteArgumentBuilder final : public ArgumentBuilder {
public:
    ConcreteArgumentBuilder() = delete;
    ~ConcreteArgumentBuilder() override;

    ConcreteArgumentBuilder(char short_name,
                            const std::string& long_name,
                            const std::string& description);

    ConcreteArgumentBuilder& Default(T default_value);
    ConcreteArgumentBuilder& MultiValue(size_t min_values);
    ConcreteArgumentBuilder& Positional();
    ConcreteArgumentBuilder& StoreValue(T& to);
    ConcreteArgumentBuilder& StoreValues(std::vector<T>& to);

    Argument* Build() override;

private:
    ArgumentInfo info_;
    ArgumentParsingStatus value_status_;
    
    T default_value_{};
    T* store_value_to_ = nullptr;
    std::vector<T>* store_values_to_ = nullptr;

    bool was_temp_vector_created_ = false;
};

template<typename T>
Argument* ConcreteArgumentBuilder<T>::Build() {
    if (info_.is_multi_value && !info_.has_store_values) {
        store_values_to_ = new std::vector<T>;
        was_temp_vector_created_ = true;
    }

    return new ConcreteArgument<T>(info_, default_value_, store_value_to_, store_values_to_);
}

template<typename T>
ConcreteArgumentBuilder<T>::~ConcreteArgumentBuilder() {
    if (was_temp_vector_created_) {
        delete store_values_to_;
    }
}

template<typename T>
ConcreteArgumentBuilder<T>::ConcreteArgumentBuilder(char short_name,
                                                    const std::string& long_name,
                                                    const std::string& description) {
    info_.long_name = long_name;
    info_.short_name = short_name;
    info_.description = description;
    info_.type = typeid(T).name();
    value_status_ = ArgumentParsingStatus::kNoArgument;
}

template<typename T>
ConcreteArgumentBuilder<T>& ConcreteArgumentBuilder<T>::Default(T default_value) {
    default_value_ = default_value;
    info_.has_default = true;
    return *this;
}

template<typename T>
ConcreteArgumentBuilder<T>& ConcreteArgumentBuilder<T>::MultiValue(size_t min_values) {
    info_.minimum_values = min_values;
    info_.is_multi_value = true;
    return *this;
}

template<typename T>
ConcreteArgumentBuilder<T>& ConcreteArgumentBuilder<T>::Positional() {
    info_.is_positional = true;
    return *this;
}

template<typename T>
ConcreteArgumentBuilder<T>& ConcreteArgumentBuilder<T>::StoreValue(T& to) {
    store_value_to_ = &to;
    info_.has_store_value = true;
    return *this;
}

template<typename T>
ConcreteArgumentBuilder<T>& ConcreteArgumentBuilder<T>::StoreValues(std::vector<T>& to) {
    store_values_to_ = &to;
    info_.has_store_values = true;
    return *this;
}

} // namespace ArgumentParser

