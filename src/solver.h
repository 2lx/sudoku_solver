#ifndef SOLVER_H
#define SOLVER_H

#include "cell.h"

#include <iosfwd>
#include <array>

namespace Sudoku
{
    template <size_t SIZE>
    class Solver
    {
    public:
        Solver() = default;

        bool read(std::istream & stream);
        bool solve();
        void print(std::ostream & stream) const;

    private:
        static constexpr size_t NCOUNT = SIZE*SIZE;

        template <typename T>
        using array_t = std::array<T, NCOUNT*NCOUNT>;
        using narray_t = std::array<std::array<size_t, NCOUNT>, NCOUNT>;

        Solver(const Solver&) = delete;
        Solver& operator=(Solver &&) = delete;
        Solver& operator=(const Solver&) = delete;
        Solver(Solver &&) = delete;

        void updatePossibilities(size_t index);
        bool restrict();
        bool assumeNumber();
        bool isSolvable() const;
        bool isFilled() const;
        bool isCorrect() const;

        size_t col(size_t i) const { return i % NCOUNT; }
        size_t row(size_t i) const { return i / NCOUNT; }
        size_t box(size_t i) const { return (i / NCOUNT) / SIZE * SIZE + (i % NCOUNT) / SIZE; }

        // static
        static constexpr narray_t InitRowNeighbors();
        static constexpr narray_t InitColNeighbors();
        static constexpr narray_t InitBoxNeighbors();

        static constexpr narray_t m_rowNeighbors = InitRowNeighbors();
        static constexpr narray_t m_colNeighbors = InitColNeighbors();
        static constexpr narray_t m_boxNeighbors = InitBoxNeighbors();

    private:
        array_t<Cell<NCOUNT>> m_cells;
    };

    // explicit instantiation declaration
    extern template class Solver<3>;
}

#endif
