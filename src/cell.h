#ifndef CELL_H
#define CELL_H

#include <iosfwd>
#include <bitset>
#include <vector>

namespace Sudoku
{
static constexpr size_t EMPTY = 0;

template <size_t N>
class Cell
{
public:
    Cell();

    size_t number() const { return m_number; }
    void setNumber(size_t number) { m_number = number; }

    std::vector<size_t> possibilities() const;
    void disable(size_t number);

    bool isEmpty() const                 { return m_number == EMPTY; }
    bool isPossible(size_t number) const { return m_possibilities[number - 1] == true; }
    bool isOnlyOne() const               { return m_possibilities.count() == 1; }
    bool isInconsistent() const          { return m_number == EMPTY && m_possibilities.count() == 0; }


private:
    size_t m_number = 0;
    std::bitset<N> m_possibilities;
};

template <size_t N>
std::ostream & operator <<(std::ostream & ostr, const Cell<N> & cell);

template <size_t N>
std::istream & operator >>(std::istream & istr, Cell<N> & cell);

// explicit instantiation declaration
extern template class Cell<9>;
extern template std::ostream & operator <<(std::ostream &, const Cell<9> &);
extern template std::istream & operator >>(std::istream &, Cell<9> &);
}

#endif
