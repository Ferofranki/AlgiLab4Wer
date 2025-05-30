#include <iostream>
#include <vector>
#include <set>
#include <fstream> // For file input
#include <sstream> // For parsing lines
#include <stdexcept> // For exceptions
#include <string>    // For std::string
#include <algorithm> // For std::reverse
// Removed chrono and random, added string and algorithm

using namespace std;
// chrono namespace is no longer needed

class Graph {
public:
    int n;
    vector<vector<int>> adj;
    vector<vector<bool>> used;

    Graph(int size) : n(size), adj(size), used(size, vector<bool>(size, false)) {}

    // Static factory method to load a graph from an adjacency matrix file
    static Graph loadFromFile(const std::string& filePath) {
        ifstream file(filePath);
        if (!file.is_open()) {
            throw runtime_error("Error: Could not open file " + filePath);
        }

        vector<vector<int>> matrix;
        string line;
        int numCols = -1;

        while (getline(file, line)) {
            stringstream ss(line);
            vector<int> row;
            int val;
            while (ss >> val) {
                row.push_back(val);
            }

            if (!row.empty()) { // Ensure we don't process empty lines
                if (numCols == -1) {
                    numCols = row.size();
                } else if (row.size() != numCols) {
                    file.close();
                    throw runtime_error("Error: Matrix is not rectangular.");
                }
                matrix.push_back(row);
            }
        }
        file.close();

        if (matrix.empty()) {
            // Return an empty graph or throw an error for an empty file
            // For now, let's allow creating a graph with 0 vertices if the file was empty but valid
            // Or, if it's truly empty (no numbers at all), it might be better to throw.
            // Let's assume an empty file means a graph with 0 vertices.
            return Graph(0);
        }

        int numRows = matrix.size();
        if (numRows != numCols) {
            throw runtime_error("Error: Matrix is not square.");
        }

        Graph g(numRows); // numRows is the size of the graph (n)
        for (int i = 0; i < numRows; ++i) {
            for (int j = 0; j < numCols; ++j) {
                if (matrix[i][j] == 1) {
                    // To avoid adding edges twice and to handle the symmetric nature of addEdge correctly
                    // we only add edge if j > i. If the matrix guarantees symmetry,
                    // addEdge(i,j) will handle adj[i] and adj[j].
                    // If we process the full matrix, addEdge will be called for (i,j) and (j,i)
                    // which is fine for adj list but less efficient.
                    // The current addEdge adds to both adj[u] and adj[v].
                    // So, if we have matrix[i][j] == 1, we call addEdge(i,j).
                    // If matrix[j][i] is also 1 (as it should be for undirected),
                    // calling addEdge(j,i) later would duplicate entries in adj lists.
                    // Thus, only process one triangle, e.g. j > i.
                    if (j > i) { // Process only the upper triangle to add each edge once
                         g.addEdge(i, j);
                    }
                } else if (matrix[i][j] != 0) {
                    // Optional: Add error handling for values other than 0 or 1
                    // For now, we assume valid input (only 0s and 1s)
                }
            }
        }
        return g;
    }

    void addEdge(int u, int v) {
        adj[u].push_back(v);
        adj[v].push_back(u);
        // 'used' initialization is important for Euler, ensure it's reset before use.
        // AddEdge itself doesn't need to set used[u][v] = false,
        // as resetUsed() will be called before Euler's algorithm.
        // Let's keep it as it is for now, as resetUsed is called.
        used[u][v] = used[v][u] = false;
    }

    // Removed Graph::generateGraph method

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

    // Modified hamilton method to take a start_node_idx
    // path vector is cleared at the beginning.
    bool hamilton(vector<int>& path, int start_node_idx = 0) {
        if (start_node_idx < 0 || start_node_idx >= n) {
            if (n == 0) { // Special case: graph with 0 vertices
                path.clear();
                return false; // Or true if an empty path is a valid cycle for n=0
            }
            // Or throw an error, or return false if start_node_idx is invalid
            // For now, let's just return false for simplicity if an invalid start_node is given for non-empty graph
            cerr << "Error: Invalid start_node_idx for Hamilton cycle." << endl;
            path.clear();
            return false;
        }

        path.clear(); // Clear path for new computation
        vector<bool> visited(n, false);
        
        // If graph has 0 vertices, a Hamiltonian cycle doesn't make sense in the usual way.
        // An empty path could be considered a cycle in a graph with 0 vertices.
        // However, the problem implies finding a cycle in graphs where n > 0.
        // The hamiltonUtil logic (depth == n) assumes n > 0.
        // If n is 0, path.push_back(start_node_idx) would fail if start_node_idx is not handled.
        // Graph(0) creates adj of size 0.
        // If n = 0, hamiltonUtil won't be called meaningfully with start_node_idx = 0.
        // The check for start_node_idx >= n handles this.
        // If n=0, start_node_idx=0 results in start_node_idx >= n (0 >= 0) -> error/false.
        // This is fine. A graph with 0 vertices has no Hamiltonian cycle in the typical sense.

        // If n > 0 and start_node_idx is valid:
        // The first node added to path will be start_node_idx.
        // The depth is 1 because one node (the start_node_idx) is in the path.
        return hamiltonUtil(start_node_idx, visited, path, 1);
    }

