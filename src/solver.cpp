#include "solver.h"

#include <iostream>
#include <algorithm>
#include <set>

using namespace std;
using namespace Sudoku;

template <size_t SIZE>
constexpr typename Solver<SIZE>::narray_t Solver<SIZE>::InitRowNeighbors()
{
    narray_t result{ 0 };

    for (auto row = 0u; row < NCOUNT; ++row)
        for (auto ind = 0u; ind < NCOUNT; ++ind)
            result[row][ind] = row * NCOUNT + ind;

    return result;
}

template <size_t SIZE>
constexpr typename Solver<SIZE>::narray_t Solver<SIZE>::InitColNeighbors()
{
    narray_t result{ 0 };

    for (auto col = 0u; col < NCOUNT; ++col)
        for (auto ind = 0u; ind < NCOUNT; ++ind)
            result[col][ind] = col + ind * NCOUNT;

    return result;
}

template <size_t SIZE>
constexpr typename Solver<SIZE>::narray_t Solver<SIZE>::InitBoxNeighbors()
{
    narray_t result{ 0 };

    for (auto box = 0u; box < NCOUNT; ++box)
    {
        const size_t boxFirst = box / SIZE * SIZE * NCOUNT + box % SIZE * SIZE;

        for (auto ind = 0u; ind < NCOUNT; ++ind)
            result[box][ind] = boxFirst + ind / SIZE * NCOUNT + ind % SIZE;
    }

    return result;
}

template <size_t SIZE>
bool Solver<SIZE>::read(std::istream & stream)
{
    for (auto & cell: m_cells)
        if (!(stream >> cell))
            return false;

    for (size_t i = 0; i < NCOUNT * NCOUNT; ++i)
        if (!m_cells[i].isEmpty())
            updatePossibilities(i);

    return true;
}

template <size_t SIZE>
void Solver<SIZE>::print(std::ostream & stream) const
{
    if (!isFilled() || !isCorrect())
        cout << "Current state is not a valid solution" << endl;

    size_t index = 0;
    for (size_t i = 0; i < NCOUNT; ++i)
    {
        for (size_t j = 0; j < NCOUNT; ++j)
        {
            stream << m_cells[index];
            if (j % SIZE == SIZE - 1 && j != NCOUNT - 1)
                stream << '|';

            index++;
        }

        stream << '\n';
        if (i % SIZE == SIZE - 1 && i != NCOUNT - 1)
        {
            for (size_t k = 0; k < SIZE; ++k)
            {
                stream << string(SIZE, '-');
                if (k != SIZE - 1)
                    stream << '+';
            }

            stream << "\n";
        }
    }
}

template <size_t SIZE>
void Solver<SIZE>::updatePossibilities(size_t index)
{
    const size_t number = m_cells[index].number();

    for (const auto nbi: m_rowNeighbors[row(index)])
        m_cells[nbi].disable(number);

    for (const auto nbi: m_colNeighbors[col(index)])
        m_cells[nbi].disable(number);

    for (const auto nbi: m_boxNeighbors[box(index)])
        m_cells[nbi].disable(number);
}

template <size_t SIZE>
bool Solver<SIZE>::restrict()
{
    auto fn_check = [this](size_t number, size_t ind, const auto & nbs)
    {
        return none_of(nbs.cbegin(), nbs.cend(), [&](auto nbi)
        {
            const auto & cell = m_cells[nbi];
            return nbi != ind && cell.isEmpty() && cell.isPossible(number);
        });
    };

    bool result = false;
    for (size_t i = 0; i < NCOUNT * NCOUNT; ++i)
    {
        auto & cell = m_cells[i];
        if (!cell.isEmpty())
            continue;

        const auto numbers = cell.possibilities();
        for (const auto & number: numbers)
        {
            if (cell.isOnlyOne()
                || fn_check(number, i, m_rowNeighbors[row(i)])
                || fn_check(number, i, m_colNeighbors[col(i)])
                || fn_check(number, i, m_boxNeighbors[box(i)]))
            {
                cell.setNumber(number);
                updatePossibilities(i);
                result = true;
                break;
            }
        }
    }

    return result;
}

// We did not add any m_numbers during the last iteration, so we have to assume
template <size_t SIZE>
bool Solver<SIZE>::assumeNumber()
{
    const auto it = find_if(m_cells.begin(), m_cells.end(),
            [](const auto & cell) { return cell.isEmpty(); });

    const size_t index = distance(m_cells.begin(), it);
    const auto poss = it->possibilities();

    for (const auto & poss_number: poss)
    {
        const auto backup = m_cells;

        print(cout);
        cout << "assume [" << col(index) << "," << row(index)
             << "]=" << poss_number << endl;

        it->setNumber(poss_number);
        updatePossibilities(index);

        if (solve())
            return true;

        cout << "wrong assumption [" << col(index) << "," << row(index)
             << "]=" << poss_number << endl;
        m_cells = move(backup);
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

        if (!restrict())
            return assumeNumber();
    }
}

template <size_t SIZE>
bool Solver<SIZE>::isFilled() const
{
    return none_of(m_cells.cbegin(), m_cells.cend(),
            [](const auto & cell) { return cell.isEmpty(); });
}

template <size_t SIZE>
bool Solver<SIZE>::isSolvable() const
{
    return none_of(m_cells.cbegin(), m_cells.cend(),
            [](const auto & cell) { return cell.isInconsistent(); });
}

template <size_t SIZE>
bool Solver<SIZE>::isCorrect() const
{
    auto testUnique = [this](const auto& neighbors) {
        set<size_t> nums;

        for (auto nbi: neighbors)
            if (!nums.insert(m_cells[nbi].number()).second)
                return false;

        return true;
    };

    for (size_t i = 0; i < NCOUNT; ++i)
    {
        if (!testUnique(m_rowNeighbors[i])
         || !testUnique(m_colNeighbors[i])
         || !testUnique(m_boxNeighbors[i]))
            return false;
    }

    return true;
}

// explicit instantiation definition
template class Sudoku::Solver<3>;
