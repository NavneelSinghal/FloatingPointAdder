//TODO: debug
//TODO: fix rounding
//TODO: fix the conv_to_float function
#include <iostream>
#include <iomanip>
#include <string>
#include <bitset>
#include <cmath>
#include <cassert>

using namespace std;

/*
 * Sign
 */

// find the sign of a binary string
int sign (const string& s) {
    if (s[0] == '0') {
        return 1;
    } else {
        return -1;
    }
}

/*
 * Exponent
 */

// find the exponent of a floating point number
int extract_exponent (const string& s) {
    bitset<8> b(s.substr(1, 8));
    return static_cast<int>(b.to_ulong());
}

// convert exponent from int to string
string int_to_string_exponent (const int exponent) {
    bitset<8> b(exponent);
    return b.to_string();
}

/*
 * Significand
 */

// find the binary value of the significand of a floating point number in its floating point representation
string extract_significand (const string& s) {
    if (extract_exponent(s) == 0) return "0" + s.substr(9); // handles significand of 0 and denormal numbers
    return "1" + s.substr(9);
}

// find the integer value of the significand of a floating point number in its floating point representation
int int_significand (const string& s) {
    string w = extract_significand(s);
    assert(w.size() == 24);
    bitset<24> b(w);
    return static_cast<int>(b.to_ulong());
}

// convert significand from int to string
string int_to_string_significand (const int significand) {
    bitset<24> b(significand);
    return b.to_string();
}

/*
 * Floating point number
 */

// stitch together a sign, exponent and a fraction into floating point representation
string stitch (const int sign, const string& exponent, const string& fraction) {
    string s = "0";
    if (sign == -1) s[0] = '1';
    s += exponent;
    s += fraction;
    return s;
}

/*
 * debugging
 */

union floating {
    unsigned n;
    float f;
};

string internal_float (float f) {
    floating fl;
    fl.f = f;
    bitset<32> b(fl.n);
    return b.to_string();
}

// convert a string to a float
float conv_to_float (const string& s) {
    float f = 1.0;
    f *= sign(s);
    // pow is exact for the argument being 2
    f *= int_significand(s) / pow(2.0, 23);
    // again the same reason as above
    f *= pow(2.0, extract_exponent(s) - 127);
    return f;
}

// print an arbitrary number of floats
void print_float () {}
template <typename... Types>
void print_float (string s, Types... args) {
    cout << s << ": " << conv_to_float(s) << ", " << internal_float(conv_to_float(s)) << endl;
    print_float(args...);
}

// return a code for each of nan, inf, -inf, normal number
// 0 if normal or denormal
// 1 if inf
// -1 if -inf
// 2 if nan

int check_corner (int sign, int exponent, int significand) {
    if (exponent == 255) {
        if (significand == (1 << 24) - 1) {
            return sign;
        }
        return 2;
    }
    return 0;
}

bool check (string s, string t, string res) {
    //check corner cases first
    //inf, nan etc
    //now do the following
    float fs = conv_to_float(s);
    float ft = conv_to_float(t);
    float ff = fs + ft;
    return res == internal_float(ff);
}

