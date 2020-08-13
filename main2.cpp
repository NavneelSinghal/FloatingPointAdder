#include <iostream>
#include <iomanip>
#include <string>
#include <bitset>
#include <cmath>
#include <cassert>

using namespace std;

union floating {
    unsigned n;
    float f;
};

#define fastio ios_base::sync_with_stdio(0);cin.tie(0);cout.tie(0);

int main (int argc, char* argv[]) {

    fastio;
    cout << setprecision(100) << fixed;
    
    // input redirection in case a file is specified
    if (argc > 1) {
        freopen(argv[1], "r", stdin);
    } else {
        cerr << "Pass an input file name to the program as the first command-line argument\n";
        return 0;
    }

    string s, t;
    
    while (cin >> s) {
    
        cin >> t;

        floating sf, tf, rf;
        sf.n = bitset<32>(s).to_ulong();
        tf.n = bitset<32>(t).to_ulong();
        rf.f = sf.f + tf.f;
        s = bitset<32>(rf.n).to_string();

        cout << s << '\n';
    }

    return 0;

}
