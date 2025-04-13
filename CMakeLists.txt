cmake_minimum_required(VERSION 3.30)
project(statki)

set(CMAKE_CXX_STANDARD 20)

add_executable(statki main.cpp
        aitest.cpp)

find_package(Python3 REQUIRED)
execute_process(COMMAND python3 -m pybind11 --cmake
        OUTPUT_VARIABLE PYBIND11_DIR
        OUTPUT_STRIP_TRAILING_WHITESPACE)

set(PYBIND11_DIR "C:/Users/kubaa/AppData/Local/Programs/Python/Python313/Lib/site-packages/pybind11/share/cmake/pybind11")

add_subdirectory(${PYBIND11_DIR} ${CMAKE_BINARY_DIR}/pybind11)


target_link_libraries(your_target_name PRIVATE pybind11::module)

