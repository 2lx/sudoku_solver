#ifndef SOLVER_H
#define SOLVER_H

#include "board.h"

namespace Sudoku
{

template <size_t N>
class Solver
{
public:
    Solver() = default;

    bool read(std::istream & stream)             { return m_board.read(stream); }
    bool isCorrect() const                       { return m_board.isCorrect(); }

    void printState(std::ostream & stream) const { stream << m_board.getState(); }
    void printSolution(std::ostream & stream) const;

    bool solve();

private:
    bool narrow();
    bool assumeNumber();

private:
    Board<N> m_board;
    std::vector<std::string> m_states;
};

template <size_t N>
void Solver<N>::printSolution(std::ostream & stream) const
{
    std::array<std::string, N * N + N + 1> lines;

    for (const auto & state: m_states)
    {
        std::istringstream ss(state);
        std::string line;

        size_t iline = 0;
        while (std::getline(ss, line))
            lines[iline++] += line + "  ";
    }

    stream << string_join(lines, "\n") << std::endl;
}

template <size_t N>
bool Solver<N>::narrow()
{
    bool narrowed = false;

    for (const auto & cell: m_board.cells())
    {
        if (!cell.isEmpty())
            continue;

        const auto numbers = cell.possibilities();
        for (const auto & number: numbers)
        {
            if (cell.isOnlyOnePossibility() || m_board.numberCanOnlyBeHere(cell, number))
            {
                m_board.setCellNumber(cell, number);
                narrowed = true;
                break;
            }
        }
    }

    return narrowed;
}

template <size_t N>
bool Solver<N>::assumeNumber()
{
    const auto & cell = m_board.firstEmptyCell();

    // and try to substitute one of its possibilities
    for (const auto & number: cell.possibilities())
    {
        auto backup{ m_board };

        m_board.setCellNumber(cell, number);
        m_states.emplace_back(m_board.getState(std::make_pair(true, cell.index())));

        if (solve())
            return true;

        m_states.emplace_back(m_board.getState(std::make_pair(false, cell.index())));
        m_board = std::move(backup);
    }

    return false;
}

template <size_t N>
bool Solver<N>::solve()
{
    while (true)
    {
        if (!m_board.isSolvable())
            return false;

        if (m_board.isFilled())
        {
            m_states.emplace_back(m_board.getState());
            return true;
        }

        if (!narrow())
            return assumeNumber();
    }
}

}

#endif

