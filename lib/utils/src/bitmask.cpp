#include <utils/bitmask.hpp>

using namespace utils;

Bitmask::Bitmask():m_bits(0){}

Bitmask::Bitmask(Bitset l_bits) :m_bits(l_bits) {}

auto Bitmask::getMask() const -> Bitset
{
    return m_bits;
}

void Bitmask::setMask(Bitset l_bits)
{
    m_bits = l_bits;
}

auto Bitmask::matches(const Bitmask& l_other, Bitset l_relevant) const -> bool
{
    if (l_relevant == 0)
    {
        return m_bits == l_other.m_bits;
    }

    return (m_bits & l_relevant) == (l_other.m_bits & l_relevant);
}

auto Bitmask::getBit(Position l_pos) const -> bool
{
    return (m_bits & (1 << l_pos)) != 0U;
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