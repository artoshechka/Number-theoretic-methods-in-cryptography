#include <boost/integer.hpp>
#include <boost/multiprecision/cpp_int.hpp>
#include <boost/random.hpp>
#include <boost/random/random_device.hpp>
#include <boost/random/uniform_int_distribution.hpp>
#include <random>

using boost::multiprecision::cpp_int;

using BigNumber = cpp_int;
using PrimeFactors = std::vector<std::pair<BigNumber, BigNumber>>;

BigNumber Generator(const BigNumber &min, const BigNumber &max)
{
    static boost::random::random_device rd;
    static boost::random::mt19937_64 rng(rd());

    boost::random::uniform_int_distribution<cpp_int> dist(min, max);
    return dist(rng);
}

BigNumber PowMod(BigNumber number, BigNumber exp, const BigNumber &mod)
{
    BigNumber result = 1;
    BigNumber base = number % mod; // Уменьшаем base по модулю, чтобы работать с меньшими числами

    while (exp > 0)
    {
        if (exp % 2 == 1)
            result = (result * base) % mod;
        base = (base * base) % mod;
        exp /= 2;
    }
    return result;
}

BigNumber JacobiNumbers(const BigNumber &a, const BigNumber &n)
{
    if (a == 0)
    {
        return 0;
    }
    if (a == 1)
    {
        return 1;
    }
    BigNumber k = 0;
    BigNumber a1 = a;

    while (a1 % 2 == 0)
    {
        a1 = a1 / 2;
        k = k + 1;
    }

    BigNumber s;
    if (k % 2 == 0)
    {
        s = 1;
    }
    else
    {
        BigNumber n_mod8 = n % 8;
        if (n_mod8 == 1 || n_mod8 == 7)
        {
            s = 1;
        }
        else
        {
            s = -1;
        }
    }
    if (n % 4 == 3 && a1 % 4 == 3)
    {
        s = -s;
    }
    if (a1 == 1)
    {
        return s;
    }
    else
    {
        return s * JacobiNumbers(n % a1, a1);
    }
};

bool FermatTest(const BigNumber &number, size_t reliabilityParameter)
{
    if (number < 4)
        throw std::invalid_argument("Число должно быть больше 3");

    for (size_t i = 0; i < reliabilityParameter; ++i)
    {
        auto randBN = Generator(2, number - 2);
        if (PowMod(randBN, number - 1, number) != 1)
            return false;
    }
    return true;
}

bool MillerRabinTest(const BigNumber &number, size_t reliabilityParameter)
{
    if (number < 4)
        throw std::invalid_argument("Число должно быть больше 3");

    BigNumber d = number - 1;
    BigNumber s = 0;

    while (d % 2 == 0)
    {
        d /= 2;
        s += 1;
    }

    for (size_t i = 0; i < reliabilityParameter; ++i)
    {
        auto randBN = Generator(2, number - 2);
        BigNumber x = PowMod(randBN, d, number);

        if (x == 1 || x == number - 1)
            continue;

        for (BigNumber j = 1; j < s; ++j)
        {
            x = PowMod(x, 2, number);
            if (x == number - 1)
                break;
            if (x == 1)
                return false;
        }

        if (x != number - 1)
            return false;
    }
    return true;
}

bool SoloveyStrassenTest(const BigNumber &number, size_t reliabilityParameter)
{
    if (number < 4)
        throw std::invalid_argument("Число должно быть больше 3");

    for (size_t i = 0; i < reliabilityParameter; ++i)
    {
        auto randBN = Generator(2, number - 2);
        auto r = PowMod(randBN, (number - 1) / 2, number);

        if (r != 1 && r != number - 1)
            return false;

        auto jacobiNumber = JacobiNumbers(randBN, number);
        BigNumber s = (jacobiNumber == -1) ? number - 1 : jacobiNumber;

        if (r != s)
            return false;
    }
    return true;
}

PrimeFactors Factorize(BigNumber n)
{
    PrimeFactors factors;

    // Обработка делителей 2
    if (n % 2 == 0)
    {
        BigNumber count = 0;
        while (n % 2 == 0)
        {
            n /= 2;
            ++count;
        }
        factors.emplace_back(2, count);
    }

    // Обработка нечётных делителей
    for (BigNumber p = 3; p * p <= n; p += 2)
    {
        if (n % p == 0)
        {
            BigNumber count = 0;
            while (n % p == 0)
            {
                n /= p;
                ++count;
            }
            factors.emplace_back(p, count);
        }
    }

    if (n > 1)
    {
        factors.emplace_back(n, 1);
    }

    return factors;
}

// Корректная реализация теста Люка
bool LucasTest(const BigNumber &n, size_t t)
{
    if (n < 4)
    {
        throw std::invalid_argument("Число должно быть больше 3");
    }
    if (n % 2 == 0)
    {
        throw std::invalid_argument("Число должно быть нечетным");
    }

    BigNumber nm1 = n - 1;
    PrimeFactors factors = Factorize(nm1);
    bool allConditionsMet = true;
    for (size_t i = 0; i < t; ++i)
    {
        BigNumber a = Generator(2, n - 2);

        if (PowMod(a, nm1, n) != 1)
            return false;

        for (const auto &[p, exp] : factors)
        {
            if (PowMod(a, nm1 / p, n) == 1)
            {
                allConditionsMet = false;
                break;
            }
        }

        if (allConditionsMet)
            return true;
    }
    return false;
}

// Генерация случайного простого числа длины bitLength бит
BigNumber GenerateRandomPrime(size_t bitLength, size_t mrRounds = 25)
{
    if (bitLength < 2)
        throw std::invalid_argument("bitLength must be at least 2");

    // Минимум: 2^(bitLength-1), максимум: 2^bitLength - 1
    BigNumber min = BigNumber(1) << (bitLength - 1);
    BigNumber max = (BigNumber(1) << bitLength) - 1;

    while (true)
    {
        // 1) Случайное число в диапазоне [min, max]
        BigNumber candidate = Generator(min, max);

        // 2) Гарантируем, что число нечетное и имеет старший бит = 1
        candidate |= 1;                               // младший бит = 1 → нечетное
        candidate |= BigNumber(1) << (bitLength - 1); // старший бит = 1

        // 3) Тест простоты
        if (MillerRabinTest(candidate, mrRounds))
            return candidate;
        // иначе — пробуем заново
    }
}

BigNumber GordonsPrimeGenerator()
{
    BigNumber s = GenerateRandomPrime(128);
    BigNumber t = GenerateRandomPrime(128);

    BigNumber i = Generator(1, BigNumber(1) << 16);
    BigNumber r;
    while (true)
    {
        r = 2 * i * t + 1;
        if (r > s && MillerRabinTest(r, 25))
            break;
        ++i;
    }

    BigNumber s_pow = PowMod(s, r - 2, r);
    BigNumber p0 = 2 * s_pow * s - 1;

    BigNumber j = Generator(1, BigNumber(1) << 16);
    BigNumber p;
    while (true)
    {
        p = 2 * j * r * s + p0;
        if (p > r * s && MillerRabinTest(p, 25))
            break;
        ++j;
    }

    return p;
}