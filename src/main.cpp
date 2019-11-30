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

    cout << "Puzzle:" << endl << solver.getState() << endl;
    solver.solve();

    cout << (solver.isCorrect() ? "Solution:\n" : "Error. Current state:\n");
    solver.printSolution(cout);
}
