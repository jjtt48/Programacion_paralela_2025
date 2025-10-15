#include <iostream>
#include <queue>
#include <unordered_set>
#include <vector>
#include <string>
#include <cmath>
#include <omp.h>
#include <chrono>
using namespace std::chrono;
using namespace std;

int N;
string TARGET;

string buildTarget(const string &start) {
    string target;
    bool isNumber = isdigit(start[0]);
    int total = N * N;

    if (isNumber) {
        for (int i = 1; i < total; i++)
            target += to_string(i)[0];
    } else {
        for (int i = 0; i < total - 1; i++)
            target += 'A' + i;
    }
    target += '#';
    return target;
}

int heuristic(const string &state) {
    int h = 0;
    int total = N * N;
    #pragma omp parallel for reduction(+:h)
    for (int i = 0; i < total; i++) {
        if (state[i] != '#' && state[i] != TARGET[i])
            h++;
    }
    return h;
}

vector<string> getNeighbors(const string &state) {
    vector<string> neighbors;
    int pos = state.find('#');
    int row = pos / N, col = pos % N;

    const int dr[4] = {-1, 1, 0, 0};
    const int dc[4] = {0, 0, -1, 1};

    vector<string> local(4);
    int count = 0;

    #pragma omp parallel for
    for (int k = 0; k < 4; k++) {
        int nr = row + dr[k], nc = col + dc[k];
        if (nr >= 0 && nr < N && nc >= 0 && nc < N) {
            int npos = nr * N + nc;
            string next = state;
            swap(next[pos], next[npos]);
            #pragma omp critical
            neighbors.push_back(next);
        }
    }
    return neighbors;
}

int aStarSearch(string start) {
    struct Node {
        string state;
        int g, f;
        bool operator>(const Node &other) const {
            return f > other.f;
        }
    };

    priority_queue<Node, vector<Node>, greater<Node>> pq;
    unordered_set<string> visited;
    int expandedNodes = 0;
    pq.push({start, 0, heuristic(start)});
    visited.insert(start);

    while (!pq.empty()) {
        Node current = pq.top();
        pq.pop();
        expandedNodes++;
        if (current.state == TARGET){
            cout << "Nodos expandidos: " << expandedNodes << endl;
            return current.g;
        }
        vector<string> neighbors = getNeighbors(current.state);

        for (const string &next : neighbors) {
            if (!visited.count(next)) {
                visited.insert(next);
                int g2 = current.g + 1;
                int f2 = g2 + heuristic(next);
                pq.push({next, g2, f2});
            }
        }
    }
    cout << "Nodos expandidos: " << expandedNodes << endl;
    return -1;
}

int main() {
    string start;
    cin >> N >> start;

    if ((int)start.size() != N * N)
        return 1;

    TARGET = buildTarget(start);
    //omp_set_num_threads(16);
    auto t1 = high_resolution_clock::now();
    cout << aStarSearch(start) << endl;
    auto t2 = high_resolution_clock::now();
    duration<double> elapsed = t2 - t1;
    cout << "Tiempo de ejecución: " << elapsed.count() << " segundos" << endl;
    return 0;
}
