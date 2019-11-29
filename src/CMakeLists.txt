file(GLOB WOS_SRC "*.cpp")
list(REMOVE_ITEM WOS_SRC "main.cpp")

add_library(SudokuLib ${WOS_SRC})
target_include_directories(SudokuLib PRIVATE common)
add_executable(SudokuSolver main.cpp)
target_link_libraries(SudokuSolver SudokuLib)

