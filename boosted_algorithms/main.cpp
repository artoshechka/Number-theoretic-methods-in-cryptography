#include "algo.hpp"
#include <cassert>
#include <chrono>
#include <iostream>
#include <sstream>
namespace
{
double fractionToDouble(const BigNumber &numerator, const BigNumber &denominator)
{
    // Преобразуем BigNumber в double (может потерять точность для очень больших чисел)
    return static_cast<double>(numerator) / static_cast<double>(denominator);
}
} // namespace

namespace ErrorProb
{
// Оптимизированная функция Эйлера
BigNumber EulerTotaient(const BigNumber &n)
{
    if (n < 1)
        return 0;
    if (n == 1)
        return 1;

    BigNumber result = n;
    BigNumber temp = n;

    // Проверка делимости на 2
    if (temp % 2 == 0)
    {
        result -= result / 2;
        while (temp % 2 == 0)
            temp /= 2;
    }

    // Проверка нечётных делителей до квадратного корня
    for (BigNumber i = 3; i * i <= temp; i += 2)
    {
        if (temp % i == 0)
        {
            result -= result / i;
            while (temp % i == 0)
                temp /= i;
        }
    }

    // Если остался простой делитель > 2
    if (temp > 1)
        result -= result / temp;

    return result;
}

// Оптимизированное возведение в степень
BigNumber Pow(const BigNumber &base, size_t exp)
{
    if (exp == 0)
        return 1;
    BigNumber result = 1;
    BigNumber x = base;

    while (exp > 0)
    {
        if (exp % 2 == 1)
        {
            result *= x;
        }
        x *= x;
        exp /= 2;
    }

    return result;
}
std::pair<BigNumber, BigNumber> Fermat(BigNumber phi, const BigNumber &n, size_t k)
{
    return {Pow(phi, k), Pow(n, k)};
}

std::pair<BigNumber, BigNumber> MillerRabin(BigNumber phi, const BigNumber &n, size_t k)
{
    return {Pow(phi, k), Pow(n * 2, k)};
}

std::pair<BigNumber, BigNumber> SoloveyStrassen(BigNumber phi, const BigNumber &n, size_t k)
{

    return {Pow(phi, k), Pow(n * 4, k)};
}
} // namespace ErrorProb

void FermatTestTest(BigNumber BN, size_t param)
{
    auto start = std::chrono::high_resolution_clock::now();
    bool result = FermatTest(BN, param);
    auto end = std::chrono::high_resolution_clock::now();

    std::cout << "Fermat Test result: " << result << "\n";
    std::cout << "Time: " << std::chrono::duration<double>(end - start).count() << "s\n";
}

void MillerRabinTestTest(BigNumber BN, size_t param)
{
    auto start = std::chrono::high_resolution_clock::now();
    bool result = MillerRabinTest(BN, param);
    auto end = std::chrono::high_resolution_clock::now();

    std::cout << "Miller-Rabin Test result: " << result << "\n";
    std::cout << "Time: " << std::chrono::duration<double>(end - start).count() << "s\n";
}

void SoloveyStrassenTestTest(BigNumber BN, size_t param)
{
    auto start = std::chrono::high_resolution_clock::now();
    bool result = SoloveyStrassenTest(BN, param);
    auto end = std::chrono::high_resolution_clock::now();

    std::cout << "Solovey-Strassen Test result: " << result << "\n";
    std::cout << "Time: " << std::chrono::duration<double>(end - start).count() << "s\n";
}

void CompleteTest()
{
    BigNumber BN;
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
void LukaTestTest()
{
    BigNumber BN;
    std::cout << "BN to check: ";
    std::cin >> BN;
    size_t param;
    std::cout << "Enter readability parameter: ";
    std::cin >> param;

    auto start = std::chrono::high_resolution_clock::now();
    bool result = LucasTest(BN, param);
    auto end = std::chrono::high_resolution_clock::now();

    std::cout << "Lucas Test result: " << result << "\n";
    std::cout << "Time: " << std::chrono::duration<double>(end - start).count() << "s\n";
}

void GordonTest()
{
    size_t param;
    std::cout << "Enter length of BigNumber to generate: ";
    std::cin >> param;

    auto start = std::chrono::high_resolution_clock::now();
    auto result = GordonsPrimeGenerator(param);
    auto end = std::chrono::high_resolution_clock::now();

    std::cout << "Gordon Generator result: " << result << "\n";
    std::cout << "IS PRIME" << std::endl;
    std::cout << "Fermat Test result: " << FermatTest(result, 25) << "\n";
    std::cout << "Solovey-Strassen Test result: " << SoloveyStrassenTest(result, 25) << "\n";
    std::cout << "Miller-Rabin Test result: " << MillerRabinTest(result, 25) << "\n";
    std::cout << "Lucas Test result: " << LucasTest(result, 25) << "\n";
    std::cout << "Time: " << std::chrono::duration<double>(end - start).count() << "s\n";
}

int main()
{
    srand(static_cast<unsigned int>(time(NULL)));

    LukaTestTest();
    return 0;
}