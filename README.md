# Analysis of Algorithms for the Travelling Salesman Problem (ATSP/STSP)

## Project Overview
This project focuses on the implementation and performance analysis of various algorithms solving the **Travelling Salesman Problem (TSP)** in both asymmetric (ATSP) and symmetric (STSP) variants. The study compares exact methods with heuristic approaches in terms of time complexity and solution quality (Upper Bound vs Lower Bound).

## Implemented Algorithms
* **Brute Force (BF):** Exact algorithm with O(n!) complexity.
* **Nearest Neighbor (NN):** Greedy heuristic starting from node 0 (O(n^2)).
* **Repetitive Nearest Neighbor (RNN):** Improved heuristic checking all possible starting nodes (O(n^3)).
* **Random Search (RAND):** Baseline approach using factorial number system (Lehmer code) for path generation.
  
## Configuration Parameters (config.txt)
The program reads parameters line by line in the following order:

1. **Input file** (`dane.atsp`) – Path to the file containing the cost matrix.
2. **Output file** (`wyniki.csv`) – Path where results will be exported.
3. **File test flag** (`1`) – `1` to run the test from the input file, `0` to skip.
4. **Random test flag** (`1`) – `1` to run tests on a randomly generated matrix, `0` to skip.
5. **Matrix size** (`12`) – Dimension $N$ for the randomly generated matrix.
6. **Iterations** (`5`) – Number of times the random test should be repeated for averaging.
7. **Start node** (`0`) – Index of the starting city for the Nearest Neighbor (NN) algorithm.
