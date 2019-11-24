#include "sudoku_solver.h"

#include <iostream>
#include <algorithm>
#include <set>

using namespace std;
using namespace Sudoku;

constexpr uint8_t EMPTY = static_cast<uint8_t>(0);

template <uint32_t SIZE>
constexpr typename Solver<SIZE>::narray_t Solver<SIZE>::InitRowNeighbors()
{
    narray_t result{ 0 };

    for (auto row = 0u; row < NCOUNT; ++row)
        for (auto ind = 0u; ind < NCOUNT; ++ind)
            result[row][ind] = row * NCOUNT + ind;

    return result;
}

template <uint32_t SIZE>
constexpr typename Solver<SIZE>::narray_t Solver<SIZE>::InitColNeighbors()
{
    narray_t result{ 0 };

    for (auto col = 0u; col < NCOUNT; ++col)
        for (auto ind = 0u; ind < NCOUNT; ++ind)
            result[col][ind] = col + ind * NCOUNT;

    return result;
}

template <uint32_t SIZE>
constexpr typename Solver<SIZE>::narray_t Solver<SIZE>::InitBoxNeighbors()
{
    narray_t result{ 0 };

    for (auto box = 0u; box < NCOUNT; ++box)
    {
        const uint32_t boxFirst = box / SIZE * SIZE * NCOUNT + box % SIZE * SIZE;

        for (auto ind = 0u; ind < NCOUNT; ++ind)
            result[box][ind] = boxFirst + ind / SIZE * NCOUNT + ind % SIZE;
    }

    return result;
}

template <uint32_t SIZE>
bool Solver<SIZE>::read(std::istream & stream)
{
    for (uint32_t i = 0; i < NCOUNT * NCOUNT; ++i)
        m_possibilities[i].set();

    string str;
    uint32_t index = 0;
    for (uint32_t i = 0; i < NCOUNT; ++i)
    {
        if (!getline(stream, str) || str.length() != NCOUNT)
        {
            cerr << "Invalid input data" << endl;
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

            ++index;
        }
    }

    return true;
}

template <uint32_t SIZE>
void Solver<SIZE>::print(std::ostream& stream) const
{
    if (!isFilled() || !isCorrect())
    {
        cerr << "Algorithm error occured" << endl;
        /* return; */
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
void Solver<SIZE>::updatePossibilities(uint32_t index)
{
    const size_t number = m_numbers[index] - 1;

    for (const auto nbi: m_rowNeighbors[row(index)])
        m_possibilities[nbi].reset(number);

    for (const auto nbi: m_colNeighbors[col(index)])
        m_possibilities[nbi].reset(number);

    for (const auto nbi: m_boxNeighbors[box(index)])
        m_possibilities[nbi].reset(number);
}

template <uint32_t SIZE>
bool Solver<SIZE>::restrict()
{
    auto fn_check = [this](uint8_t number, uint32_t ind, const auto & neighbors)
    {
        for (size_t i = 0; i < NCOUNT; ++i)
        {
            const auto nbi = neighbors[i];

            if (nbi != ind && m_numbers[nbi] == EMPTY && m_possibilities[nbi][number - 1])
                return false;
        }

        return true;
    };

    bool result = false;

    for (uint8_t number = 1; number <= NCOUNT; ++number)
        for (uint32_t i = 0; i < NCOUNT*NCOUNT; ++i)
        {
            if (m_numbers[i] != EMPTY || m_possibilities[i][number - 1] == false)
                continue;

            if (m_possibilities[i].count() == 1
             || fn_check(number, i, m_rowNeighbors[row(i)])
             || fn_check(number, i, m_colNeighbors[col(i)])
             || fn_check(number, i, m_boxNeighbors[box(i)]))
            {
                m_numbers[i] = number;
                updatePossibilities(i);
                result = true;
            }
        }

    return result;
}

// We did not add any m_numbers during the last iteration, so we have to assume
template <uint32_t SIZE>
bool Solver<SIZE>::assumeNumber()
{
    uint32_t index = 0;
    while (m_numbers[index] != EMPTY)
        ++index;

    auto poss_index = m_possibilities[index]._Find_first();
    while (poss_index != NCOUNT)
    {
        auto backup = *this;

        print(cout);
        cout << "assume index=" << index << ", number=" << poss_index + 1 << endl;
        m_numbers[index] = static_cast<uint8_t>(poss_index + 1u);

        if (solve())
            return true;

        cout << "wrong assumption(" << index << ")" << endl;
        *this = move(backup);
        poss_index = m_possibilities[index]._Find_next(poss_index);
    }

    return false;
}

template <uint32_t SIZE>
bool Solver<SIZE>::solve()
{
    for (uint32_t i = 0; i < NCOUNT * NCOUNT; ++i)
        if (m_numbers[i] != EMPTY)
            updatePossibilities(i);

    while (true)
    {
        const bool isSolvable = this->isSolvable();
        if (isFilled() || !isSolvable)
            return isSolvable;

        if (!restrict())
            return assumeNumber();
    }
}

template <uint32_t SIZE>
bool Solver<SIZE>::isFilled() const
{
    return m_numbers.cend() == find(m_numbers.cbegin(), m_numbers.cend(), EMPTY);
}

template <uint32_t SIZE>
bool Solver<SIZE>::isSolvable() const
{
    for (uint32_t i = 0; i < NCOUNT*NCOUNT; i++)
        if (m_numbers[i] == EMPTY && m_possibilities[i].count() == 0)
            return false;

    return true;
}

template <uint32_t SIZE>
bool Solver<SIZE>::isCorrect() const
{
    auto testUnique = [this](const auto& neighbors) {
        set<uint32_t> nums;

        for (auto nbi: neighbors)
            if (!nums.insert(m_numbers[nbi]).second)
                return false;

        return true;
    };

    for (uint32_t i = 0; i < NCOUNT; ++i)
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