int main (int argc, char* argv[]) {
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
        int cycles = 0;

        // print_float(s, t);

        int s_significand = int_significand(s);
        int t_significand = int_significand(t);
        int s_exponent = extract_exponent(s);
        int t_exponent = extract_exponent(t);
        int s_sign = sign(s);
        int t_sign = sign(t);

        int t_type = check_corner(t_sign, t_exponent, t_significand);
        int s_type = check_corner(s_sign, s_exponent, s_significand);

        int res_type = 0; // normal
        if (t_type + s_type == 0 && s_type != 0) { // inf - inf
            res_type = 2; // nan
        }
        else if (t_type == 2 || s_type == 2) { // any one is nan
            res_type = 2; // nan
        }
        else if (t_type + s_type != 0) { // at least one is inf of some sort, and it is not inf, -inf
            if (t_type != 0) 
                res_type = t_type;
            else 
                res_type = s_type;
        }

        int result_exponent;

        // TODO: fix rounding
        // and check what happens to negative numbers

        int prev = 0;
        int out_of = 1;

        // step 1
        // try to write it without the loop, and put additional guards which limit prev to 25 bits (maybe make prev a string of a fixed size)
        if (s_exponent < t_exponent) {
            result_exponent = t_exponent;
            while (s_exponent < t_exponent) {
                s_exponent++;
                prev = (prev << 1) | (s_significand & 1);
                out_of <<= 1;
                s_significand >>= 1;
            }
            if (prev) {
                if (s_sign == 1 && t_sign == -1) { // bigger is negative
                    s_significand++;
                    prev = out_of - prev;
                } else if (s_sign == -1 && t_sign == 1) { // smaller is negative
                    s_significand--;
                    prev = out_of - prev;
                }
            }
        } else if (t_exponent < s_exponent) {
            result_exponent = s_exponent;
            while (t_exponent < s_exponent) {
                t_exponent++;
                prev = (prev << 1) | (t_significand & 1);
                out_of <<= 1;
                t_significand >>= 1;
            }
            if (prev) {
                if (s_sign == 1 && t_sign == -1) { // bigger is negative
                    s_significand++;
                    prev = out_of - prev;
                } else if (s_sign == -1 && t_sign == 1) { // smaller is negative
                    s_significand--;
                    prev = out_of - prev;
                }
            }
        }
        cycles++;

        // step 2
        int result_significand = t_sign * t_significand + s_sign * s_significand; //t_significand + s_significand;
        int result_sign = (result_significand < 0) ? -1 : 1;
        result_significand *= result_sign;

        // cout << s_sign << " " << s_exponent << " " << s_significand << endl;
        // cout << t_sign << " " << t_exponent << " " << t_significand << endl;
        // cout << result_sign << " " << result_exponent << " " << result_significand << endl;

        cycles++;

        while (true) {

            // check whether this loop is okay or not

            while (result_exponent > 0 && result_significand < (1 << 23)) { //check whether 1 << 24 or something else
                // cout << "reducing exponent\n";
                prev >>= 1;
                out_of >>= 1;
                result_significand <<= 1;
                --result_exponent;
            }

            while (result_exponent < 255 && result_significand >= (1 << 24)) {
                // cout << "increasing exponent\n";
                prev = (prev << 1) | result_significand & 1;
                out_of <<= 1;
                result_significand >>= 1;
                ++result_exponent;
            }

            cycles++;

            // now check for 0, denormal number, inf, nan

            // possible corner case : when the significand becomes (1 << 24) - 1 and we need to round up

            // first four cases are due to input issues
            if (res_type == 1) {
                cerr << "inf\n";
                result_sign = 1;
                result_exponent = 255;
                result_significand = (1 << 24) - 1;
                break;
            } else if (res_type == -1) {
                cerr << "-inf\n";
                result_sign = -1;
                result_exponent = 255;
                result_significand = (1 << 24) - 1;
                break;
            } else if (res_type == 2) {
                cerr << "nan\n";
                result_sign = 1;
                result_exponent = 255;
                result_significand = (3 << 22) - 1;
                break;
            } 

            // these two cases are due to stuff getting detected here in this stage

            else if (result_exponent == 0) {
                if (result_significand != 0) {
                    cerr << "denormal (underflow)\n";
                    break;
                }
            } else if (result_exponent == 255) {
                cerr << "NaN or inf\n";
                break;
            }


            // now round - if prev is not exactly halfway, we round to nearest, else we round to the nearest even
            
            // cout << "prev: " << prev << endl \
                 << "out_of: " << out_of << endl;

            if (prev) {
                if (prev < (out_of >> 1)) {}
                // for the else condition, you can replace it with else {result_significand += (prev < (out_of << 1) || (result_significand & 1));}
                else if (prev > (out_of >> 1)) {
                    ++result_significand;
                }
                else {
                    if (result_significand & 1) {
                        ++result_significand;
                    }
                }
            }

            cycles++;

            // now check if normalised
            if (result_significand < (1 << 24)) {
                break;
            }

        }

        // cout << s_sign << " " << s_exponent - 127 << " " << s_significand << endl;
        // cout << t_sign << " " << t_exponent - 127 << " " << t_significand << endl;
        // cout << result_sign << " " << result_exponent - 127 << " " << result_significand << endl;

        string ans = stitch(result_sign, int_to_string_exponent(result_exponent), int_to_string_significand(result_significand).substr(1));

        //debug
        print_float(s, t, ans);

        if (check(s, t, ans)) cout << "YES\n";
        else cout << "NO\n";

        cout << ans << " " << cycles << endl;
    }

    return 0;

}
