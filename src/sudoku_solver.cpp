#include "sudoku_solver.h"

#include <iostream>
#include <algorithm>
#include <set>
#include <cassert>
#include <iostream>

using namespace std;
using namespace Sudoku;

constexpr uchar EMPTY = static_cast<uchar>(0);
template <uint SIZE> const uint Solver<SIZE>::NCOUNT;
template <uint SIZE> const uint Solver<SIZE>::ICOUNT;

template <uint SIZE>
constexpr typename Solver<SIZE>::NeighborArray Solver<SIZE>::init_neighbors() {
    NeighborArray array{ 0 };

    constexpr auto add_unique = [](std::array<uint, ICOUNT> & ar, const auto num) {
        for (uint i = 0; i < ICOUNT && ar[i] != num; i++) {
            if (ar[i] == 0) { ar[i] = num; return; }
        }
    };

    for (uint i = 0; i < NCOUNT*NCOUNT; i++) {
        const uint x = i  % NCOUNT, y = i  / NCOUNT;
        const uint box_first = y / SIZE * SIZE * NCOUNT + x / SIZE * SIZE;

        for (uint j = 0; j < NCOUNT; j++) {
            add_unique(array[i], y * NCOUNT + j);
            add_unique(array[i], x + j * NCOUNT);
            add_unique(array[i], box_first + j/SIZE*NCOUNT + j%SIZE);
        }
    }

    return array;
}

template <uint SIZE>
bool Solver<SIZE>::read_level_data(std::istream & stream) {
    for (uint i = 0; i < NCOUNT*NCOUNT; i++) {
        processed[i] = false;
        possibilities[i].set();
    }

    string str;
    uint index = 0;
    for (uint i = 0; i < NCOUNT; i++) {
        if (!getline(stream, str))  { return false; }
        if (str.length() != NCOUNT) { return false; }

        for (const auto ch: str) {
            if (::isdigit(ch)) {
                numbers.at(index) = static_cast<uchar>(ch - '0');
            } else if (::isalpha(ch)) {
                numbers.at(index) = static_cast<uchar>(::toupper(ch) - 'A' + 10);
            } else {
                numbers.at(index) = EMPTY;
            }
            index++;
        }
    }

    init_neighbors();
    return true;
}

template <uint SIZE>
void Solver<SIZE>::print_solution(std::ostream & stream) const {
    uint index = 0;
    for (uint i = 0; i < NCOUNT; i++) {
        for (uint j = 0; j < NCOUNT; j++) {
            if (numbers[index] == EMPTY) { stream << ' '; }
            else if (numbers[index] < 10) {
                stream << static_cast<char>(numbers[index] + '0');
            } else {
                stream << static_cast<char>(numbers[index] + 'A' - 10);
            }

            if (j % SIZE == SIZE - 1 && j != NCOUNT - 1) { stream << '|'; }
            index++;
        }
        stream << '\n';
        if (i % SIZE == SIZE - 1 && i != NCOUNT - 1) {
            for (uint k = 0; k < SIZE; k++) {
                stream << string(SIZE, '-');
                if (k != SIZE - 1) { stream << '+'; }
            }
            stream << "\n";
        }
    }
}

template <uint SIZE>
void Solver<SIZE>::restrict_possibilities() {
    for (uint i = 0; i < NCOUNT*NCOUNT; i++) {
        if (numbers[i] == EMPTY || processed[i]) { continue; }

        for (const auto nbi: neighbors[i]) {
            possibilities[nbi].reset(numbers[i] - 1);
        }
        processed[i] = true;
    }
}

template <uint SIZE>
tuple<bool, bool, bool> Solver<SIZE>::write_down_sole_possibilities() {
    bool was_replenished = false;
    bool is_complete = true;

    for (uint i = 0; i < NCOUNT*NCOUNT; i++) {
        if (numbers[i] != EMPTY) { continue; }

        switch(possibilities[i].count()) {
            case 0: return {false, false, false};
            case 1: {
                // find the only possible number
                numbers[i] = static_cast<uchar>(possibilities[i]._Find_first() + 1u);
                was_replenished = true;
                break;
            }
            default:
                is_complete = false;
        }
    }
    return {true, is_complete, was_replenished};
}

// We did not add any numbers during the last iteration, so we have to assume
template <uint SIZE>
bool Solver<SIZE>::assume_number() {
    uint index = 0;
    while (numbers[index] != EMPTY) { index++; }

    auto poss_index = possibilities[index]._Find_first();
    while (poss_index != NCOUNT) {
        auto backup = *this;

        /* cout << "assumption(" << index << ") number = " << poss_index + 1 << endl; */
        numbers[index] = static_cast<uchar>(poss_index + 1u);
        if (solve()) { return true; }

        /* cout << "wrong assumption(" << index << ")" << endl; */
        *this = move(backup);
        poss_index = possibilities[index]._Find_next(poss_index);
    }
    return false;
}

template <uint SIZE>
bool Solver<SIZE>::solve() {
    while (true) {
        // for every not empty cell update possibilities of its neighbors
        restrict_possibilities();

        // for every empty puzzle cell check if there is only one possible number
        const auto [has_result, is_complete, was_replenished] = write_down_sole_possibilities();
        if (!has_result || is_complete) { return has_result; }

        // if numbers was not replenished make an assumption
        if (!was_replenished) { return assume_number(); }
    }
}

// check if the puzzle was completely solved
template <uint SIZE>
bool Solver<SIZE>::solution_is_complete() const {
    for (const auto num: numbers) {
        if (num == EMPTY) { return false; }
    }

    auto is_unique = [](auto & r) {
        sort(begin(r), end(r));
        return unique(begin(r), end(r)) == end(r);
    };

    for (uint i = 0; i < NCOUNT; i++) {
        const uint box_first = i%SIZE*SIZE*NCOUNT + i/SIZE*SIZE;
        array<uint, NCOUNT> vrow, vcol, vbox;

        for (uint j = 0; j < NCOUNT; j++) {
            vrow[j] = i * NCOUNT + j;
            vcol[j] = j * NCOUNT + i;
            vbox[j] = box_first + j/SIZE*NCOUNT + j%SIZE;
        }

        if (!is_unique(vrow) || !is_unique(vcol) || !is_unique(vbox)) { return false; }
    }
    return true;
}

// explicit instantiation definition
template class Sudoku::Solver<3>;
