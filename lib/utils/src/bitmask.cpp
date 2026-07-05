#include <utils/bitmask.hpp>

Bitmask::Bitmask():m_bits(0){}

Bitmask::Bitmask(Bitset l_bits) :m_bits(l_bits) {}

Bitset Bitmask::getMask() const
{
    return m_bits;
}

void Bitmask::setMask(Bitset l_bits)
{
    m_bits = l_bits;
}

bool Bitmask::matches(const Bitmask& l_other, Bitset l_relevant) const
{
    if (l_relevant == 0)
    {
        return m_bits == l_other.m_bits;
    }

    return (m_bits & l_relevant) == (l_other.m_bits & l_relevant);
}

bool Bitmask::getBit(Position l_pos) const
{
    return (m_bits & (1 << l_pos)) != 0u;
}

void Bitmask::turnOnBit(Position l_pos)
{
    m_bits |= (1 << l_pos);
}

void Bitmask::turnOnBits(Bitset l_bits)
{
    m_bits |= l_bits;
}

void Bitmask::clearBit(Position l_pos)
{
    m_bits &= ~(1 << l_pos);
}

void Bitmask::toggleBit(Position l_pos)
{
    m_bits ^= (1 << l_pos);
}

void Bitmask::clear()
{
    m_bits = 0;
}