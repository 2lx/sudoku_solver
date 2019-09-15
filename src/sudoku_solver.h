#ifndef SUDOKU_SOLVER_H
#define SUDOKU_SOLVER_H

#include <iosfwd>
#include <tuple>
#include <array>
#include <bitset>
#include <cstdint>

namespace Sudoku
{
    template <uint32_t SIZE>
    class Solver
    {
    public:
        Solver() = default;

        bool ReadLevelData(std::istream & stream);
        bool Solve();
        void Print(std::ostream & stream) const;

    private:
        // types
        static constexpr uint32_t NCOUNT = SIZE*SIZE;

        template <typename T>
        using array_t = std::array<T, NCOUNT*NCOUNT>;
        using narray_t = std::array<std::array<uint32_t, NCOUNT>, NCOUNT>;

        // methods
        Solver(const Solver&) = default;
        Solver& operator=(Solver &&) = default;

        Solver& operator=(const Solver&) = delete;
        Solver(Solver &&) = delete;

        void RestrictPossibilities();
        std::tuple<bool, bool, bool> WriteDownSolePossibilities();
        bool AssumeNumber();
        bool IsComplete() const;

        int col(int i) const { return i % NCOUNT; }
        int row(int i) const { return i / NCOUNT; }
        int box(int i) const { return (i / NCOUNT) / SIZE * SIZE + (i % NCOUNT) / SIZE; }

        // static
        static constexpr narray_t InitRowNeighbors();
        static constexpr narray_t InitColNeighbors();
        static constexpr narray_t InitBoxNeighbors();

        static constexpr narray_t m_rowNeighbors = InitRowNeighbors();
        static constexpr narray_t m_colNeighbors = InitColNeighbors();
        static constexpr narray_t m_boxNeighbors = InitBoxNeighbors();

        // data members
        array_t<uint8_t> m_numbers;
        array_t<std::bitset<NCOUNT>> m_possibilities;
        array_t<bool> m_processed;
    };

    // explicit instantiation declaration
    extern template class Solver<3>;
}

#endif
