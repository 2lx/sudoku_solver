#include "cell.h"

#include <iostream>

using namespace std;

namespace Sudoku
{
template <size_t N>
Cell<N>::Cell()
: m_number(0)
{
    m_possibilities.set();
}

template <size_t N>
void Cell<N>::fromChar(char ch)
{
    if (::isdigit(ch))
        m_number = static_cast<uint8_t>(ch - '0');
    else if (::isalpha(ch))
        m_number = static_cast<uint8_t>(::toupper(ch) - 'A' + 10);
    else
        m_number = EMPTY;
}

template <size_t N>
void Cell<N>::disable(size_t number)
{
    assert(number > 0);
    m_possibilities.reset(number - 1);
}

template <size_t N>
vector<size_t> Cell<N>::possibilities() const
{
    vector<size_t> result;

    auto poss_index = m_possibilities._Find_first();
    while (poss_index != N)
    {
        result.push_back(poss_index + 1);
        poss_index = m_possibilities._Find_next(poss_index);
    }

    return result;
}

template <size_t N>
ostream & operator <<(ostream & ostr, const Cell<N> & cell)
{
    const auto & number = cell.number();

    if (number == Cell<N>::EMPTY)
        return ostr << ' ';

    if (number < 10)
        return ostr << static_cast<char>(number + '0');

    return ostr << static_cast<char>(number + 'A' - 10);
}

// explicit instantiation definition
template class Cell<9>;
template ostream & operator <<(ostream &, const Cell<9> &);
}

