#ifndef BITMASK_HPP
#define BITMASK_HPP

#include <stdint.h>

using Bitset = uint32_t;

class Bitmask
{
private:
  using Position = uint8_t;

public:
    Bitmask();
    Bitmask(Bitset l_bits);

    Bitset getMask() const;
    void setMask(Bitset l_bits);

    bool matches(const Bitmask& l_other, Bitset l_relevant = 0) const;
    bool getBit(Position l_pos) const;
    
    void turnOnBit(Position l_pos);
    void turnOnBits(Bitset l_bits);
    void clearBit(Position l_pos);
    void toggleBit(Position l_pos);
    void clear();
private:
    Bitset m_bits;

};

#endif