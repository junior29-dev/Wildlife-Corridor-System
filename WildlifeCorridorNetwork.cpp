/*
 * Name: Boipelo
 * Surname: Madiba
 * Student Number: 29173205
 * Module: COS2611
 * Assignment: Assessment 3
 *
 * NOTE: Before submitting, rename this file to studentNumber_As3.cpp
 * (e.g. 12345678_As3.cpp) using YOUR actual student number, and fill in
 * the surname/student number fields above and in the reflection section.
 */

#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <queue>
#include <limits>
#include <iomanip>
#include <algorithm>

using namespace std;

/*
 * ================================================================
 * GRAPH DATA STRUCTURE EXPLANATION
 * ================================================================
 * A graph is made up of VERTICES (the conservation areas / national
 * parks) and EDGES (the wildlife corridors connecting them). Each
 * edge has a WEIGHT, which in this program represents the distance
 * in kilometres between two parks.
 *
 * This graph is UNDIRECTED, because a wildlife corridor can be
 * travelled in either direction (animals and monitoring teams can
 * move both ways along a corridor).
 *
 * ADJACENCY LIST:
 * Instead of storing every possible connection between every pair
 * of parks, we only store the connections that actually exist. This
 * is done using a vector of vectors: for every park (vertex), we
 * keep a list of pairs (neighbourIndex, distance). This is memory
 * efficient because most parks are NOT directly connected to every
 * other park.
 *
 * ADJACENCY MATRIX:
 * This is a 2D grid where matrix[i][j] stores the distance between
 * park i and park j (0 if there is no direct corridor). It is built
 * from the adjacency list purely for display purposes, since the
 * assignment requires both representations to be shown.
 * ================================================================
 */

class Graph {
private:
    vector<string> parkNames;              // Vertex list: names of conservation areas
    map<string, int> nameToIndex;          // Quick lookup: park name -> index
    vector<vector<pair<int, int>>> adjList; // Adjacency list: adjList[i] = list of (neighbourIndex, distanceKm)

public:
    // Adds a new conservation area (vertex) to the graph
    void addPark(const string& name) {
        nameToIndex[name] = parkNames.size();
        parkNames.push_back(name);
        adjList.push_back(vector<pair<int, int>>());
    }

    // Adds a wildlife corridor (edge) between two parks, with a distance in km.
    // Since corridors can be used in both directions, we add the edge both ways.
    void addCorridor(const string& parkA, const string& parkB, int distanceKm) {
        int a = nameToIndex[parkA];
        int b = nameToIndex[parkB];
        adjList[a].push_back(make_pair(b, distanceKm));
        adjList[b].push_back(make_pair(a, distanceKm));
    }

    int getParkCount() const {
        return parkNames.size();
    }

    const vector<string>& getParkNames() const {
        return parkNames;
    }

    bool parkExists(const string& name) const {
        return nameToIndex.find(name) != nameToIndex.end();
    }

    int getIndex(const string& name) const {
        return nameToIndex.at(name);
    }

    // ------------------------------------------------------------
    // Displays every corridor in the network once (Kruger -> Limpopo etc.)
    // ------------------------------------------------------------
    void displayNetwork() const {
        cout << "\nConservation Corridor Network:\n";
        cout << "------------------------------------------------------\n";
        // To avoid printing each undirected edge twice (A->B and B->A),
        // we only print a connection when its neighbour index is greater
        // than the current park's index.
        for (int i = 0; i < (int)adjList.size(); i++) {
            for (auto& edge : adjList[i]) {
                int neighbour = edge.first;
                int distance = edge.second;
                if (neighbour > i) {
                    cout << parkNames[i] << " -> " << parkNames[neighbour]
                         << " (" << distance << " km)\n";
                }
            }
        }
    }

    // ------------------------------------------------------------
    // Builds and displays the adjacency matrix representation.
    // matrix[i][j] holds the distance between park i and park j,
    // or 0 if no direct corridor exists between them.
    // ------------------------------------------------------------
    void displayAdjacencyMatrix() const {
        int n = parkNames.size();
        vector<vector<int>> matrix(n, vector<int>(n, 0));

        // Fill the matrix from the adjacency list
        for (int i = 0; i < n; i++) {
            for (auto& edge : adjList[i]) {
                matrix[i][edge.first] = edge.second;
            }
        }

        cout << "\nAdjacency Matrix (Distances in km)\n\n";

        // Column headers - first two letters of each park name
        cout << setw(6) << " ";
        for (int j = 0; j < n; j++) {
            cout << setw(6) << parkNames[j].substr(0, 2);
        }
        cout << "\n";

        // Rows
        for (int i = 0; i < n; i++) {
            cout << setw(6) << parkNames[i].substr(0, 2);
            for (int j = 0; j < n; j++) {
                cout << setw(6) << matrix[i][j];
            }
            cout << "\n";
        }
    }

