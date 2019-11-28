#include <iostream>
#include "solver.h"

using namespace std;

int main()
{
    Sudoku::Solver<3> solver;

    if (!solver.read(cin))
    {
        cout << "Wrong input string format" << endl;
        return EXIT_FAILURE;
    }

    solver.solve();

    cout << (solver.isCorrect() ? "Result:\n" : "No result. Current state:\n");
    solver.print(cout);
}
