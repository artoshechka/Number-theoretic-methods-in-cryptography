#include <boost/integer.hpp>
#include <boost/multiprecision/cpp_int.hpp>
#include <boost/random.hpp>
#include <boost/random/random_device.hpp>
#include <boost/random/uniform_int_distribution.hpp>
#include <random>

using boost::multiprecision::cpp_int;

using BigNumber = cpp_int;

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
