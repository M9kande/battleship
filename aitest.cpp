#include <pybind11/embed.h>
#include <iostream>
#include <vector>

namespace py = pybind11;

int main() {
    py::scoped_interpreter guard{};  // Initialize the Python interpreter

    // Import your Python script (make sure battleship_ai.py is in the same directory)
    py::module ai = py::module::import("battleship_ai");

    // Example game board and remaining ships
    std::vector<std::vector<int>> board(8, std::vector<int>(8, 0));
    std::vector<int> remaining_ships = {5, 4, 3, 3, 2};

    // Call the Python function and get the result
    py::object result = ai.attr("choose_move")(board, remaining_ships);
    auto coords = result.cast<std::pair<int, int>>();

    // Output the recommended move
    std::cout << "AI recommends: " << coords.first << ", " << coords.second << "\n";

    return 0;
}