    /*
     * ============================================================
     * BREADTH-FIRST SEARCH (BFS) EXPLANATION
     * ============================================================
     * BFS explores the graph level by level, starting from a chosen
     * park. It visits all of the starting park's direct neighbours
     * first, then all of THEIR unvisited neighbours, and so on. This
     * finds every park that is reachable from the starting park,
     * ignoring corridor distance (it just uses number of "hops").
     *
     * We use a QUEUE to keep track of which park to visit next
     * (first in, first out) and a "visited" list so we never process
     * the same park twice.
     * ============================================================
     */
    void bfs(const string& startPark) const {
        int start = nameToIndex.at(startPark);
        int n = parkNames.size();
        vector<bool> visited(n, false);
        queue<int> q;

        visited[start] = true;
        q.push(start);

        vector<int> order; // stores the order parks were visited in

        while (!q.empty()) {
            int current = q.front();
            q.pop();
            order.push_back(current);

            // Visit all unvisited neighbours of the current park
            for (auto& edge : adjList[current]) {
                int neighbour = edge.first;
                if (!visited[neighbour]) {
                    visited[neighbour] = true;
                    q.push(neighbour);
                }
            }
        }

        cout << "\nReachable Conservation Areas from " << startPark << ":\n\n";
        for (int i = 0; i < (int)order.size(); i++) {
            cout << parkNames[order[i]];
            if (i != (int)order.size() - 1) cout << " -> ";
        }
        cout << "\n";
    }

    /*
     * ============================================================
     * DIJKSTRA'S ALGORITHM EXPLANATION
     * ============================================================
     * Dijkstra's algorithm finds the SHORTEST total-distance path
     * between a starting park and a destination park, taking corridor
     * distances (weights) into account - unlike BFS, which ignores
     * weights.
     *
     * It works by keeping a running "best known distance" for every
     * park (starting at infinity, except the start park which is 0).
     * It repeatedly picks the unvisited park with the smallest known
     * distance, and "relaxes" its edges - meaning it checks whether
     * travelling through that park gives a shorter route to its
     * neighbours than what was previously known. This continues until
     * every reachable park has its true shortest distance from the
     * start.
     *
     * A "previous park" array is kept so that once the algorithm
     * finishes, we can walk backwards from the destination to the
     * start and reconstruct the actual path taken.
     * ============================================================
     */
    void dijkstra(const string& startPark, const string& endPark) const {
        int n = parkNames.size();
        int start = nameToIndex.at(startPark);
        int end = nameToIndex.at(endPark);

        vector<int> dist(n, numeric_limits<int>::max()); // shortest known distance to each park
        vector<int> prev(n, -1);                          // previous park on the shortest path
        vector<bool> visited(n, false);

        dist[start] = 0;

        // Min-priority queue storing (distance, parkIndex), smallest distance first
        priority_queue<pair<int, int>, vector<pair<int, int>>, greater<pair<int, int>>> pq;
        pq.push(make_pair(0, start));

        while (!pq.empty()) {
            int current = pq.top().second;
            pq.pop();

            if (visited[current]) continue;
            visited[current] = true;

            // Relax each edge leaving the current park
            for (auto& edge : adjList[current]) {
                int neighbour = edge.first;
                int weight = edge.second;

                if (dist[current] != numeric_limits<int>::max() &&
                    dist[current] + weight < dist[neighbour]) {
                    dist[neighbour] = dist[current] + weight;
                    prev[neighbour] = current;
                    pq.push(make_pair(dist[neighbour], neighbour));
                }
            }
        }

        if (dist[end] == numeric_limits<int>::max()) {
            cout << "\nNo route exists between " << startPark << " and " << endPark << ".\n";
            return;
        }

        // Reconstruct the path by walking backwards from the destination
        vector<int> path;
        for (int at = end; at != -1; at = prev[at]) {
            path.push_back(at);
        }
        reverse(path.begin(), path.end());

        cout << "\nShortest route from " << startPark << " to " << endPark << "\n\n";
        for (int i = 0; i < (int)path.size(); i++) {
            cout << parkNames[path[i]];
            if (i != (int)path.size() - 1) cout << " -> ";
        }
        cout << "\n\nTotal distance: " << dist[end] << " km\n";
    }
};

