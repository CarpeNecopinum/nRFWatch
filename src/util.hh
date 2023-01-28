inline int8_t millis_compare(uint32_t a, uint32_t b)
{
    constexpr uint32_t HALF = 0x80000000;

    // "estimate" of a > b, based on the knowledge that
    // an overflow may have occurred between them
    if (a == b)
        return 0;

    return (a - b < HALF) ? 1 : -1;
}

inline uint32_t millis_max(uint32_t a, uint32_t b)
{
    return millis_compare(a, b) > 0 ? a : b;
}

inline uint32_t millis_min(uint32_t a, uint32_t b)
{
    return millis_compare(a, b) < 0 ? a : b;
}