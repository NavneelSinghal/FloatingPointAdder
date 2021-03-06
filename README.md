# Floating point adder

## Implementation:

The adder was implemented using the design given in the book, and all efforts have been made to keep the structure as close to that design as possible (for example, the only exit points of the floating point adder are either the exception generating unit, and the final result).

The binary representation of a floating point number is stored as it is in an integer variable. For example, if the floating point number is 01010101010101010101010101010101, then it will be stored in an integer whose binary representation is identical to this.

To carry out this representation change, we use the bitset data structure in C++, which is essentially an array of boolean values. The string is passed as a constructor to it, and the result is a bitset, which can be converted to an unsigned integer.

We then extract the sign, exponent, significand and type (type denotes whether the floating point number given to us is normal, denormal, inf, -inf or nan) for each input. All of these are stored in integers.

The representation of the sign and exponent are done in the standard way (sign is +1 or -1, and exponent can be anything between 0 and 255, with the special cases for each type of numbers being handled).

For the significand, we use the standard three bits - guard, round and sticky - as used by most processors which follow the IEEE standard.

So the significand is a 27 bit integer, whose top 24 bits are extracted from the integer representation of the floating point number, and the bottom three bits are the guard, round and sticky bits.

After all this extraction, we find the result's corresponding data.

To find the result type (type is as defined above), we use the types of the inputs. This result type will be used at the exit point at the exceptions block in the diagram mentioned in the book.

Now to compute the result sign, significand and exponent, we first align the two inputs. To carry this out, we keep shifting the one with the smaller exponent right (keeping the sticky bit 1 if there is ever a 1 at that position) till the exponents match.

The result sign can be easily found using integer addition/subtraction. Finding the result significand happens by shifting result significand so found while the exponent is valid and the result significand is out of the range.

Then we check for exceptions. There are two types of exceptions that we handle here, one of them arises from the input deduced types, and the other one is deduced from the values of the result exponent and significand as computed in the previous cycle. Note that we send the exception data to stderr instead of stdout, and the result is still sent to stdout (in order to maintain the output format).

Then we do rounding. For rounding, we check the last three bits of the result. If the number formed by them is > 4, we have to round up, and if it is < 4, then we don't have to round. If the number is exactly 4, we use the default rounding scheme as specified in the IEEE standard, which says that we need to round to the nearest number whose least significant bit is 0. This is implemented here by adding the least significant bit of the number to the number (after shifting).

Then we check if the result is normalised, and if it is not, we run the process again.

At this point, we have the correct sign, significand, exponent and type values for the result, which we stitch into a 32-bit binary string which we print.

To check for the correctness, we make use of the fact that the processor that we have is IEEE compliant, and hence the result should match with the result computed using the inbuilt floating point adder.

For this, we would need to convert a string to a float and back, and for that we exploit unions in C++. Since unions store the relevant member in the same memory location, and unsigned int and float both have 32 bits, if we have an unsigned int and we store it into a union of an unsigned int and a float, then the floating point number, when extracted, will be the one which has the floating point binary representation the same as the binary representation of the unsigned int, and
this is precisely what has been done for the sake of the conversion.

After the checking has been complete, we print the binary representation of the result and the number of cycles taken for the computation to be complete in a single line.

## Testing

For the testing plan, see `test_cases.md`.

As mentioned in the mentioned document, we have two kinds of tests - random and corner cases. For the random cases, run `make brute`, and to clean the directory with files which have been generated from that run, run `make cleanrandom`.

For running the corner cases, run `make run`.
