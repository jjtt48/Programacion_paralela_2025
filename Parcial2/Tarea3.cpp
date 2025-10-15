#include <iostream>
#include <string>
using namespace std;

void listAvailable(const string &board) {
    int pos = board.find('#');
    int row = pos / 4;
    int col = pos % 4;

    if (row > 0) cout << "UP" << endl;
    if (row < 3) cout << "DOWN" << endl;
    if (col > 0) cout << "LEFT" << endl;
    if (col < 3) cout << "RIGHT" << endl;
}

int main() {
    string board;
    cin >> board;
    listAvailable(board);
    return 0;
}

