//
// Created by lixinzhuang1 on 2024/6/28.
//

#ifndef BE_CONSTEXPR_H
#define BE_CONSTEXPR_H

namespace starrocks {
    // For HyperLogLog type
    constexpr int HLL_COLUMN_PRECISION = 14;
    constexpr int HLL_ZERO_COUNT_BITS = (64 - HLL_COLUMN_PRECISION);
    constexpr int HLL_EXPLICIT_INT64_NUM = 160;
    constexpr int HLL_SPARSE_THRESHOLD = 4096;
    constexpr int HLL_REGISTERS_COUNT = 16 * 1024;

    // maximum size in byte of serialized HLL: type(1) + registers (2^14)
    constexpr int HLL_COLUMN_DEFAULT_LEN = HLL_REGISTERS_COUNT + 1;

    // 1 for type; 1 for hash value count; 8 for hash value
    constexpr int HLL_EMPTY_SIZE = 1;

    // For JSON type
    constexpr int kJsonDefaultSize = 128;
    constexpr int kJsonMetaDefaultFormatVersion = 1;

    constexpr __int128 MAX_INT128 = ~((__int128)0x01 << 127);
    constexpr __int128 MIN_INT128 = ((__int128)0x01 << 127);

}

#endif//BE_CONSTEXPR_H
