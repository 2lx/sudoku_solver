#include "cell.h"

#include <iostream>
#include <cassert>

using namespace std;

namespace Sudoku
{

template <size_t N>
Cell<N>::Cell()
{
    m_possibilities.set();
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

    for (size_t index = 0; index < N; ++index)
        if (m_possibilities[index])
            result.push_back(index + 1);

    return result;
}

template <size_t N>
ostream & operator <<(ostream & ostr, const Cell<N> & cell)
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

    return cin;
}

// explicit instantiation definition
template class Cell<9>;
template ostream & operator <<(ostream &, const Cell<9> &);
template istream & operator >>(istream &, Cell<9> &);
}

