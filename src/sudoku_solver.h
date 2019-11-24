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

        bool read(std::istream & stream);
        bool solve();
        void print(std::ostream & stream) const;

    private:
        static constexpr uint32_t NCOUNT = SIZE*SIZE;

        template <typename T>
        using array_t = std::array<T, NCOUNT*NCOUNT>;
        using narray_t = std::array<std::array<uint32_t, NCOUNT>, NCOUNT>;

        Solver(const Solver&) = delete;
        Solver& operator=(Solver &&) = delete;
        Solver& operator=(const Solver&) = delete;
        Solver(Solver &&) = delete;

        void updatePossibilities(uint32_t index);
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
        struct CellData
        {
            uint8_t number;
            std::bitset<NCOUNT> possibilities;
        };

        array_t<CellData> m_data;
    };

    // explicit instantiation declaration
    extern template class Solver<3>;
}

#endif
