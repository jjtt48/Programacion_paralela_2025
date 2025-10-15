#include <iostream>
#include <queue>
#include <unordered_set>
#include <chrono>
using namespace std;
using namespace std::chrono;

const int dRow[] = {-1, 1, 0, 0};
const int dCol[] = {0, 0, -1, 1};

const string GOAL = "ABCDEFGHIJKLMNO#"; 

struct State {
    string board;   
    int blankPos;   
    int cost;       

    State(string b, int pos, int c) : board(b), blankPos(pos), cost(c) {}
};

string swapBoardTiles(const string &currentBoard, int position1, int position2) {
    string newBoard = currentBoard;
    swap(newBoard[position1], newBoard[position2]);
    return newBoard;
}

int bfs(string start) {
    queue<State> q;
    unordered_set<string> visited;
    int expanded = 0;
    int blankPos = start.find('#');
    q.push(State(start, blankPos, 0));
    visited.insert(start);

    while (!q.empty()) {
        State current = q.front();
        q.pop();
        expanded++;
        if (current.board == GOAL){ 
            cout << "Nodos expandidos: " << expanded << endl;
            return current.cost;
        }

        int row = current.blankPos / 4;
        int col = current.blankPos % 4;

        for (int i = 0; i < 4; i++) {
            int newRow = row + dRow[i];
            int newCol = col + dCol[i];

            if (newRow >= 0 && newRow < 4 && newCol >= 0 && newCol < 4) {
                int newPos = newRow * 4 + newCol;
                string newBoard = swapBoardTiles(current.board, current.blankPos, newPos);

                if (visited.find(newBoard) == visited.end()) {
                    q.push(State(newBoard, newPos, current.cost + 1));
                    visited.insert(newBoard);
                }
            }
        }
    }
    cout << "Nodos expandidos: " << expanded << endl;
    return -1;
}

int main() {
    string start;
    cin >> start;
    auto t1 = high_resolution_clock::now();
    int result = bfs(start);
    auto t2 = high_resolution_clock::now();
    duration<double> elapsed = t2 - t1;
    cout << "Tiempo de ejecución: " << elapsed.count() << " segundos" << endl;
    if (result == -1)
        cout << "UNSOLVABLE" << endl; 
    else
        cout << result << endl;        

    return 0;
}
