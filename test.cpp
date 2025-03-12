#include "big_number.hpp"
#include <cassert>
#include <iostream>
#include <sstream>

using namespace big_number;

// Вспомогательная функция для получения строкового представления BigNumber через operator<<.
std::string toString(const BigNumber &num)
{
    std::ostringstream oss;
    oss << num;
    return oss.str();
}

// Вспомогательная функция для создания BigNumber из строки (десятичное представление)
BigNumber fromString(const std::string &s)
{
    std::istringstream iss(s);
    BigNumber num;
    iss >> num;
    return num;
}

void TestConstructorsAndIO()
{
    // Тест: конструктор из строки (новый конструктор)
    BigNumber num1("1234567890");
    assert(toString(num1) == "1234567890");

    // Тест: operator>> с числом "0"
    BigNumber num0;
    {
        std::istringstream iss("0");
        iss >> num0;
    }
    assert(toString(num0) == "0");

    std::cout << "[+] TestConstructorsAndIO PASSED\n";
}

void TestConstructorFromNumber()
{
    // Тест для нового конструктора из числа
    BigNumber num0(0ULL);
    assert(toString(num0) == "0");

    BigNumber num1(123456789ULL);
    assert(toString(num1) == "123456789");

    unsigned long long bigVal = 9876543210123456789ULL;
    BigNumber num2(bigVal);
    // Сравниваем со строковым представлением числа (используем std::to_string)
    assert(toString(num2) == std::to_string(bigVal));

    std::cout << "[+] TestConstructorFromNumber PASSED\n";
}

void TestArithmeticOperations()
{
    BigNumber a = fromString("1000");
    BigNumber b = fromString("200");

    // Сложение: 1000 + 200 = 1200
    BigNumber sum = a + b;
    assert(toString(sum) == "1200");

    // Вычитание: 1000 - 200 = 800
    BigNumber diff = a - b;
    assert(toString(diff) == "800");

    // Умножение: 1000 * 200 = 200000
    BigNumber prod = a * b;
    assert(toString(prod) == "200000");

    // Деление: 1000 / 200 = 5
    BigNumber quot = a / b;
    assert(toString(quot) == "5");

    // Остаток: 1000 % 200 = 0
    BigNumber rem = a % b;
    assert(toString(rem) == "0");

    // Тест деления на скаляр
    BigNumber c = fromString("9876543210");
    BigNumber d = c / 10;
    assert(toString(d) == "987654321");
    BigNumber mod = c % 10;
    assert(toString(mod) == "0");

    std::cout << "[+] TestArithmeticOperations PASSED\n";
}

void TestComparisonOperations()
{
    BigNumber a = fromString("12345");
    BigNumber b = fromString("54321");

    assert(a < b);
    assert(b > a);
    assert(a != b);

    BigNumber c = fromString("12345");
    assert(a == c);
    assert(a <= c);
    assert(a >= c);

    std::cout << "[+] TestComparisonOperations PASSED\n";
}

void TestEdgeCases()
{
    // Нулевые операции
    BigNumber zero = fromString("0");
    BigNumber one = fromString("1");
    assert((zero + zero) == zero);
    assert((zero * zero) == zero);
    assert((one / one) == one);
    assert((one % one) == zero);

    // Большие числа
    BigNumber big1 = fromString("999999999999999999999");
    BigNumber big2 = fromString("1");
    BigNumber sum = big1 + big2;
    assert(sum == fromString("1000000000000000000000"));

    BigNumber diff = big1 - big2;
    assert(diff == fromString("999999999999999999998"));

    std::cout << "[+] TestEdgeCases PASSED\n";
}

void stressTest()
{
    // Количество итераций – можно увеличить для более сильного стресса
    const int iterations = 100000;
    // Максимальная длина для генерации случайных чисел
    const int M = 1000;

    for (int i = 0; i < iterations; ++i)
    {
        // Генерируем случайные длины для чисел A и B
        int n = rand() % M + 1;
        int m = rand() % M + 1;

        // Создаем два случайных больших числа с инициализацией (параметр 1)
        BigNumber A(n, 1);
        BigNumber B(m, 1);

        // Выполняем операцию деления и взятия остатка
        BigNumber C = A / B;
        BigNumber D = A % B;

        // Проверяем, что выполняется свойство: A == C * B + D
        assert(A == C * B + D);
        // Проверяем альтернативное равенство
        assert(A - D == C * B);
        // Проверяем, что остаток меньше делителя
        assert(D < B);

        if (i % 1000 == 0)
        {
            std::cout << "Iteration: " << i << " / " << iterations << std::endl;
        }
    }

    std::cout << "Stress test passed (" << iterations << " iterations)" << std::endl;
}
void FastSquareTest()
{
    big_number::BigNumber BN;
    std::cin >> BN;
    auto multiply = BN * BN;
    std::cout << "BN: " << BN << std::endl << "BN*BN:\n " << multiply << std::endl;
    auto square = BN.FastSquare();
    std::cout << "BN.FastSquare():\n " << square << std::endl;
    if (multiply == square)
        std::cout << "--- BNs are EQUAL ---";
}

int main()
{
    srand(static_cast<unsigned int>(time(NULL)));

    FastSquareTest();
    return 0;
}
