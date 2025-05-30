#include "big_number.hpp"
#include <cassert>
#include <chrono>
#include <iostream>
#include <sstream>

using namespace big_number;
namespace ErrorProb
{
BigNumber Pow(const BigNumber &number, const BigNumber &exp)
{
    BigNumber result("1");
    for (big_number::BigNumber i("0"); i < exp; i += BigNumber("1"))
    {
        result = result * number;
    }
    return result;
};
// Вспомогательная функция: φ(n)
BigNumber GCD(BigNumber a, BigNumber b)
{
    BigNumber zero("0");
    while (b != zero)
    {
        BigNumber temp(b);
        b = a % b;
        a = temp;
    }
    return a;
}

BigNumber EulerTotient(const BigNumber &n)
{
    BigNumber count("0");
    BigNumber one("1");

    for (BigNumber i = one; i < n; i = i + one)
    {
        if (GCD(i, n) == one)
        {
            count = count + one;
        }
    }

    return count;
}

std::pair<BigNumber, BigNumber> Fermat(const BigNumber &n, size_t reliabilityParameter)
{
    BigNumber phi = Pow(EulerTotient(n), BigNumber(std::to_string(reliabilityParameter)));
    BigNumber den = Pow(n, BigNumber(std::to_string(reliabilityParameter)));
    return {phi, den};
}

std::pair<BigNumber, BigNumber> MillerRabin(const BigNumber &n, size_t reliabilityParameter)
{
    BigNumber phi = Pow(EulerTotient(n), BigNumber(std::to_string(reliabilityParameter)));
    BigNumber den = Pow((n * BigNumber("2")), BigNumber(std::to_string(reliabilityParameter)));
    return {phi, den};
}

std::pair<BigNumber, BigNumber> SoloveyStrassen(const BigNumber &n, size_t reliabilityParameter)
{
    BigNumber phi = Pow(EulerTotient(n), BigNumber(std::to_string(reliabilityParameter)));
    BigNumber den = Pow((n * BigNumber("4")), BigNumber(std::to_string(reliabilityParameter)));
    return {phi, den};
}

} // namespace ErrorProb
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

    auto start_multiply = std::chrono::high_resolution_clock::now();
    auto multiply = BN * BN;
    auto end_multiply = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration_multiply = end_multiply - start_multiply;

    std::cout << "BN: " << BN << std::endl << "BN*BN:\n " << multiply << std::endl;

    auto start_square = std::chrono::high_resolution_clock::now();
    auto square = BN.FastSquare();
    auto end_square = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration_square = end_square - start_square;

    std::cout << "BN.FastSquare():\n " << square << std::endl;

    if (multiply == square)
        std::cout << "--- BNs are EQUAL ---\n";

    std::cout << "Multiplication took " << duration_multiply.count() << " seconds\n";
    std::cout << "FastSquare took " << duration_square.count() << " seconds\n";
}

void DichatomicExponentiationTest()
{
    big_number::BigNumber BNp;
    big_number::BigNumber BNe;
    std::cout << "BN to pow: ";
    std::cin >> BNp;
    std::cout << "BN as exp: ";
    std::cin >> BNe;

    auto start_pow = std::chrono::high_resolution_clock::now();
    auto result_pow = BNp.DichatomicExponentiation(BNe);
    auto end_pow = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration_pow = end_pow - start_pow;

    auto start_loop = std::chrono::high_resolution_clock::now();
    big_number::BigNumber result_loop(1, 1); // Инициализируем результат единицей
    big_number::BigNumber one(1, 1);
    big_number::BigNumber counter(1, 0); // Счётчик, начинаем с 0

    // Предполагается, что экспонента BNe неотрицательная
    while (counter < BNe)
    {
        result_loop = result_loop * BNp;
        counter = counter + one; // Увеличиваем счётчик на 1
    }
    auto end_loop = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration_loop = end_loop - start_loop;

    std::cout << "DichatomicExponentiation result: " << result_pow << std::endl;
    std::cout << "Cyclic result: " << result_loop << std::endl;

    std::cout << "DichatomicExponentiation took " << duration_pow.count() << " seconds\n";
    std::cout << "Cyclic took " << duration_loop.count() << " seconds\n";

    if (result_pow == result_loop)
    {
        std::cout << "equal";
    }
}

void BarretAlgoTest()
{
    big_number::BigNumber BNp;
    big_number::BigNumber BNe;
    std::cout << "BN to mod: ";
    std::cin >> BNp;
    std::cout << "BN as mod: ";
    std::cin >> BNe;

    // Проверка стандартного остатка от деления
    auto start = std::chrono::high_resolution_clock::now();
    auto expected_result = BNp % BNe;
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration = end - start;

    // Проверка алгоритма Барретта
    auto start_barret = std::chrono::high_resolution_clock::now();
    auto result_barret = BNp.BarretAlgo(BNe);
    auto end_barret = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration_barret = end_barret - start_barret;

    std::cout << "BN result: " << result_barret << std::endl;
    std::cout << "% result:  " << expected_result << std::endl;
    std::cout << "BarretAlgo took " << duration_barret.count() << " seconds\n";
    std::cout << "% took " << duration.count() << " seconds\n";
}

void FermatTestTest(BigNumber BN, size_t param)
{
    auto start = std::chrono::high_resolution_clock::now();
    bool expected_result = BN.FermatTest(param);
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration = end - start;

    std::cout << "Fermat Test result: " << expected_result << std::endl;
    std::cout << "Fermat Test took: " << duration.count() << std::endl;

    /*if (expected_result)
    {
        auto resError = ErrorProb::Fermat(BN, param);
        std::cout << "Error chance: " << resError.first << " / " << resError.second << "\n";
    }
    */
}

void MillerRabinTestTest(BigNumber BN, size_t param)
{
    auto start = std::chrono::high_resolution_clock::now();
    bool expected_result = BN.MillerRabinTest(param);
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration = end - start;

    std::cout << "MillerRabin Test result: " << expected_result << std::endl;
    std::cout << "MillerRabin Test took: " << duration.count() << std::endl;

    /*if (expected_result)
    {
        auto resError = ErrorProb::MillerRabin(BN, param);
        std::cout << "Error chance: " << resError.first << " / " << resError.second << "\n";
    }*/
}
void SoloveyStrassenTestTest(BigNumber BN, size_t param)
{
    auto start = std::chrono::high_resolution_clock::now();
    bool expected_result = BN.SoloveyStrassenTest(param);
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration = end - start;

    std::cout << "SoloveyStrassen Test result: " << expected_result << std::endl;
    std::cout << "SoloveyStrassen Test took: " << duration.count() << std::endl;
    /*if (expected_result)
    {
        auto resError = ErrorProb::SoloveyStrassen(BN, param);
        std::cout << "Error chance: " << resError.first << " / " << resError.second << "\n";
    }
    */
}

void CompleteTest()
{
    big_number::BigNumber BN;
    std::cout << "BN to check: ";
    std::cin >> BN;
    size_t param;
    std::cout << "Enter readability parameter: ";
    std::cin >> param;

    std::cout << "\n";
    FermatTestTest(BN, param);
    std::cout << "\n";
    MillerRabinTestTest(BN, param);
    std::cout << "\n";
    SoloveyStrassenTestTest(BN, param);
}

int main()
{
    srand(static_cast<unsigned int>(time(NULL)));

    CompleteTest();
    return 0;
}
