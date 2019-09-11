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
        void PrintSolution(std::ostream & stream) const;

    private:
        // types
        static constexpr uint32_t NCOUNT = SIZE*SIZE;
        static constexpr uint32_t ICOUNT = 3*SIZE*SIZE - 2*SIZE;

        template <typename T>
        using NNArray = std::array<T, NCOUNT*NCOUNT>;
        using NeighborArray = NNArray<std::array<uint32_t, ICOUNT>>;

        // methods
        Solver(const Solver&) = default;
        Solver& operator=(Solver &&) = default;

        Solver& operator=(const Solver&) = delete;
        Solver(Solver &&) = delete;

        void RestrictPossibilities();
        std::tuple<bool, bool, bool> WriteDownSolePossibilities();
        bool AssumeNumber();
        bool SolutionIsComplete() const;

        static constexpr NeighborArray InitNeighbors();
        static constexpr NeighborArray neighbors = InitNeighbors();

        // data members
        NNArray<uint8_t> m_numbers;
        NNArray<std::bitset<NCOUNT>> m_possibilities;
        NNArray<bool> m_processed;
    };

    // explicit instantiation declaration
    extern template class Solver<3>;
}

#endif
