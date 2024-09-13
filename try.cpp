#include <iostream>
#include <queue>
#include <vector>

int main() {
    // Priority queue (max-heap by default) to store pairs of (key, value)
    std::priority_queue<std::pair<int, double>> heap;

    // Example key-value pairs
    std::vector<std::pair<int, double>> keyValuePairs = {
        {1, 2.5},
        {3, 3.8},
        {2, 1.7},
        {5, 4.2},
        {4, 3.0}
    };

    // Insert key-value pairs into the heap in a for loop
    for (const auto& pair : keyValuePairs) {
        heap.push(pair);
        std::cout << "Inserted (" << pair.first << ", " << pair.second << ") into the heap." << std::endl;
    }

    // Output the elements of the heap (in descending order by key)
    std::cout << "\nElements in the heap (max-heap based on key):" << std::endl;
    while (!heap.empty()) {
        auto top = heap.top();
        std::cout << "Key: " << top.first << ", Value: " << top.second << std::endl;
        heap.pop();  // Remove the top element
    }

    return 0;
}