#ifndef SOLVER_H
#define SOLVER_H

#include "cell.h"
#include "string_join.h"

#include <iostream>
#include <array>
#include <string>
#include <algorithm>
#include <numeric>
#include <unordered_set>
#include <functional>

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
    static constexpr size_t NCOUNT = SIZE * SIZE;
    using narray_t = std::array<std::array<size_t, NCOUNT>, NCOUNT>;

    Solver(const Solver &) = delete;
    Solver & operator=(Solver &&) = delete;
    Solver & operator=(const Solver &) = delete;
    Solver(Solver &&) = delete;

private:
    void updatePossibilities(const Cell<NCOUNT> & cell);
    bool narrow();
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

template <size_t SIZE>
template <NbType TYPE>
constexpr typename Solver<SIZE>::narray_t Solver<SIZE>::initNeighbors()
{
    narray_t result{ 0 };

    for (size_t i1 = 0; i1 < NCOUNT; ++i1)
    {
        const size_t boxFirst = i1 / SIZE * SIZE * NCOUNT + i1 % SIZE * SIZE;

        for (size_t i2 = 0; i2 < NCOUNT; ++i2)
            if constexpr (TYPE == ROW)
                result[i1][i2] = i1 * NCOUNT + i2;
            else if constexpr (TYPE == COL)
                result[i1][i2] = i2 * NCOUNT + i1;
            else
                result[i1][i2] = boxFirst + i2 / SIZE * NCOUNT + i2 % SIZE;
    }

    return result;
}

template <size_t SIZE>
bool Solver<SIZE>::read(std::istream & stream)
{
    for (auto & cell: m_cells)
    {
        if (!(stream >> cell))
            return false;

        if (!cell.isEmpty())
            updatePossibilities(cell);
    }

    return true;
}

template <size_t SIZE>
void Solver<SIZE>::print(std::ostream & stream) const
{
    static const std::string rowdelim =
        '+' + string_join(std::vector(SIZE, std::string(SIZE, '-')), "+") + "+\n";

    std::ostringstream ss;
    std::array<std::string, SIZE> strings, rows, blocks;
    size_t index = 0;

    for (auto & block: blocks)
    {
        for (auto & row: rows)
        {
            for (auto & str: strings)
            {
                ss.seekp(0);
                for (size_t i = 0; i < SIZE; ++i)
                    ss << m_cells[index++];

                str = ss.str();
            }
            row = '|' + string_join(strings, "|") + '|';
        }
        block = string_join(rows, "\n") + '\n';
    }

    stream << rowdelim + string_join(blocks, rowdelim.c_str()) + rowdelim;
}

template <size_t SIZE>
void Solver<SIZE>::updatePossibilities(const Cell<NCOUNT> & cell)
{
    for (const auto nbi: m_neighbors[ROW][row(cell.index())])
        m_cells[nbi].disable(cell.number());

    for (const auto nbi: m_neighbors[COL][col(cell.index())])
        m_cells[nbi].disable(cell.number());

    for (const auto nbi: m_neighbors[BOX][box(cell.index())])
        m_cells[nbi].disable(cell.number());
}

template <size_t SIZE>
bool Solver<SIZE>::narrow()
{
    auto fn_check = [this](size_t number, size_t ind, const auto & nbs)
    {
        return std::none_of(nbs.cbegin(), nbs.cend(), [&](auto nbi)
        {
            const auto & cell = m_cells[nbi];
            return nbi != ind && cell.isEmpty() && cell.isPossible(number);
        });
    };

    bool narrowed = false;
    for (auto & cell: m_cells)
    {
        if (!cell.isEmpty())
            continue;

        const auto numbers = cell.possibilities();
        for (const auto & number: numbers)
        {
            if (cell.isOnlyOne()
                || fn_check(number, cell.index(), m_neighbors[ROW][row(cell.index())])
                || fn_check(number, cell.index(), m_neighbors[COL][col(cell.index())])
                || fn_check(number, cell.index(), m_neighbors[BOX][box(cell.index())]))
            {
                cell.setNumber(number);
                updatePossibilities(cell);
                narrowed = true;
                break;
            }
        }
    }

    return narrowed;
}

template <size_t SIZE>
bool Solver<SIZE>::assumeNumber()
{
    auto & cell = *std::find_if(m_cells.begin(), m_cells.end(),
                                bind(&Cell<NCOUNT>::isEmpty, std::placeholders::_1));

    for (const auto & number: cell.possibilities())
    {
        auto backup{ m_cells };

        print(std::cout);
        std::cout << "assume [" << col(cell.index()) << "," << row(cell.index())
                  << "]=" << number << std::endl;

        cell.setNumber(number);
        updatePossibilities(cell);

        if (solve())
            return true;

        std::cout << "wrong assumption" << std::endl;
        m_cells = std::move(backup);
    }

    return false;
}

template <size_t SIZE>
bool Solver<SIZE>::solve()
{
    while (true)
    {
        const bool isSolvable = this->isSolvable();
        if (isFilled() || !isSolvable)
            return isSolvable;

        if (!narrow())
            return assumeNumber();
    }
}

template <size_t SIZE>
bool Solver<SIZE>::isFilled() const
{
    return std::none_of(m_cells.cbegin(), m_cells.cend(),
            [](const auto & cell) { return cell.isEmpty(); });
}

template <size_t SIZE>
bool Solver<SIZE>::isSolvable() const
{
    return std::none_of(m_cells.cbegin(), m_cells.cend(),
            [](const auto & cell) { return cell.isInconsistent(); });
}

template <size_t SIZE>
bool Solver<SIZE>::isCorrect() const
{
    auto fn_testUnique = [this](const auto & nbs) {
        std::unordered_set<size_t> numbers;

        return std::all_of(nbs.cbegin(), nbs.cend(), [&](const auto nbi)
            { return numbers.insert(m_cells[nbi].number()).second; });
    };

    std::array<size_t, NCOUNT> indexes;
    std::iota(indexes.begin(), indexes.end(), 0);

    return std::all_of(indexes.begin(), indexes.end(), [&fn_testUnique](size_t index)
    {
        return fn_testUnique(m_neighbors[ROW][index])
            && fn_testUnique(m_neighbors[COL][index])
            && fn_testUnique(m_neighbors[BOX][index]);
    });
}

}

#endif
