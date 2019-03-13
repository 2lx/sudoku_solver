#include <iostream>
#include "sudoku_solver.h"

using namespace std;

int main() {
    Sudoku::Solver<3> solver;
    if (!solver.read_level_data(cin)) {
        cout << "Invalid input data" << endl;
        return EXIT_FAILURE;
    }
    solver.solve();
    if (solver.solution_is_complete()) {
        solver.print_solution(cout);
    } else {
        cout << "Algorithm error occured" << endl;
    }
}
