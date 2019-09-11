#include <iostream>
#include "sudoku_solver.h"

using namespace std;

int main()
{
    Sudoku::Solver<3> solver;

    if (!solver.ReadLevelData(cin))
        return EXIT_FAILURE;

    solver.Solve();
    solver.PrintSolution(cout);
}
