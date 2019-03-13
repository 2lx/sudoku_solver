#ifndef SUDOKU_SOLVER_H
#define SUDOKU_SOLVER_H

#include <iosfwd>
#include <tuple>
#include <array>
#include <bitset>

namespace Sudoku
{
using uchar = unsigned char;
using uint  = unsigned int;

template <uint SIZE>
class Solver {
private:
    static constexpr uint NCOUNT = SIZE*SIZE;
    static constexpr uint ICOUNT = 3*SIZE*SIZE - 2*SIZE;

    std::array<uchar, NCOUNT*NCOUNT> numbers;
    std::array<std::bitset<NCOUNT>, NCOUNT*NCOUNT> possibilities;
    std::array<std::array<uint, ICOUNT>, NCOUNT*NCOUNT> neighbors;
    std::array<bool, NCOUNT*NCOUNT> processed;

    Solver(const Solver &) = default;
    Solver & operator=(const Solver &) = default;

    void init_neighbor_indexes();
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
}

#endif