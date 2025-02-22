#include "big_number.hpp"
#include <iostream>
#include <cassert>

using namespace big_number;
void TestBigNumber()
{
    // Тест конструктора
    BigNumber num1(5, 1);
    assert(num1.GetLength() == 5);

    // Тест конструктора копирования
    BigNumber num2 = num1;
    assert(num2.GetLength() == num1.GetLength());

    // Тест сложения
    BigNumber num3(5, 1);
    BigNumber sum = num1 + num3;
    assert(sum.GetLength() >= num1.GetLength());

    // Тест вычитания
    if (num1 > num3)
    {
        BigNumber diff = num1 - num3;
        assert(diff.GetLength() <= num1.GetLength());
    }

    // Тест умножения
    BigNumber prod = num1 * num3;
    assert(prod.GetLength() >= num1.GetLength());

    // Тест деления
    if (num3.GetLength() > 1)
    {
        BigNumber div = num1 / num3;
        assert(div.GetLength() <= num1.GetLength());
    }

    // Тест нормализации длины
    BigNumber zero(5, 0);
    zero.NormalizeLength();
    assert(zero.GetLength() == 1);

    std::cout << "All tests passed successfully!" << std::endl;
}

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

        std::cout << "A: " << A << ", B: " << B << ", C: " << C << ", D: " << D << std::endl;
        std::cout << "Check: " << (A == C * B + D) << ", " << (A - D == C * B) << ", " << (D < B) << std::endl;

    } while (A == C * B + D && A - D == C * B && D < B && --T);

    std::cout << "Remaining T: " << T << std::endl;
}

using namespace std;
int main()
{
    srand(time(NULL));
    test();
    TestBigNumber();
    return 0;
}