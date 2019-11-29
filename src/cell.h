#ifndef CELL_H
#define CELL_H

#include <iostream>
#include <bitset>
#include <vector>

namespace Sudoku
{
static constexpr size_t EMPTY = 0;

template <size_t N>
class Cell
{
public:
    size_t number() const                { return m_number; }
    void setNumber(size_t number)        { m_number = number; }
    void disable(size_t number)          { m_possibilities.reset(number - 1); }

    bool isEmpty() const                 { return m_number == EMPTY; }
    bool isPossible(size_t number) const { return m_possibilities[number - 1] == true; }
    bool isOnlyOne() const               { return m_possibilities.count() == 1; }
    bool isInconsistent() const          { return m_number == EMPTY && m_possibilities.count() == 0; }

    std::vector<size_t> possibilities() const
    {
        std::vector<size_t> result;

        for (size_t index = 0; index < N; ++index)
            if (m_possibilities[index])
                result.push_back(index + 1);

        return result;
    }

private:
    size_t m_number = 0;
    std::bitset<N> m_possibilities = std::bitset<N>().set();
};

template <size_t N>
std::ostream & operator <<(std::ostream & ostr, const Cell<N> & cell)
{
    if (cell.isEmpty())
        return ostr << ' ';

    const auto & number = cell.number();
    if (number < 10)
        return ostr << static_cast<char>(number + '0');

    return ostr << static_cast<char>(number + 'A' - 10);
}

template <size_t N>
std::istream & operator >>(std::istream & istr, Cell<N> & cell)
{
    char ch;
    istr >> ch;

    if (::isdigit(ch))
        cell.setNumber(static_cast<size_t>(ch - '0'));
    else if (::isalpha(ch))
        cell.setNumber(static_cast<size_t>(::toupper(ch) - 'A' + 10));
    else
        cell.setNumber(EMPTY);

    return istr;
}
}

#endif
