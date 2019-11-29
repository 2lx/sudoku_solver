#ifndef SOLVER_H
#define SOLVER_H

#include "cell.h"

#include <iosfwd>
#include <array>

namespace Sudoku
{
enum NbType { ROW, COL, BOX };

template <size_t SIZE>
class Solver
{
public:
    Solver() = default;

    bool read(std::istream & stream);
    bool solve();
    void print(std::ostream & stream) const;
    bool isCorrect() const;

private:
    static constexpr size_t NCOUNT = SIZE*SIZE;
    using narray_t = std::array<std::array<size_t, NCOUNT>, NCOUNT>;

    Solver(const Solver &) = delete;
    Solver & operator=(Solver &&) = delete;
    Solver & operator=(const Solver &) = delete;
    Solver(Solver &&) = delete;

private:
    void updatePossibilities(size_t index);
    bool restrict();
    bool assumeNumber();
    bool isSolvable() const;
    bool isFilled() const;

    size_t col(size_t i) const { return i % NCOUNT; }
    size_t row(size_t i) const { return i / NCOUNT; }
    size_t box(size_t i) const { return (i / NCOUNT) / SIZE * SIZE + (i % NCOUNT) / SIZE; }

    template <NbType TYPE>
    static constexpr narray_t initNeighbors();

private:
    std::array<Cell<NCOUNT>, NCOUNT * NCOUNT> m_cells;

    static constexpr narray_t m_neighbors[] =
        { initNeighbors<ROW>(), initNeighbors<COL>(), initNeighbors<BOX>() };
};

// explicit instantiation declaration
extern template class Solver<3>;
extern template class Solver<4>;
}

#endif
