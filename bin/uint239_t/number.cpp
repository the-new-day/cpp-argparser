#include "number.h"

#include <cstddef>
#include <cstring>

void SetShiftBits(uint239_t& number, uint64_t shift);
void ShiftSignificantBitsToLeft(uint239_t& number, uint64_t shift);
void ShiftToLeft(uint239_t& number, uint64_t shift);
void ExtractSignificantBits(const uint239_t& number, uint8_t to[kSignificantBytesSize]);
void SetSignificantBits(uint239_t& number, uint8_t from[kSignificantBytesSize]);

uint239_t FromInt(uint32_t value, uint32_t shift) {
    uint239_t result;

    uint8_t significant_bytes[kSignificantBytesSize]{0};

    for (size_t i = 0; i < sizeof(value); ++i) {
        significant_bytes[kSignificantBytesSize - i - 1] = (value >> (8 * i)) & 0xff;
    }

    SetSignificantBits(result, significant_bytes);
    ShiftToLeft(result, shift);

    return result;
}

void DivideByTwo(char* number) {
    uint8_t add = 0;
    
    for (int i = 0; i < std::strlen(number); ++i) {
        char digit = number[i] - '0';
        
        number[i] = (digit / 2 + add) + '0';
        add = (digit % 2 == 0) ? 0 : 5;
    }
}

bool AreAllChars(const char* str, char ch) {
    for (size_t i = 0; i < std::strlen(str); ++i) {
        if (str[i] != ch) {
            return false;
        }
    }

    return true;
}

uint239_t FromString(const char* str, uint32_t shift) {
    uint239_t result;

    size_t str_length = std::strlen(str);
    uint8_t significant_bits[kSignificantBytesSize];

    char* current_number = new char[str_length + 1];
    std::strcpy(current_number, str);

    uint8_t current_digit = kNumberByteSize - 1;
    uint8_t write_to_bit = 7;

    while (!AreAllChars(current_number, '0')) {
        if (write_to_bit == 0) {
            --current_digit;
            write_to_bit = 7;
        }

        if ((current_number[str_length - 1] - '0') % 2 == 1) {
            result.data[current_digit] |= 1 << (7 - write_to_bit);
        }

        --write_to_bit;

        DivideByTwo(current_number);
    }

    ShiftToLeft(result, shift);
    delete[] current_number;
    return result;
}

uint239_t operator+(const uint239_t& lhs, const uint239_t& rhs) {
    uint8_t significant_bytes_lhs[kSignificantBytesSize];
    uint8_t significant_bytes_rhs[kSignificantBytesSize];
    ExtractSignificantBits(lhs, significant_bytes_lhs);
    ExtractSignificantBits(rhs, significant_bytes_rhs);

    // perform ariphmetic operations with base-256 numbers

    uint8_t result_bytes[kSignificantBytesSize]{0};
    uint16_t carry = 0;

    for (int32_t i = kSignificantBytesSize - 1; i >= 0; --i) {
        uint32_t result_digit = significant_bytes_lhs[i] + significant_bytes_rhs[i] + carry;
        carry = result_digit / 256;
        result_bytes[i] = result_digit % 256;
    }

    uint239_t result;

    SetSignificantBits(result, result_bytes);
    ShiftToLeft(result, (GetShift(lhs) + GetShift(rhs)) % kMaxShift);

    return result;
}

uint239_t operator-(const uint239_t& lhs, const uint239_t& rhs) {
    uint8_t significant_bytes_lhs[kSignificantBytesSize];
    uint8_t significant_bytes_rhs[kSignificantBytesSize];
    ExtractSignificantBits(lhs, significant_bytes_lhs);
    ExtractSignificantBits(rhs, significant_bytes_rhs);

    uint8_t result_bytes[kSignificantBytesSize]{0};

    uint239_t result;

    uint8_t carry = 0;
    for (int32_t i = kSignificantBytesSize - 1; i >= 0; --i) {
        int32_t result_digit = significant_bytes_lhs[i] - significant_bytes_rhs[i] - carry;
        carry = 0;

        if (result_digit < 0) {
            carry = 1;
            result_digit += 256;
        }

        result_bytes[i] = result_digit;
    }

    SetSignificantBits(result, result_bytes);
    ShiftToLeft(result, (GetShift(lhs) - GetShift(rhs)) % kMaxShift);

    return result;
}

