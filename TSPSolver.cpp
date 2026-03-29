#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <chrono>
#include <algorithm>
#include <numeric>
#include <random>
#include <iomanip>

using namespace std;

class TSPSolver {
private:
    vector<vector<int>> matrix;
    int n;
    mt19937 rng;

public:
    TSPSolver() : rng(random_device{}()), n(0) {}

    bool loadMatrix(string filename) {
        ifstream file(filename);
        if (!file.is_open()) return false;
        if (!(file >> n)) return false;

        matrix.assign(n, vector<int>(n));
        for (int i = 0; i < n; i++) {
            for (int j = 0; j < n; j++) {
                if (!(file >> matrix[i][j])) break;
            }
        }
        file.close();
        return true;
    }

    void generateRandom(int size) {
        n = size;
        matrix.assign(n, vector<int>(n));
        uniform_int_distribution<int> dist(1, 100);
        for (int i = 0; i < n; i++) {
            for (int j = 0; j < n; j++) {
                matrix[i][j] = (i == j) ? -1 : dist(rng);
            }
        }
    }

    long long calculatePathCost(const vector<int>& path) {
        long long cost = 0;
        if (n == 0) return 0;
        for (int i = 0; i < n - 1; i++) {
            if (matrix[path[i]][path[i + 1]] == -1) return 1e15;
            cost += matrix[path[i]][path[i + 1]];
        }
        cost += matrix[path.back()][path[0]];
        return cost;
    }

    long long solveRAND() {
        if (n <= 0) return 0;
        vector<int> path(n);
        iota(path.begin(), path.end(), 0);
        shuffle(path.begin(), path.end(), rng);
        return calculatePathCost(path);
    }

    // --- NOWA FUNKCJA BADAWCZA ---
    long long countRandIterationsUntilError(long long optCost, double targetError) {
        if (n <= 0 || optCost <= 0) return 0;
        vector<int> path(n);
        iota(path.begin(), path.end(), 0);
        
        long long iterations = 0;
        long long bestCost = numeric_limits<long long>::max();
        long long maxLimit = 2700000000; // Limit 10 mln, żeby program nie wisiał wiecznie

        while (iterations < maxLimit) {
            iterations++;
            shuffle(path.begin(), path.end(), rng);
            long long currentCost = calculatePathCost(path);
            if (currentCost < bestCost) bestCost = currentCost;

            double currentError = ((double)(bestCost - optCost) / optCost) * 100.0;
            if (currentError <= targetError) return iterations;
        }
        return -1; // Zwraca -1 jeśli nie znaleziono w 10 mln prób
    }

    long long solveNN(int start) {
        if (n <= 0) return 0;
        if (start >= n) start = 0;
        vector<int> path;
        vector<bool> visited(n, false);
        int current = start;
        path.push_back(current);
        visited[current] = true;
        for (int i = 0; i < n - 1; i++) {
            int next = -1;
            int minW = numeric_limits<int>::max();
            for (int j = 0; j < n; j++) {
                if (!visited[j] && matrix[current][j] != -1 && matrix[current][j] < minW) {
                    minW = matrix[current][j];
                    next = j;
                }
            }
            if (next != -1) {
                visited[next] = true;
                path.push_back(next);
                current = next;
            }
        }
        return calculatePathCost(path);
    }

    long long solveRNN() {
        long long best = numeric_limits<long long>::max();
        for (int i = 0; i < n; i++) best = min(best, solveNN(i));
        return best;
    }

    long long solveBF() {
        if (n <= 0) return 0;
        vector<int> p(n);
        iota(p.begin(), p.end(), 0);
        long long minC = numeric_limits<long long>::max();
        do {
            minC = min(minC, calculatePathCost(p));
        } while (next_permutation(p.begin() + 1, p.end()));
        return minC;
    }

    int getSize() { return n; }
};

int main() {
    TSPSolver solver;
    ifstream config("config.txt");
    if (!config.is_open()) { cout << "!!! BLAD: Brak config.txt !!!" << endl; return 1; }

    string inputFileName, outputFileName;
    int sizeToGen, repeats, nnStartCity;
    bool doFileTest, doRandomTest;

    if (!(config >> inputFileName >> outputFileName >> doFileTest >> doRandomTest >> sizeToGen >> repeats >> nnStartCity)) {
        cout << "BLAD: Niepoprawny format danych w config.txt" << endl; return 1;
    }
    config.close();

    ofstream csv(outputFileName, ios::app);

    auto runTest = [&](string name, auto func, long long optCost = -1) {
        auto start = chrono::high_resolution_clock::now();
        long long cost = func();
        auto end = chrono::high_resolution_clock::now();
        double time = chrono::duration<double, micro>(end - start).count();
        double error = (optCost > 0) ? (((double)(cost - optCost) / optCost) * 100.0) : 0.0;
        string tStr = to_string(time); replace(tStr.begin(), tStr.end(), '.', ',');
        string eStr = to_string(error); replace(eStr.begin(), eStr.end(), '.', ',');

        cout << left << setw(12) << name << " | Koszt: " << setw(7) << cost
            << " | Czas: " << fixed << setprecision(2) << setw(10) << time << " us"
            << " | Blad: " << error << "%" << endl;
        csv << name << ";" << solver.getSize() << ";" << cost << ";" << tStr << ";" << eStr << "%\n";
        return cost;
    };

    // --- TEST Z PLIKU ---
    if (doFileTest && solver.loadMatrix(inputFileName)) {
        cout << "\n>>> TEST Z PLIKU: " << inputFileName << endl;
        long long opt = -1;
        if (solver.getSize() <= 13) opt = runTest("BruteForce", [&]() { return solver.solveBF(); });
        runTest("RAND", [&]() { return solver.solveRAND(); }, opt);
        runTest("NN", [&]() { return solver.solveNN(nnStartCity); }, opt);
        runTest("RNN", [&]() { return solver.solveRNN(); }, opt);

        if (opt > 0) {
            long long iters = solver.countRandIterationsUntilError(opt, 10.0);
            cout << "[INFO] RAND potrzebowal " << iters << " iteracji by błąd <= 10%" << endl;
            csv << "RAND_RESEARCH;" << solver.getSize() << ";-;" << iters << ";10%\n";
        }
    }

    // --- TESTY LOSOWE ---
    if (doRandomTest) {
        cout << "\n>>> TESTY LOSOWYCH (N=" << sizeToGen << ")" << endl;
        for (int r = 0; r < repeats; r++) {
            solver.generateRandom(sizeToGen);
            cout << "\n PROBA " << r + 1 << endl;
            long long opt = -1;
            if (sizeToGen <= 13) opt = runTest("BruteForce", [&]() { return solver.solveBF(); });
            runTest("RAND", [&]() { return solver.solveRAND(); }, opt);
            runTest("NN", [&]() { return solver.solveNN(nnStartCity); }, opt);
            runTest("RNN", [&]() { return solver.solveRNN(); }, opt);

            if (opt > 0) {
                long long iters = solver.countRandIterationsUntilError(opt, 10.0);
                cout << "[INFO] RAND potrzebowal " << iters << " iteracji by błąd <= 10%" << endl;
                csv << "RAND_RESEARCH;" << solver.getSize() << ";-;" << iters << ";10%\n";
            }
        }
    }

    csv.close();
    return 0;
}