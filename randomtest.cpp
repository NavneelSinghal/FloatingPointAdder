#include <bits/stdc++.h>
using namespace std;

#define TESTS 1000000

int main() {
    srand(time(NULL));
    for (int i = 0; i < 2 * TESTS; ++i) {
        string s = "";
        for (int j = 0; j < 32; j++) {
            s += char('0' + (rand() & 1));
        }
        cout << s << '\n';
    }
}
