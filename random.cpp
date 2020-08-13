#include <iostream>

using namespace std;

union floating {
    unsigned n;
    float f;
};

int main () {
    float f = 0.8134813409834;
    floating fl;
    fl.f = f;
    cout << fl.n << endl;
}

