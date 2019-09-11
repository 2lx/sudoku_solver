#include "sudoku_solver.h"

#include <iostream>
#include <algorithm>
#include <set>

using namespace std;
using namespace Sudoku;

constexpr uint8_t EMPTY = static_cast<uint8_t>(0);

template <uint32_t SIZE>
constexpr typename Solver<SIZE>::NeighborArray Solver<SIZE>::InitNeighbors()
{
    NeighborArray array{ 0 };

    constexpr auto uniqueAdd = [](auto& ar, auto num) {
        uint32_t index = 0;
        while (index < ICOUNT && ar[index] != num && ar[index] != 0)
            ++index;

        ar[index] = num;
    };

    for (uint32_t i = 0; i < NCOUNT*NCOUNT; ++i)
    {
        const uint32_t x = i % NCOUNT;
        const uint32_t y = i / NCOUNT;
        const uint32_t boxFirst = y / SIZE * SIZE * NCOUNT + x / SIZE * SIZE;

        for (uint32_t j = 0; j < NCOUNT; ++j)
        {
            uniqueAdd(array[i], y * NCOUNT + j);
            uniqueAdd(array[i], x + j * NCOUNT);
            uniqueAdd(array[i], boxFirst + j / SIZE * NCOUNT + j % SIZE);
        }
    }

    return array;
}

template <uint32_t SIZE>
bool Solver<SIZE>::ReadLevelData(std::istream & stream)
{
    for (uint32_t i = 0; i < NCOUNT*NCOUNT; ++i)
    {
        m_processed[i] = false;
        m_possibilities[i].set();
    }

    string str;
    uint32_t index = 0;
    for (uint32_t i = 0; i < NCOUNT; ++i)
    {
        if (!getline(stream, str) || str.length() != NCOUNT)
        {
            cout << "Invalid input data" << endl;
            return false;
        }

        for (const auto ch: str)
        {
            if (::isdigit(ch))
                m_numbers.at(index) = static_cast<uint8_t>(ch - '0');

            else if (::isalpha(ch))
                m_numbers.at(index) = static_cast<uint8_t>(::toupper(ch) - 'A' + 10);

            else
                m_numbers.at(index) = EMPTY;

            index++;
        }
    }

    InitNeighbors();
    return true;
}

template <uint32_t SIZE>
void Solver<SIZE>::PrintSolution(std::ostream& stream) const
{
    if (!SolutionIsComplete())
    {
        cout << "Algorithm error occured" << endl;
        return;
    }

    uint32_t index = 0;
    for (uint32_t i = 0; i < NCOUNT; ++i)
    {
        for (uint32_t j = 0; j < NCOUNT; ++j)
        {
            if (m_numbers[index] == EMPTY)
                stream << ' ';
            else if (m_numbers[index] < 10)
                stream << static_cast<char>(m_numbers[index] + '0');
            else
                stream << static_cast<char>(m_numbers[index] + 'A' - 10);

            if (j % SIZE == SIZE - 1 && j != NCOUNT - 1)
                stream << '|';

            index++;
        }

        stream << '\n';
        if (i % SIZE == SIZE - 1 && i != NCOUNT - 1)
        {
            for (uint32_t k = 0; k < SIZE; ++k)
            {
                stream << string(SIZE, '-');
                if (k != SIZE - 1)
                    stream << '+';
            }

            stream << "\n";
        }
    }
}

template <uint32_t SIZE>
void Solver<SIZE>::RestrictPossibilities()
{
    for (uint32_t i = 0; i < NCOUNT*NCOUNT; i++)
    {
        if (m_numbers[i] == EMPTY || m_processed[i])
            continue;

        for (const auto nbi: neighbors[i])
            m_possibilities[nbi].reset(m_numbers[i] - 1);

        m_processed[i] = true;
    }
}

template <uint32_t SIZE>
tuple<bool, bool, bool> Solver<SIZE>::WriteDownSolePossibilities()
{
    bool wasReplenished = false;
    bool isComplete = true;

    for (uint32_t i = 0; i < NCOUNT*NCOUNT; i++)
    {
        if (m_numbers[i] != EMPTY)
            continue;

        switch(m_possibilities[i].count())
        {
            case 0:
                return { false, false, false };
            case 1:
            {
                // find the only possible number
                m_numbers[i] = static_cast<uint8_t>(m_possibilities[i]._Find_first() + 1u);
                wasReplenished = true;
                break;
            }
            default:
                isComplete = false;
        }
    }
    return { true, isComplete, wasReplenished };
}

// We did not add any m_numbers during the last iteration, so we have to assume
template <uint32_t SIZE>
bool Solver<SIZE>::AssumeNumber()
{
    uint32_t index = 0;
    while (m_numbers[index] != EMPTY)
        ++index;

    auto poss_index = m_possibilities[index]._Find_first();
    while (poss_index != NCOUNT)
    {
        auto backup = *this;

        /* cout << "assumption(" << index << ") number = " << poss_index + 1 << endl; */
        m_numbers[index] = static_cast<uint8_t>(poss_index + 1u);
        if (Solve())
            return true;

        /* cout << "wrong assumption(" << index << ")" << endl; */
        *this = move(backup);
        poss_index = m_possibilities[index]._Find_next(poss_index);
    }

    return false;
}

template <uint32_t SIZE>
bool Solver<SIZE>::Solve()
{
    while (true)
    {
        // for every not empty cell update m_possibilities of its neighbors
        RestrictPossibilities();

        // for every empty puzzle cell check if there is only one possible number
        const auto [isSolvable, isComplete, wasReplenished] = WriteDownSolePossibilities();
        if (!isSolvable || isComplete)
            return isSolvable;

        // if m_numbers was not replenished make an assumption
        if (!wasReplenished)
            return AssumeNumber();
    }
}

// check if the puzzle was completely solved
template <uint32_t SIZE>
bool Solver<SIZE>::SolutionIsComplete() const
{
    if (find(m_numbers.cbegin(), m_numbers.cend(), EMPTY) != m_numbers.cend())
        return false;

    for (uint32_t i = 0; i < NCOUNT; ++i)
    {
        const uint32_t boxFirst = i % SIZE * SIZE * NCOUNT + i / SIZE * SIZE;
        set<uint32_t> rowNums, colNums, boxNums;

        for (uint32_t j = 0; j < NCOUNT; ++j)
        {
            const uint32_t rowNum = i * NCOUNT + j;
            const uint32_t colNum = j * NCOUNT + i;
            const uint32_t boxNum = boxFirst + j / SIZE * NCOUNT + j % SIZE;

            if (!rowNums.insert(rowNum).second
             || !colNums.insert(colNum).second
             || !boxNums.insert(boxNum).second)
                return false;
        }
    }

    return true;
}

// explicit instantiation definition
template class Sudoku::Solver<3>;
