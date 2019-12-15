#ifndef BOARD_H
#define BOARD_H

#include "cell.h"
#include "string_join.h"
#include "term_color.h"

#include <numeric>

namespace Sudoku
{
enum NbType { ROW, COL, BOX };

template <size_t N>
class Board
{
private:
    static constexpr size_t N2 = N * N;
    using narray_t = std::array<std::array<size_t, N2>, N2>;

public:
    Board() = default;

    Board(const Board &) = default;
    Board & operator=(Board &&) = default;
    Board & operator=(const Board &) = delete;
    Board(Board &&) = delete;

    bool read(std::istream & stream);
    std::string getState(const std::optional<std::pair<bool, size_t>> & mark = std::nullopt) const;

    void setCellNumber(const Cell<N2> & cell, size_t number);
    bool isCorrect() const;
    bool isSolvable() const;
    bool isFilled() const;

    bool numberCanOnlyBeHere(const Cell<N2> & cell, size_t number) const;
    const Cell<N2> & firstEmptyCell() const;
    const std::array<Cell<N2>, N2 * N2> & cells() const { return m_cells; }

private:
    size_t col(size_t i) const { return i % N2; }
    size_t row(size_t i) const { return i / N2; }
    size_t box(size_t i) const { return (i / N2) / N * N + (i % N2) / N; }

    void updatePossibilities(const Cell<N2> & cell);

    template <NbType TYPE>
    static constexpr narray_t initNeighbors();

private:
    std::array<Cell<N2>, N2 * N2> m_cells;

    static constexpr narray_t m_neighbors[] =
        { initNeighbors<ROW>(), initNeighbors<COL>(), initNeighbors<BOX>() };
};

template <size_t N>
template <NbType TYPE>
constexpr typename Board<N>::narray_t Board<N>::initNeighbors()
{
    narray_t result{ 0 };

    for (size_t i1 = 0; i1 < N2; ++i1)
    {
        const size_t boxFirst = i1 / N * N * N2 + i1 % N * N;

        for (size_t i2 = 0; i2 < N2; ++i2)
            if constexpr (TYPE == ROW)
                result[i1][i2] = i1 * N2 + i2;
            else if constexpr (TYPE == COL)
                result[i1][i2] = i2 * N2 + i1;
            else
                result[i1][i2] = boxFirst + i2 / N * N2 + i2 % N;
    }

    return result;
}

template <size_t N>
bool Board<N>::read(std::istream & stream)
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

template <size_t N>
std::string Board<N>::getState(const std::optional<std::pair<bool, size_t>> & mark) const
{
    using namespace Term;

    auto fn_rowdelim = [](const std::string & beg, const std::string & mid, const std::string & end)
    {
        const std::string str = string_join(std::vector<const char *>{ N, "─" }, "");
        return beg + string_join(std::vector(N, str), mid.c_str()) + end + '\n';
    };

    const Color  mark_color = mark.has_value() && mark.value().first ? Color::Green
                                                                     : Color::Red;
    const size_t mark_index = mark.has_value() ? mark.value().second
                                               : std::numeric_limits<size_t>::max();

    std::ostringstream ss;
    std::array<std::string, N> strings, rows, blocks;
    size_t index = 0;

    for (auto & block: blocks)
    {
        for (auto & row: rows)
        {
            for (auto & str: strings)
            {
                ss.str(std::string());

                for (size_t i = 0; i < N; ++i)
                {
                    const Color color = mark_index == index ? mark_color : Color::Yellow;
                    ss << set_font(color, Color::None, Effect::Bold) << m_cells[index++];
                }

                ss << set_font(Color::None);
                str = ss.str();
            }
            row = "│" + string_join(strings, "│") + "│";
        }
        block = string_join(rows, "\n") + '\n';
    }

    return fn_rowdelim("┌", "┬", "┐")
        + string_join(blocks, fn_rowdelim("├", "┼", "┤").c_str())
        + fn_rowdelim("└", "┴", "┘");
}

template <size_t N>
void Board<N>::updatePossibilities(const Cell<N2> & cell)
{
    for (const auto nbi: m_neighbors[ROW][row(cell.index())])
        m_cells[nbi].disable(cell.number());

    for (const auto nbi: m_neighbors[COL][col(cell.index())])
        m_cells[nbi].disable(cell.number());

    for (const auto nbi: m_neighbors[BOX][box(cell.index())])
        m_cells[nbi].disable(cell.number());
}

template <size_t N>
const Cell<Board<N>::N2> & Board<N>::firstEmptyCell() const
{
    return *std::find_if(m_cells.begin(), m_cells.end(),
                         bind(&Cell<N2>::isEmpty, std::placeholders::_1));
}

template <size_t N>
bool Board<N>::numberCanOnlyBeHere(const Cell<N2> & cell, size_t number) const
{
    auto fn_check = [this](size_t number, size_t ind, const auto & nbs)
    {
        return std::none_of(nbs.cbegin(), nbs.cend(), [&](auto nbi)
        {
            const auto & cell = m_cells[nbi];
            return nbi != ind && cell.isEmpty() && cell.isPossible(number);
        });
    };

    return fn_check(number, cell.index(), m_neighbors[ROW][row(cell.index())])
        || fn_check(number, cell.index(), m_neighbors[COL][col(cell.index())])
        || fn_check(number, cell.index(), m_neighbors[BOX][box(cell.index())]);
}

template <size_t N>
void Board<N>::setCellNumber(const Cell<N2> & cell, size_t number)
{
    m_cells[cell.index()].setNumber(number);
    updatePossibilities(cell);
}

template <size_t N>
bool Board<N>::isFilled() const
{
    return std::none_of(m_cells.cbegin(), m_cells.cend(),
            [](const auto & cell) { return cell.isEmpty(); });
}

template <size_t N>
bool Board<N>::isSolvable() const
{
    return std::none_of(m_cells.cbegin(), m_cells.cend(),
            [](const auto & cell) { return cell.isInconsistent(); });
}

template <size_t N>
bool Board<N>::isCorrect() const
{
    auto fn_testUnique = [this](const auto & nbs) {
        std::bitset<N2> flags;

        for (const auto & nbi: nbs)
            flags.set(m_cells[nbi].number() - 1);

        return flags.all();
    };

    std::array<size_t, N2> indexes;
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
