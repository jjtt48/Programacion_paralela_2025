#include <iostream>
#include <string>
using namespace std;

char rot(char c, int s) {
    if (c >= 'a' && c <= 'z') return char('a' + ( (c - 'a' + s) % 26 ));
    if (c >= 'A' && c <= 'Z') return char('A' + ( (c - 'A' + s) % 26 ));
    return c; 
}

int main(int argc, char* argv[]) {
    
    int s = stoi(string(argv[1]));
    s = ( (s % 26) + 26 ) % 26;

    string text;
    for (int i = 2; i < argc; ++i) {
        if (i > 2) text += ' ';
        text += argv[i];
    }

    for (char c : text) cout << rot(c, s);
    cout << '\n';
    return 0;
}

