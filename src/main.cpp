#include <iostream>
#include "solver.h"

using namespace std;

int main()
{
    Sudoku::Solver<3> solver;

    if (!solver.read(cin))
    {
        cout << "Wrong input format" << endl;
        return EXIT_FAILURE;
    }

    solver.solve();
    solver.print(cout);
}
