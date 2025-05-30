#include <iostream>
#include <vector>
#include <set>
#include <chrono>
#include <random>

using namespace std;
using namespace chrono;

class Graph {
public:
    int n;
    vector<vector<int>> adj;
    vector<vector<bool>> used;

    Graph(int size) : n(size), adj(size), used(size, vector<bool>(size, false)) {}



    void addEdge(int u, int v) {
        adj[u].push_back(v);
        adj[v].push_back(u);
        used[u][v] = used[v][u] = false;
    }

    static Graph generateGraph(int n, double densityPercent) {
        Graph g(n);
        int maxEdges = n * (n - 1) / 2;
        int edgeCount = (int)(densityPercent / 100.0 * maxEdges);

        // Tworzymy spójny cykl bazowy (Hamilton i Euler)
        for (int i = 0; i < n - 1; ++i)
            g.addEdge(i, i + 1);
        g.addEdge(n - 1, 0);

        set<pair<int, int>> existingEdges;
        for (int i = 0; i < n; ++i)
            for (int j : g.adj[i])
                if (i < j) existingEdges.insert({ i, j });

        random_device rd;
        mt19937 gen(rd());
        uniform_int_distribution<> dist(0, n - 1);

        while ((int)existingEdges.size() < edgeCount) {
            int u = dist(gen), v = dist(gen);
            if (u == v) continue;
            pair<int, int> e = { min(u,v), max(u,v) };
            if (existingEdges.count(e)) continue;
            g.addEdge(u, v);
            existingEdges.insert(e);
        }

        // Dopilnuj, żeby każdy wierzchołek miał parzysty stopień (Euler)
        for (int i = 0; i < n; ++i) {
            if (g.adj[i].size() % 2 != 0) {
                int j = (i + 1) % n;
                // Sprawdź, czy krawędź już istnieje, aby uniknąć duplikatu
                pair<int, int> e = { min(i,j), max(i,j) };
                if (existingEdges.count(e) == 0) {
                    g.addEdge(i, j);
                    existingEdges.insert(e);
                }
            }
        }

        return g;
    }

    void resetUsed() {
        used = vector<vector<bool>>(n, vector<bool>(n, false));
    }

    void euler(int v, vector<int>& cycle) {
        for (int u : adj[v]) {
            if (!used[v][u]) {
                used[v][u] = used[u][v] = true;
                euler(u, cycle);
            }
        }
        cycle.push_back(v);
    }

    bool hamiltonUtil(int v, vector<bool>& visited, vector<int>& path, int depth) {
        path.push_back(v);
        visited[v] = true;

        if (depth == n) {
            for (int u : adj[v]) {
                if (u == path[0]) {
                    path.push_back(path[0]);
                    return true;
                }
            }
        }

        for (int u : adj[v]) {
            if (!visited[u]) {
                if (hamiltonUtil(u, visited, path, depth + 1))
                    return true;
            }
        }

        visited[v] = false;
        path.pop_back();
        return false;
    }

    bool hamilton(vector<int>& path) {
        vector<bool> visited(n, false);
        return hamiltonUtil(0, visited, path, 1);
    }
};



void test(int n, double density) {
    cout << "Test dla n = " << n << ", gęstość = " << density << "%\n";
    Graph g = Graph::generateGraph(n, density);

    vector<int> eulerCycle;
    auto start = high_resolution_clock::now();
    g.resetUsed();
    g.euler(0, eulerCycle);
    auto end = high_resolution_clock::now();
    auto timeEuler = duration_cast<microseconds>(end - start).count();

    cout << "Czas algorytmu Eulera: " << timeEuler << " µs\n";

    vector<int> hamiltonCycle;
    start = high_resolution_clock::now();
    bool hFound = g.hamilton(hamiltonCycle);
    end = high_resolution_clock::now();
    auto timeHamilton = duration_cast<microseconds>(end - start).count();

    cout << "Czas algorytmu Hamiltona: " << timeHamilton << " µs\n";
    cout << "Czy cykl Hamiltona znaleziony? " << (hFound ? "TAK" : "NIE") << "\n\n";
}


int main() {
    // Testy dla małych n, bo dla większych będzie długo
    for (int n = 5; n <= 65; n += 5) {
        test(n, 30.0);  // rzadki graf
    }
    for (int n = 5; n <= 125; n += 5) {
        test(n, 70.0);  // gęsty graf
    }
    return 0;
}
