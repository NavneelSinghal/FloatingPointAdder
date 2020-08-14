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
} fl;

// float to string
string internal_float (float f) {
    fl.f = f;
    bitset<32> b(fl.n);
    return b.to_string();
}

// convert a string to a float
float conv_to_float (const string& s) {
    fl.n = bitset<32>(s).to_ulong();
    return fl.f;
}

int conv_to_int (const string& s) {
    return bitset<32>(s).to_ulong();
}

// print an arbitrary number of floats
void print_float () {}
template <typename... Types>
void print_float (string s, Types... args) {
    cout << s << ": " << conv_to_float(s) << ", " << internal_float(conv_to_float(s)) << '\n';
    print_float(args...);
}

// return a code for each of nan, inf, -inf, normal number
// 0 if normal or denormal
// 1 if inf
// -1 if -inf
// 2 if nan

int check_corner (int sign, int exponent, int significand) {
    if (exponent == 255) {
        if (significand == (1 << 26)) {
            return sign;
        }
        return 2;
    }
    return 0;
}

string stitch (int sign, int exponent, int significand) {
    string s;
    s += '0' + (sign == -1);
    s += bitset<8>(exponent).to_string();
    if (exponent) significand -= 1 << 27;
    else significand >>= 1;
    s += bitset<23>(significand >> 3).to_string();
    return s;
}

#define fastio ios_base::sync_with_stdio(0);cin.tie(0);cout.tie(0);

int main (int argc, char* argv[]) {

    fastio;
    cout << setprecision(100) << fixed;

    if (argc > 1) {
        freopen(argv[1], "r", stdin);
    } else {
        cerr << "Pass an input file name to the program as the first command-line argument\n";
        return 0;
    }

    string s, t;

    while (cin >> s) {
        cin >> t;

        int cycles = 0;

        // prefix | meaning
        // s      | first operand
        // t      | second operand
        // r      | result
        int s_int, s_exponent, s_sign, s_significand, s_type;
        int t_int, t_exponent, t_sign, t_significand, t_type;
        int r_int, r_exponent, r_sign, r_significand, r_type;

        s_int = conv_to_int(s);
        t_int = conv_to_int(t);

        s_exponent = (s_int >> 23) & ((1 << 8) - 1);
        t_exponent = (t_int >> 23) & ((1 << 8) - 1);

        s_sign = (s_int >> 31) ? -1 : 1;
        t_sign = (t_int >> 31) ? -1 : 1;

        // handling the different cases based on exponent
        // shifting by 3 because of guard, round and sticky bits
        if (s_exponent == 0) s_significand = (s_int & ((1 << 23) - 1)) << 3;
        else s_significand = ((1 << 23) | (s_int & ((1 << 23) - 1))) << 3;

        if (t_exponent == 0) t_significand = (t_int & ((1 << 23) - 1)) << 3;
        else t_significand = ((1 << 23) | (t_int & ((1 << 23) - 1))) << 3;

        // handling denormal numbers
        if (s_significand != 0 && s_exponent == 0) s_exponent = 1;
        if (t_significand != 0 && t_exponent == 0) t_exponent = 1;

        t_type = check_corner(t_sign, t_exponent, t_significand);
        s_type = check_corner(s_sign, s_exponent, s_significand);

        r_type = 0;
        if (t_type + s_type == 0 && s_type != 0) { // inf - inf
            r_type = 2; // nan
        }
        else if (t_type == 2 || s_type == 2) { // any one is nan
            r_type = 2; // nan
        }
        else if (t_type + s_type != 0) { // at least one is inf of some sort, and it is not inf, -inf
            if (t_type != 0) 
                r_type = t_type;
            else 
                r_type = s_type;
        }

        // step 1
        if (s_exponent < t_exponent) {
            r_exponent = t_exponent;
            while (s_exponent < t_exponent) {
                s_exponent++;
                s_significand = (s_significand & 1) | (s_significand >> 1);
            }
        } else {
            r_exponent = s_exponent;
            while (t_exponent < s_exponent) {
                t_exponent++;
                t_significand = (t_significand & 1) | (t_significand >> 1);
            }
        }
        cycles++;

        // step 2
        r_significand = t_sign * t_significand + s_sign * s_significand; 
        r_sign = (r_significand < 0) ? -1 : 1;
        r_significand *= r_sign;

        cycles++;

        // step 3
        while (true) {

            while (r_exponent > 0 && (r_significand < (1 << 26))) {
                r_significand <<= 1;
                --r_exponent;
            }

            while (r_exponent < 255 && r_significand >= (1 << 27)) {
                r_significand = (r_significand & 1) | (r_significand >> 1);
                ++r_exponent;
            }

            cycles++;

            // now check for 0, denormal number, inf, nan
            // possible corner case : when the significand becomes (1 << 27) - 1 and we need to round up
            // first three cases are due to input issues

            if (r_type == 1) {
                cerr << "inf\n";
                r_sign = 1;
                r_exponent = 255;
                r_significand = (1 << 26);
                break;
            } else if (r_type == -1) {
                cerr << "-inf\n";
                r_sign = -1;
                r_exponent = 255;
                r_significand = (1 << 26);
                break;
            } else if (r_type == 2) {
                cerr << "nan\n";
                r_sign = 1;
                r_exponent = 255;
                r_significand = (1 << 27) - 1;
                break;
            }

            // these two cases are due to stuff getting detected here in this stage

            else if (r_exponent == 0) {
                if (r_significand != 0) {
                    cerr << "denormal\n";
                    break;
                }
            } else if (r_exponent == 255) {
                if (r_significand >= (1 << 26)) {
                    r_significand = 1 << 26;
                    cerr << ((r_sign == -1) ? "-" : "") << "inf\n";
                    break;
                } else {
                    assert(false);
                }
                break;
            }

            // rounding mechanism, checks the guard, round and sticky bits

            int round = r_significand & 7;

            r_significand >>= 3;

            if (round > 4) {
                r_significand++;
            } else if (round == 4) {
                r_significand += (r_significand & 1);
            }

            r_significand <<= 3;

            cycles++;

            // checking for normalisation
            if (r_significand < (1 << 27)) {
                break;
            }

        }

        string s_rep, t_rep, r_rep;
        s_rep = stitch(s_sign, s_exponent, s_significand);
        t_rep = stitch(t_sign, t_exponent, t_significand);
        r_rep = stitch(r_sign, r_exponent, r_significand);

        // correctness testing mechanism

        float s_float = conv_to_float(s);
        float t_float = conv_to_float(t);
        float r_float = conv_to_float(r_rep);
        float r_actual = s_float + t_float;

        if (!isnan(r_actual)) {
            if (r_actual != r_float) {
                cout << "s is " << s << endl;
                cout << "t is " << t << endl;
                cout << internal_float(r_actual) << endl;
                cout << r_rep << endl;
                cout << cycles << endl;
            } 
            assert(r_actual == r_float);
            assert(r_rep == internal_float(r_actual));
        } else {
            assert (r_type == 2);
            assert (r_rep == "01111111111111111111111111111111");
        }

        cout << r_rep << " " << cycles << '\n';
    }

    return 0;

}
