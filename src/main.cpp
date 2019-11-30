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

    cout << "Puzzle:" << endl;
    solver.printState(cout);
    cout << endl;

    cout << (solver.solve() ? "Solution:\n" : "Error. Log of states:\n");
    solver.printSolution(cout);
}