uint239_t operator*(const uint239_t& lhs, const uint239_t& rhs) {
    uint8_t significant_bytes_lhs[kSignificantBytesSize];
    uint8_t significant_bytes_rhs[kSignificantBytesSize];
    ExtractSignificantBits(lhs, significant_bytes_lhs);
    ExtractSignificantBits(rhs, significant_bytes_rhs);

    uint239_t result;

    for (int32_t i = kSignificantBytesSize - 1; i >= 0; --i) {
        uint8_t partial_product_bytes[kSignificantBytesSize]{0};
        int32_t carry = 0;
        
        int32_t offset = i;

        for (int32_t j = kSignificantBytesSize - 1; j >= 0 && offset >= 0; --j) {
            uint16_t result_byte = significant_bytes_lhs[j] * significant_bytes_rhs[i] + carry;

            partial_product_bytes[offset] = result_byte % 256;
            carry = result_byte / 256;

            --offset;
        }

        uint239_t partial_product;
        SetSignificantBits(partial_product, partial_product_bytes);

        result = result + partial_product;
    }

    ShiftToLeft(result, GetShift(lhs) + GetShift(rhs));

    return result;
}

uint8_t GetFirstQuotientDigit(const uint239_t& lhs, const uint239_t& rhs) {
    uint16_t first = 0;
    uint16_t last = 256;
    
    while (first < last - 1) {
        uint16_t guess = (last + first) / 2;
        if (FromInt(guess, 0) * rhs > lhs) {
            last = guess;
        } else {
            first = guess;
        }
    }
    
    return first;
}

uint239_t operator/(const uint239_t& lhs, const uint239_t& rhs) {
    uint8_t significant_from_lhs[kSignificantBytesSize];
    uint8_t significant_from_rhs[kSignificantBytesSize];
    ExtractSignificantBits(lhs, significant_from_lhs);
    ExtractSignificantBits(rhs, significant_from_rhs);

    uint239_t result;

    size_t i = 0;
    for (; i < kSignificantBytesSize; ++i) {
        if (significant_from_lhs[i] != 0) {
            break;
        }
    }

    if (i == kSignificantBytesSize) {
        return result;
    }

    size_t lhs_significant_digits_offset = i;

    uint8_t result_bytes[kSignificantBytesSize]{0};
    uint8_t dividend_bytes[kSignificantBytesSize]{0};

    for (; i < kSignificantBytesSize; ++i) {
        for (size_t j = kSignificantBytesSize - lhs_significant_digits_offset; j < kSignificantBytesSize; ++j) {
            dividend_bytes[j - 1] = dividend_bytes[j];
        }

        dividend_bytes[kSignificantBytesSize - 1] = significant_from_lhs[i];
        uint239_t dividend;
        SetSignificantBits(dividend, dividend_bytes);

        uint8_t next_digit = GetFirstQuotientDigit(dividend, rhs);

        if (next_digit != 0) {
            result_bytes[i] = next_digit;
            dividend = dividend - (rhs * FromInt(next_digit, 0));
            ExtractSignificantBits(dividend, dividend_bytes);
            ++lhs_significant_digits_offset;
        }
    }

    SetSignificantBits(result, result_bytes);
    ShiftToLeft(result, GetShift(lhs) - GetShift(rhs));

    return result;
}

bool operator==(const uint239_t& lhs, const uint239_t& rhs) {
    uint8_t significant_from_lhs[kSignificantBytesSize];
    uint8_t significant_from_rhs[kSignificantBytesSize];
    ExtractSignificantBits(lhs, significant_from_lhs);
    ExtractSignificantBits(rhs, significant_from_rhs);

    for (size_t i = 0; i < kSignificantBytesSize; ++i) {
        if (significant_from_lhs[i] != significant_from_rhs[i]) {
            return false;
        }
    }

    return true;
}

