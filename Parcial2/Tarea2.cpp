#include <iostream>
#include <vector>
using namespace std;

void print_board(vector<vector<char>> board) {
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            cout << board[i][j];
            if (j < 3) cout << " ";
        }
        if (i < 4) cout << endl;
    }
}

void doMove(vector<vector<char>> &board, string move) {
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            if (board[i][j] == '#') {
                if (move == "DOWN" && i < 3) {
                    char temp = board[i + 1][j];
                    board[i][j] = temp;
                    board[i + 1][j] = '#';
                } else if (move == "UP" && i > 0) {
                    char temp = board[i - 1][j];
                    board[i][j] = temp;
                    board[i - 1][j] = '#';
                } else if (move == "RIGHT" && j < 3) {
                    char temp = board[i][j + 1];
                    board[i][j] = temp;
                    board[i][j + 1] = '#';
                } else if (move == "LEFT" && j > 0) {
                    char temp = board[i][j - 1];
                    board[i][j] = temp;
                    board[i][j - 1] = '#';
                }
                print_board(board);
                return; 
            }
        }
    }
}

int main() {
    vector<vector<char>> board;
    string in;
    string move;
    cin >> in;
    cin >> move;

    for (int r = 0; r < 4; ++r) {
        vector<char> row;
        for (int i = 0; i < 4; i++) {
            row.push_back(in[4 * r + i]);
        }
        board.push_back(row);
    }
    doMove(board, move);
    return 0;
}
