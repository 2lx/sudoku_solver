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
            m_data[index].fromChar(ch);
            ++index;
        }
    }

    return true;
}

template <uint32_t SIZE>
void Solver<SIZE>::print(std::ostream& stream) const
{
    if (!isFilled() || !isCorrect())
        cout << "Current state is not a valid solution" << endl;

    uint32_t index = 0;
    for (uint32_t i = 0; i < NCOUNT; ++i)
    {
        for (uint32_t j = 0; j < NCOUNT; ++j)
        {
            stream << m_data[index];
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
    const size_t number = m_data[index].number();

    for (const auto nbi: m_rowNeighbors[row(index)])
        m_data[nbi].disable(number);

    for (const auto nbi: m_colNeighbors[col(index)])
        m_data[nbi].disable(number);

    for (const auto nbi: m_boxNeighbors[box(index)])
        m_data[nbi].disable(number);
}

template <uint32_t SIZE>
bool Solver<SIZE>::restrict()
{
    auto fn_check = [this](uint8_t number, uint32_t ind, const auto & nbs)
    {
        return nbs.cend() == find_if(nbs.cbegin(), nbs.cend(), [&](auto nbi)
        {
            const auto & data = m_data[nbi];
            return nbi != ind && data.is_empty() && data.is_possible(number);
        });
    };

    bool result = false;

    for (uint8_t number = 1; number <= NCOUNT; ++number)
        for (uint32_t i = 0; i < NCOUNT*NCOUNT; ++i)
        {
            auto & data = m_data[i];

            if (!data.is_empty() || !data.is_possible(number))
                continue;

            if (data.is_onlyone()
             || fn_check(number, i, m_rowNeighbors[row(i)])
             || fn_check(number, i, m_colNeighbors[col(i)])
             || fn_check(number, i, m_boxNeighbors[box(i)]))
            {
                data.setNumber(number);
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
    const auto it = find_if(m_data.begin(), m_data.end(),
            [](const auto & data) { return data.is_empty(); });

    const size_t index = distance(m_data.begin(), it);
    const auto poss = it->possibilities();

    for (const auto & poss_number: poss)
    {
        const auto backup = m_data;

        print(cout);
        cout << "assume [" << col(index) << "," << row(index)
             << "]=" << poss_number << endl;
        it->setNumber(poss_number);

        if (solve())
            return true;

        cout << "wrong assumption [" << col(index) << "," << row(index)
             << "]=" << poss_number << endl;
        m_data = move(backup);
    }

    return false;
}

template <uint32_t SIZE>
bool Solver<SIZE>::solve()
{
    // initially update possibilities
    for (uint32_t i = 0; i < NCOUNT * NCOUNT; ++i)
        if (m_data[i].number() != EMPTY)
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
    return none_of(m_data.cbegin(), m_data.cend(),
            [](const auto & data) { return data.is_empty(); });
}

template <uint32_t SIZE>
bool Solver<SIZE>::isSolvable() const
{
    return none_of(m_data.cbegin(), m_data.cend(),
            [](const auto & data) { return data.is_inconsistent(); });
}

template <uint32_t SIZE>
bool Solver<SIZE>::isCorrect() const
{
    auto testUnique = [this](const auto& neighbors) {
        set<size_t> nums;

        for (auto nbi: neighbors)
            if (!nums.insert(m_data[nbi].number()).second)
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
