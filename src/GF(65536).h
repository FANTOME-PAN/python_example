/*
  implementation of operations (divide, multiply, add, subtract, power ) in finite field GF(65536).
  implementation of langrange interpolation in GF(65536).
*/
#ifndef GF_65536
#define GF_65536


namespace GF65536 {
    class operand {
    public:
        unsigned short num;
        operand();
        operand(unsigned short n);

        operand operator+(operand b);
        operand operator^(operand b);
        operand operator-(operand b);
        void operator=(int n);
        operand operator*(operand b);
        bool operator!=(operand b);
        operand operator~();
        operand operator/(operand b);

        void print();
    };


    typedef struct point {
        operand x;
        operand y;
    }point;

};
/*
  slow multiplication of bytes, a * b,Time Complexity : O(log a)
*/
GF65536::operand slowMul(GF65536::operand& a, GF65536::operand& b);

std::ostream& operator<<(std::ostream& out, GF65536::operand b);

bool operator<(const GF65536::operand& lhs, const GF65536::operand& rhs);
/*
  calculating a ^ b, time Complexity : O(log b)
*/
GF65536::operand power(GF65536::operand a, int b);
/*
  lagrange interpolation algorithm.
  rather than calculating the complete ploynomial p(x) only value of p(0) is calculated as this is
  secret.
*/
unsigned short interpolate_gf65536(std::vector<GF65536::point>& share);
#endif
