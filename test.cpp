#include "big_number.hpp"
#include <iostream>
#include <cassert>

using namespace big_number;
void test()
{
    srand(time(NULL));
    int M = 1000;
    int T = 1000;

    BigNumber A;
    BigNumber B;
    BigNumber C;
    BigNumber D;

    do
    {
        int n = rand() % M + 1;
        int m = rand() % M + 1;

        BigNumber E(n, 1);
        BigNumber G(m, 1);

        A = E;

        B = G;

        C = A / B;
        D = A % B;

        std::cout << "m: " << m << " ";
        std::cout << "n: " << n << " ";
        std::cout << "T: " << T << std::endl;

    } while (A == C * B + D && A - D == C * B && D < B && --T);

    std::cout << T << std::endl;
}

using namespace std;
int main()
{
    srand(time(NULL));
    test();
    return 0;
}