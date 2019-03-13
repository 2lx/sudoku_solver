file(GLOB WOS_SRC "*.cpp")
list(REMOVE_ITEM WOS_SRC "main.cpp")

add_library(SudokuLib ${WOS_SRC})
target_include_directories(SudokuLib PRIVATE common)
add_executable(SudokuSolver main.cpp)
target_link_libraries(SudokuSolver SudokuLib)

# add_executable(SudokuLogging EXCLUDE_FROM_ALL ${WOS_SRC} main.cpp)
# target_compile_definitions(SudokuLogging PRIVATE LOGGING)
# target_include_directories(SudokuLogging PRIVATE common)
