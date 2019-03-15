#ifndef SUDOKU_SOLVER_H
#define SUDOKU_SOLVER_H

#include <iosfwd>
#include <tuple>
#include <array>
#include <bitset>
#include <set>
#include <cassert>

namespace Sudoku
{
using uchar = unsigned char;
using uint  = unsigned int;

template <uint SIZE>
class Solver {
private:
    static constexpr uint NCOUNT = SIZE*SIZE;
    static constexpr uint ICOUNT = 3*SIZE*SIZE - 2*SIZE;

    template <typename T>
    using NNArray = std::array<T, NCOUNT*NCOUNT>;
    using NeighborArray = NNArray<std::array<uint, ICOUNT>>;

    static constexpr NeighborArray init_neighbors();
    static constexpr NeighborArray neighbors = init_neighbors();

    NNArray<uchar> numbers;
    NNArray<std::bitset<NCOUNT>> possibilities;
    NNArray<bool> processed;

    Solver(const Solver &) = default;
    Solver(Solver &&) = delete;
    Solver & operator=(const Solver &) = delete;
    Solver & operator=(Solver &&) = default;

    void restrict_possibilities();
    std::tuple<bool, bool, bool> write_down_sole_possibilities();
    bool assume_number();

public:
    Solver() = default;

    bool read_level_data(std::istream & stream);
    bool solve();
    void print_solution(std::ostream & stream) const;
    bool solution_is_complete() const;
};

// explicit instantiation declaration
extern template class Solver<3>;
}

#endif