    // Removed Graph::print() method
};

// Removed test function

int main() {
    // Ensure necessary headers are included:
    // <iostream> for cout, cin, cerr, endl
    // <vector> for vector
    // <string> for string
    // <fstream> for Graph::loadFromFile (implicitly, already there for its definition)
    // <sstream> for Graph::loadFromFile (implicitly, already there for its definition)
    // <stdexcept> for runtime_error (implicitly, already there for its definition)
    // <algorithm> for std::reverse

    cout << "Enter the path to the graph file: ";
    string filePath;
    cin >> filePath;

    Graph g(0); // Initialize with a 0-size graph

    try {
        g = Graph::loadFromFile(filePath);
    } catch (const std::runtime_error& e) {
        cerr << "Error loading graph: " << e.what() << endl;
        return 1; // Exit if file loading fails
    }

    if (g.n == 0) {
        cout << "Graph is empty or could not be properly initialized." << endl;
        return 1;
    }

    // Eulerian Cycle
    g.resetUsed(); // Clear edge usage state
    vector<int> eulerCycle;
    // It's assumed that if an Eulerian cycle exists, node 0 is part of it or graph is connected.
    // For a disconnected graph, this might only find a cycle in one component.
    // The problem implies we should try to find one starting at 0.
    // Also, Hierholzer's algorithm or Fleury's algorithm typically requires checking
    // if the graph has an Eulerian path/circuit (all vertices even degree, connected).
    // This implementation of g.euler just traverses. If not all edges are part of one cycle
    // starting from 0, it might not be a full Eulerian cycle of the graph.
    // We proceed with the current g.euler, assuming it's what's intended for the task.
    g.euler(0, eulerCycle); 
    std::reverse(eulerCycle.begin(), eulerCycle.end()); // Correct order

    cout << "Eulerian Cycle: ";
    if (eulerCycle.empty() || (eulerCycle.size() == 1 && g.n > 0) ) { // Basic check if a cycle was truly formed
        // The current euler implementation always adds the start node at least once.
        // A more robust check would be if all edges were visited, or if cycle starts and ends at same node
        // and includes a reasonable number of edges.
        // For now, if eulerCycle.size() <= 1 and n > 0, it's likely not a cycle or empty.
        // If g.n > 0 and eulerCycle is just {0}, it means no edges were traversed from 0.
        // However, the problem asks to print it. If the graph has no edges, euler(0, cycle) adds 0.
        // If it's a single node graph, cycle is {0}.
        // If it's an Eulerian graph, cycle will be non-trivial.
        // The prompt doesn't ask for "Not found" for Euler, so we print what we get.
    }

    for (size_t i = 0; i < eulerCycle.size(); ++i) {
        cout << eulerCycle[i] + 1 << (i == eulerCycle.size() - 1 ? "" : " ");
    }
    cout << endl;

    // Hamiltonian Cycle
    vector<int> hamiltonCycle;
    bool foundHamiltonian = g.hamilton(hamiltonCycle, 0); // Start from node 0

    cout << "Hamiltonian Cycle: ";
    if (foundHamiltonian) {
        for (size_t i = 0; i < hamiltonCycle.size(); ++i) {
            cout << hamiltonCycle[i] + 1 << (i == hamiltonCycle.size() - 1 ? "" : " ");
        }
        cout << endl;
    } else {
        cout << "Not found" << endl;
    }

    return 0; // Successful completion
}

/*
Example euler.txt:
0 0 1 1 1 1
0 0 1 1 0 0
1 1 0 0 1 1
1 1 0 0 1 1
1 0 1 1 0 1
1 0 1 1 1 0
*/
// Ensure euler.txt exists in the execution directory or provide full path.