bool operator!=(const uint239_t& lhs, const uint239_t& rhs) {
    return !(lhs == rhs);
}

bool operator>(const uint239_t& lhs, const uint239_t& rhs) {
    uint8_t significant_from_lhs[kSignificantBytesSize];
    uint8_t significant_from_rhs[kSignificantBytesSize];
    ExtractSignificantBits(lhs, significant_from_lhs);
    ExtractSignificantBits(rhs, significant_from_rhs);

    for (size_t i = 0; i < kSignificantBytesSize; ++i) {
        uint8_t byte_left = significant_from_lhs[i];
        uint8_t byte_right = significant_from_rhs[i];

        if (byte_left != byte_right) {
            return byte_left > byte_right;
        }
    }
    
    return false;
}

bool operator<(const uint239_t& lhs, const uint239_t& rhs) {
    uint8_t significant_from_lhs[kSignificantBytesSize];
    uint8_t significant_from_rhs[kSignificantBytesSize];
    ExtractSignificantBits(lhs, significant_from_lhs);
    ExtractSignificantBits(rhs, significant_from_rhs);

    for (size_t i = 0; i < kSignificantBytesSize; ++i) {
        uint8_t byte_left = significant_from_lhs[i];
        uint8_t byte_right = significant_from_rhs[i];

        if (byte_left != byte_right) {
            return byte_left < byte_right;
        }
    }
    
    return false;
}

bool operator<=(const uint239_t& lhs, const uint239_t& rhs) {
    return lhs == rhs || lhs < rhs;
}

bool operator>=(const uint239_t& lhs, const uint239_t& rhs) {
    return lhs == rhs || lhs > rhs;
}

std::ostream& operator<<(std::ostream& stream, const uint239_t& value) {
    uint8_t significant_bytes[kSignificantBytesSize];
    ExtractSignificantBits(value, significant_bytes);

    char decimal_result[kSignificantBytesSize * 8]{'0'};
    size_t decimal_length = 1;

    for (size_t i = 0; i < kSignificantBytesSize; ++i) {
        int32_t carry = 0;

        for (int32_t j = decimal_length - 1; j >= 0; --j) {
            int32_t current_digit = (decimal_result[j] - '0') * 256 + carry;
            carry = current_digit / 10;
            decimal_result[j] = (current_digit % 10) + '0';
        }

        while (carry > 0) {
            for (int32_t j = decimal_length; j > 0; --j) {
                decimal_result[j] = decimal_result[j - 1];
            }

            decimal_result[0] = (carry % 10) + '0';
            carry /= 10;
            decimal_length++;
        }

        carry = significant_bytes[i];

        for (int32_t j = decimal_length - 1; j >= 0; --j) {
            int32_t current_digit = (decimal_result[j] - '0') + carry;
            carry = current_digit / 10;
            decimal_result[j] = (current_digit % 10) + '0';
        }

        while (carry > 0) {
            for (int32_t j = decimal_length; j > 0; --j) {
                decimal_result[j] = decimal_result[j - 1];
            }

            decimal_result[0] = (carry % 10) + '0';
            carry /= 10;
            decimal_length++;
        }
    }

    decimal_result[decimal_length] = '\0';

    stream << decimal_result;
    return stream;
}

uint64_t GetShift(const uint239_t& value) {
    uint64_t shift{0};

    for (size_t i = 0; i < kNumberByteSize; ++i) {
        uint64_t current_shift_bit = value.data[i] >> 7;
        shift |= (current_shift_bit << (kNumberByteSize - i - 1));
    }

    return shift;
}

void SetShiftBits(uint239_t& number, uint64_t shift) {
    uint8_t set_to_byte = kNumberByteSize - 1;
    uint8_t shift_bits_extracted = 0;

    while (shift_bits_extracted < kNumberByteSize) {
        uint8_t current_shift_bit = (shift >> shift_bits_extracted) & 1;
        number.data[set_to_byte] &= 0b01111111;
        number.data[set_to_byte] |= current_shift_bit << 7;
        ++shift_bits_extracted;
        --set_to_byte;
    }
}

