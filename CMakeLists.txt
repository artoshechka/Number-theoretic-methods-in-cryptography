cmake_minimum_required(VERSION 3.10)
project(NumberTheoreticMethods)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Добавляем подпроекты
add_subdirectory(big_numbers)
# Добавляем подпроекты
add_subdirectory(big_numbers_algorithms)

# Главный исполняемый файл
add_executable(MainApp test.cpp)

# Линкуем с библиотеками
target_link_libraries(MainApp BigNumbersLib BigNumbersAlgoLib)
