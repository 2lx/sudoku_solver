#include <iostream>
#include "solver.h"

using namespace std;

int main()
{
    Sudoku::Solver<3> solver;

    if (!solver.read(cin))
    {
        cout << "Wrong input" << endl;
        return EXIT_FAILURE;
    }

    solver.solve();

    cout << (solver.isCorrect() ? "Result:\n" : "Error. Current state:\n");
    solver.print(cout);
}