void ExtractSignificantBits(const uint239_t& number, uint8_t to[kSignificantBytesSize]) {
    std::fill(to, to + kSignificantBytesSize, 0);
    uint8_t number_shift = GetShift(number) % kRealMaxShift;

    uint8_t pos_to_write_bit = kSignificantBytesSize * 8 - 1;
    uint16_t pos_to_extract_bit = 8 * kNumberByteSize - number_shift - number_shift / 7 - 1;
    uint8_t bits_copied = 0;

    while (bits_copied < 8 * kSignificantBytesSize - 1) {
        if (pos_to_extract_bit == 0) {
            pos_to_extract_bit = 8 * kNumberByteSize - 1;
        }

        if (pos_to_extract_bit % 8 == 0) {
            --pos_to_extract_bit;
        }

        uint8_t byte_to_read = pos_to_extract_bit / 8;
        uint8_t bit_to_read = (pos_to_extract_bit % 8);

        uint8_t current_significant_bit = (number.data[byte_to_read] >> (7 - bit_to_read)) & 1;

        uint8_t write_to_byte = pos_to_write_bit / 8;
        uint8_t write_to_bit = pos_to_write_bit % 8;

        to[write_to_byte] |= (current_significant_bit << (7 - write_to_bit));

        --pos_to_write_bit;
        --pos_to_extract_bit;
        ++bits_copied;
    }
}

void ShiftToLeft(uint239_t& number, uint64_t shift) {
    ShiftSignificantBitsToLeft(number, shift);
    SetShiftBits(number, GetShift(number) + shift);
}

void ShiftSignificantBitsToLeft(uint239_t& number, uint64_t shift) {
    uint64_t prev_shift = GetShift(number);
    shift = (prev_shift + shift) % kRealMaxShift;

    uint8_t significant_bits[kSignificantBytesSize];
    ExtractSignificantBits(number, significant_bits);

    uint239_t result;

    size_t pos_to_write_bit = 8 * kNumberByteSize - shift - shift / 7 - 1;
    uint8_t bits_written = 0;

    while (bits_written < kSignificantBytesSize * 8 - 1) {
        if (pos_to_write_bit == 0) {
            pos_to_write_bit = 8 * kNumberByteSize - 1;
        }

        if (pos_to_write_bit % 8 == 0) {
            --pos_to_write_bit;
        }

        uint8_t write_to_byte = pos_to_write_bit / 8;
        uint8_t write_to_bit = pos_to_write_bit % 8;
        uint8_t read_from_bit = bits_written % 8;
        uint8_t read_from_byte = kSignificantBytesSize - (bits_written / 8) - 1;

        uint8_t current_significant_bit = (significant_bits[read_from_byte] >> read_from_bit) & 1;

        result.data[write_to_byte] |= (current_significant_bit << (7 - write_to_bit));
        
        ++bits_written;
        --pos_to_write_bit;
    }

    SetShiftBits(result, prev_shift);
    number = result;
}

void SetSignificantBits(uint239_t& number, uint8_t from[kSignificantBytesSize]) {
    uint64_t number_shift = GetShift(number) % kRealMaxShift;
    uint16_t pos_to_write_bit = 8 * kNumberByteSize - number_shift - number_shift / 7 - 1;
    uint16_t pos_to_extract_bit = 8 * kSignificantBytesSize - 1;
    uint16_t bits_written = 0;

    std::fill(number.data, number.data + kNumberByteSize, 0);

    while (bits_written < 8 * kSignificantBytesSize - 1) {
        uint8_t current_bit = (from[pos_to_extract_bit / 8] >> (7 - (pos_to_extract_bit % 8))) & 1;

        if (pos_to_write_bit == 0) {
            pos_to_write_bit = 8 * kNumberByteSize - 1;
        }

        if (pos_to_write_bit % 8 == 0) {
            --pos_to_write_bit;
        }

        number.data[pos_to_write_bit / 8] |= current_bit << (7 - (pos_to_write_bit % 8));

        ++bits_written;
        --pos_to_extract_bit;
        --pos_to_write_bit;
    }

    SetShiftBits(number, number_shift);
}
