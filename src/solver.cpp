#include "solver.h"
#include "string_join.h"

#include <iostream>
#include <algorithm>
#include <unordered_set>
#include <functional>

using namespace std;
using namespace Sudoku;

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
    static const string rowdelim =
        '+' + string_join(vector(SIZE, string(SIZE, '-')), "+") + "+\n";

    ostringstream ss;
    array<string, SIZE> strings, rows, blocks;
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
void Solver<SIZE>::updatePossibilities(size_t index)
{
    const size_t number = m_cells[index].number();

    for (const auto nbi: m_neighbors[ROW][row(index)])
        m_cells[nbi].disable(number);

    for (const auto nbi: m_neighbors[COL][col(index)])
        m_cells[nbi].disable(number);

    for (const auto nbi: m_neighbors[BOX][box(index)])
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
                || fn_check(number, i, m_neighbors[ROW][row(i)])
                || fn_check(number, i, m_neighbors[COL][col(i)])
                || fn_check(number, i, m_neighbors[BOX][box(i)]))
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

template <size_t SIZE>
bool Solver<SIZE>::assumeNumber()
{
    using namespace placeholders;
    const auto it = find_if(m_cells.begin(), m_cells.end(), bind(&Cell<NCOUNT>::isEmpty, _1));

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

        cout << "wrong assumption" << endl;
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
    auto testUnique = [this](const auto & nbs) {
        unordered_set<size_t> numbers;

        return all_of(nbs.cbegin(), nbs.cend(), [&](const auto nbi)
            { return numbers.insert(m_cells[nbi].number()).second; });
    };

    for (size_t i = 0; i < NCOUNT; ++i)
    {
        if (!testUnique(m_neighbors[ROW][i])
         || !testUnique(m_neighbors[COL][i])
         || !testUnique(m_neighbors[BOX][i]))
            return false;
    }

    return true;
}

// explicit instantiation definition
template class Sudoku::Solver<3>;
template class Sudoku::Solver<4>;
