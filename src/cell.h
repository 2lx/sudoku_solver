#ifndef CELLDATA_H
#define CELLDATA_H

#include <iosfwd>
#include <bitset>
#include <cassert>
#include <cstdint>
#include <vector>

namespace Sudoku
{

template <size_t N>
class Cell
{

public:
    static constexpr uint8_t EMPTY = static_cast<uint8_t>(0);

    Cell();

    void fromChar(char ch);

    void setNumber(size_t number) { m_number = number; }
    size_t number() const { return static_cast<size_t>(m_number); }
    bool is_empty() const { return m_number == EMPTY; }

    void disable(size_t number);
    bool is_possible(size_t number) const { return m_possibilities[number - 1] == true; }
    bool is_onlyone() const { return m_possibilities.count() == 1; }

    bool is_inconsistent() const { return m_number == EMPTY && m_possibilities.count() == 0; }

    std::vector<size_t> possibilities() const;

private:
    size_t m_number;
    std::bitset<N> m_possibilities;
};

template <size_t N>
std::ostream & operator << (std::ostream & ostr, const Cell<N> & cell);

// explicit instantiation declaration
extern template class Cell<9>;
extern template std::ostream & operator <<(std::ostream &, const Cell<9> &);
}

#endif
