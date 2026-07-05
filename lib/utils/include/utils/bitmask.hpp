#ifndef UTILS_BITMASK_HPP
#define UTILS_BITMASK_HPP

#include <cstdint>

namespace utils
{
using Bitset = uint32_t;

class Bitmask
{
  public:
    using Position = uint8_t;

    Bitmask();
    Bitmask(Bitset l_bits);

    [[nodiscard]] auto getMask() const -> Bitset;
    void               setMask(Bitset l_bits);

    [[nodiscard]] auto matches(const Bitmask& l_other, Bitset l_relevant = 0) const -> bool;
    [[nodiscard]] auto getBit(Position l_pos) const -> bool;

    void turnOnBit(Position l_pos);
    void turnOnBits(Bitset l_bits);
    void clearBit(Position l_pos);
    void toggleBit(Position l_pos);
    void clear();

  private:
    Bitset m_bits;
};
} // namespace utils

#endif