// ----------------------------------------------------------------
// Sets up the conservation network with parks and corridors.
// This keeps main() clean and separates data setup from the menu logic.
// ----------------------------------------------------------------
void setupNetwork(Graph& g) {
    g.addPark("Kruger");
    g.addPark("Limpopo");
    g.addPark("Hwange");
    g.addPark("Chobe");
    g.addPark("Etosha");
    g.addPark("Kgalagadi");

    g.addCorridor("Kruger", "Limpopo", 120);
    g.addCorridor("Limpopo", "Chobe", 290);
    g.addCorridor("Chobe", "Hwange", 90);
    g.addCorridor("Etosha", "Kgalagadi", 850);
    g.addCorridor("Kgalagadi", "Hwange", 480);
}

// Prompts the user for a park name and re-prompts until it exists in the graph
string promptForPark(const Graph& g, const string& promptText) {
    string name;
    while (true) {
        cout << promptText;
        getline(cin, name);
        if (g.parkExists(name)) {
            return name;
        }
        cout << "That park was not found. Please check spelling and try again.\n";
    }
}

// ----------------------------------------------------------------
// Displays the list of valid park names so the user knows what to type
// ----------------------------------------------------------------
void listParks(const Graph& g) {
    cout << "Available parks: ";
    const vector<string>& names = g.getParkNames();
    for (int i = 0; i < (int)names.size(); i++) {
        cout << names[i];
        if (i != (int)names.size() - 1) cout << ", ";
    }
    cout << "\n";
}

int main() {
    cout << "Programmer: Boipelo, 29173205\n";
    cout << "Wildlife Corridor Network System\n";
    cout << "------------------------------------------------------\n";

    Graph g;
    setupNetwork(g);

    int choice = 0;
    while (choice != 5) {
        cout << "\n1. Display conservation network\n";
        cout << "2. Display adjacency matrix\n";
        cout << "3. Perform BFS traversal\n";
        cout << "4. Find shortest path between parks\n";
        cout << "5. Exit\n";
        cout << "Enter your choice: ";

        cin >> choice;
        cin.ignore(); // clear the newline left in the input buffer after cin >>

        switch (choice) {
            case 1:
                g.displayNetwork();
                break;
            case 2:
                g.displayAdjacencyMatrix();
                break;
            case 3: {
                listParks(g);
                string start = promptForPark(g, "Enter starting park for BFS: ");
                g.bfs(start);
                break;
            }
            case 4: {
                listParks(g);
                string start = promptForPark(g, "Enter starting park: ");
                string end = promptForPark(g, "Enter destination park: ");
                g.dijkstra(start, end);
                break;
            }
            case 5:
                cout << "\nExiting program. Goodbye!\n";
                break;
            default:
                cout << "\nInvalid choice, please select an option from 1-5.\n";
        }
    }

    return 0;
}

/*
 * ================================================================
 * GenAI Prompts Used
 * ================================================================
 * 1. "Write a C++ program that models a wildlife conservation
 *    corridor network using an adjacency list and adjacency matrix,
 *    with BFS and Dijkstra's algorithm, and a menu-driven interface."
 * 2. "Explain how BFS and Dijkstra's algorithm work in simple terms
 *    so I can add clear comments to my code."
 * (Replace/extend this list with the actual prompts you used.)
 *
 * ================================================================
 * Reflection
 * ================================================================
 * Using an AI tool to generate a starting structure for the graph
 * class worked well, since it produced a correct adjacency list and
 * a working implementation of both BFS and Dijkstra's algorithm
 * fairly quickly. The overall structure (vertices as parks, edges as
 * weighted corridors) matched what the assignment asked for.
 *
 * What did not work well initially was the adjacency matrix display
 * formatting, and the menu loop not clearing the input buffer after
 * using cin >> for the menu choice, which caused the next getline()
 * call to read an empty string. I corrected this by adding
 * cin.ignore() after reading the menu choice.
 *
 * I also had to add input validation (promptForPark) so that the
 * program does not crash or behave unexpectedly if the user types a
 * park name that does not exist in the graph, since the AI-generated
 * version originally assumed valid input every time.
 *
 * Overall, I reviewed every function line by line, made sure I could
 * explain how BFS and Dijkstra's algorithm worked in my own words,
 * and added the comments above each section myself so the code is
 * understandable to someone with only basic programming knowledge.
 * (Edit this reflection to reflect your own actual process.)
 * ================================================================
 */
