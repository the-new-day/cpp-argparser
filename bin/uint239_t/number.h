#pragma once

#include <cinttypes>
#include <iostream>

const uint8_t kNumberByteSize = 35;
const uint64_t kMaxShift = 34'359'738'367ULL; // 2^35 - 1
const uint8_t kRealMaxShift = 245; // 280 - 35
const uint8_t kSignificantBytesSize = 30; // ceil((280 - 35 - 6) / 8)

struct uint239_t {
    uint8_t data[kNumberByteSize]{0};
};

static_assert(sizeof(uint239_t) == 35, "Size of uint239_t must be no higher than 35 bytes");

uint239_t FromInt(uint32_t value, uint32_t shift);

uint239_t FromString(const char* str, uint32_t shift);

uint239_t operator+(const uint239_t& lhs, const uint239_t& rhs);

uint239_t operator-(const uint239_t& lhs, const uint239_t& rhs);

uint239_t operator*(const uint239_t& lhs, const uint239_t& rhs);

uint239_t operator/(const uint239_t& lhs, const uint239_t& rhs);

bool operator==(const uint239_t& lhs, const uint239_t& rhs);

bool operator!=(const uint239_t& lhs, const uint239_t& rhs);

bool operator>(const uint239_t& lhs, const uint239_t& rhs);

bool operator<(const uint239_t& lhs, const uint239_t& rhs);

bool operator>=(const uint239_t& lhs, const uint239_t& rhs);

bool operator<=(const uint239_t& lhs, const uint239_t& rhs);

std::ostream& operator<<(std::ostream& stream, const uint239_t& value);

uint64_t GetShift(const uint239_t& value);
