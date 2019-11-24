#include <iostream>
#include "sudoku_solver.h"

using namespace std;

int main()
{
    Sudoku::Solver<3> solver;

    if (!solver.read(cin))
        return EXIT_FAILURE;

    solver.solve();
    solver.print(cout);
}
