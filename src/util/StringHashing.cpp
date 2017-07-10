#include "StringHashing.hpp"

// Runtime string hashing
unsigned int stringCrc32(const char* str, unsigned int prev_crc)
{
    // Base case
    if (!str || !*str)
        return prev_crc ^ 0xFFFFFFFF;

    // Do not change this function without also changing ConstCrc::crc32()
    return stringCrc32(str + 1, (prev_crc >> 8) ^ crc_table[(prev_crc ^ *str) & 0xFF]);
}