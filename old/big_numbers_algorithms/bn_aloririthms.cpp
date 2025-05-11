#include "big_number.hpp"

using big_number::BigNumber;
namespace
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

int JacobiNumbers(const BigNumber &a, const BigNumber &n)
{
    if (a == BigNumber("0"))
    {
        return 0;
    }
    if (a == BigNumber("1"))
    {
        return 1;
    }
    BigNumber k = BigNumber("0");
    BigNumber a1 = a;

    while (a1 % BigNumber("2") == BigNumber("0"))
    {
        a1 = a1 / BigNumber("2");
        k = k + BigNumber("1");
    }

    int s;
    if (k % BigNumber("2") == BigNumber("0"))
    {
        s = 1;
    }
    else
    {
        BigNumber n_mod8 = n % BigNumber("8");
        if (n_mod8 == BigNumber("1") || n_mod8 == BigNumber("7"))
        {
            s = 1;
        }
        else
        {
            s = -1;
        }
    }
    if (n % BigNumber("4") == BigNumber("3") && a1 % BigNumber("4") == BigNumber("3"))
    {
        s = -s;
    }
    if (a1 == BigNumber("1"))
    {
        return s;
    }
    else
    {
        return s * JacobiNumbers(n % a1, a1);
    }
};
} // namespace

BigNumber BigNumber::FastSquare()
{
    // Шаг 1:
    BigNumber result(2 * length_ + 1);
    result.length_ = 2 * length_ + 1;

    // Шаг 2
    for (int i = 0; i < length_; i++)
    {
        // 2.1
        DoubleBaseType doubledTemp =
            static_cast<DoubleBaseType>(result.coefficients_[2 * i]) +
            static_cast<DoubleBaseType>(coefficients_[i]) * static_cast<DoubleBaseType>(coefficients_[i]);
        DoubleBaseType carry = doubledTemp / BASE_SIZE;

        result.coefficients_[2 * i] = static_cast<BaseType>(doubledTemp % BASE_SIZE);

        // 2.2.
        for (int j = i + 1; j < length_; j++)
        {
            FoursBaseType extendedTemp =
                static_cast<FoursBaseType>(result.coefficients_[i + j]) +
                static_cast<FoursBaseType>(2) *
                    (static_cast<FoursBaseType>(coefficients_[i]) * static_cast<FoursBaseType>(coefficients_[j])) +
                static_cast<FoursBaseType>(carry);

            result.coefficients_[i + j] = static_cast<BaseType>(extendedTemp % BASE_SIZE);

            carry = static_cast<DoubleBaseType>(extendedTemp) / BASE_SIZE;
        }

        result.coefficients_[i + length_] += static_cast<BaseType>(carry % BASE_SIZE);
        result.coefficients_[i + length_ + 1] += static_cast<BaseType>(carry / BASE_SIZE);
    }

    result.NormalizeLength();
    return result;
}

BigNumber BigNumber::DichatomicExponentiation(const BigNumber &exp) const
{
    BigNumber result(1, 1); // Начинаем с 1
    BigNumber base = *this;
    BigNumber exponent = exp;
    BigNumber zero(1, 0);
    BigNumber one(1, 1);
    BigNumber two(1, 2);

    while (exponent != zero)
    {
        if (exponent % two == one)
        {
            result *= base;
        }
        base = base.FastSquare();
        exponent = exponent / two;
    }
    return result;
}

BigNumber BigNumber::BarretAlgo(const BigNumber &m) const
{
    if (m <= BigNumber("0"))
    {
        throw std::invalid_argument("The modulus must be a positive number.");
    }
    if (*this < m)
    {
        return *this;
    }

    int k = m.length_;
    if (this->length_ > 2 * k)
    {
        throw std::invalid_argument("Invalid data.");
    }
    BigNumber base("10");

    if (k == 0)
    {
        return *this;
    }

    // Вычисление b^(k)
    BigNumber base_power_k("1");
    BigNumber base_bn(base);

    for (int i = 0; i < k; ++i)
    {
        base_power_k = base_bn * base_power_k;

        if (base_power_k == BigNumber("0"))
        {
            throw std::runtime_error("Exponentiation failed, base_power_k is zero!");
        }
    }

    // Вычисление b^(2k)
    BigNumber base_power_2k = base_power_k * base_power_k;

    // Вычисление z = (b^(2k) / m)
    BigNumber z = base_power_2k / m;

    // Вычисление b^(k-1) и b^(k+1)
    BigNumber b_k_minus_1 = base_power_2k / base;
    BigNumber b_k_plus_1 = base_power_2k * base;

    if (b_k_plus_1 == BigNumber("0"))
    {
        throw std::runtime_error("Division by zero: b_k_plus_1 is zero!");
    }
    // Вычисление q' = (x * z) / b^(2k)
    BigNumber q_prime = (*this * z) / base_power_2k;
    // BigNumber q_prime = ((*this / b_k_minus_1) * z) / b_k_plus_1;

    // Вычисление r1 и r2
    BigNumber r1 = *this % b_k_plus_1;
    BigNumber r2 = (q_prime * m) % b_k_plus_1;
    BigNumber r = *this - q_prime * m;

    // Инициализация переменной r_
    BigNumber r_;

    if (r1 >= r2)
    {
        r_ = r1 - r2;
    }
    else
    {
        r_ = b_k_plus_1 + r1 - r2;
    }
    r = r % m;

    while (r_ >= m)
    {
        r_ = r_ - m;
    }
    return r;
}

BigNumber BigNumber::ModularExponentiation(const BigNumber &exponent, const BigNumber &modulus) const
{
    // Начинаем с result = 1, приводим базу к модулю
    BigNumber result("0");
    BigNumber base(*this);

    std::vector<bool> bits = exponent.ToBinary();
    for (auto bit : bits)
    {
        std::cout << bit << " ";
    }
    std::cout << "size:" << bits.size() << std::endl;
    if (bits[bits.size() - 1] == 0)
    {
        result = BigNumber("1");
    }
    else
    {
        result = *this;
    }
    std::cout << "\n" << result;

    for (int i = bits.size() - 2; i >= 0; --i)
    {
        std::cout << "\n" << base * base;
        base = base * base;
        if (bits[i] == 1)
        {
            result = result * base;
        }
        std::cout << "\n" << i << " " << base << " " << result << std::endl;
    }
    /*// Проходим по всем битам сверху вниз
    for (bool bit : bits)
    {

        if (bit)
        {
            result = (result * base) % modulus;
        }
        base = (base * base) % modulus;
    }*/

    return result % modulus;
}

std::vector<bool> BigNumber::ToBinary() const
{
    std::vector<bool> bits;
    BigNumber temp = *this;

    while (temp != BigNumber("0"))
    {
        bits.insert(bits.begin(), (temp % BigNumber("2")) == BigNumber("1"));
        temp = (temp) / BigNumber("2");
    }
    return bits;
}

BigNumber BigNumber::Generator(int length, BigNumber startValue, BigNumber endValue)
{
    BigNumber number(length);

    number.length_ = length;
    for (int i = 0; i < number.maxLength_; i++)
    {
        number.coefficients_[i] = rand();
    }
    if (sizeof(BaseType) == sizeof(unsigned int))
    { // увеличинная генерация больших чтсел в основании с int
        for (int i = 0; i < number.maxLength_; ++i)
        {
            number.coefficients_[i] <<= 16;
            number.coefficients_[i] |= rand();
        }
    }
    number = number % (endValue - startValue + BigNumber("1")) + startValue;
    return number;
}

bool BigNumber::FermatTest(size_t reliabilityParameter)
{
    if (*this < BigNumber("4"))
    {
        throw std::invalid_argument("N must be grater then 3");
    }

    if (*this % BigNumber("2") == BigNumber("0"))
    {
        return false;
    }
    for (size_t i = 0; i < reliabilityParameter; ++i)
    {
        auto randBN = Generator(length_, BigNumber("2"), (*this - BigNumber("2")));
        std::cout << "randGen: " << randBN << " pow: " << (*this - BigNumber("1")) << " mod: " << *this << " equals ->"
                  << randBN.ModularExponentiation((*this - BigNumber("1")), *this) << std::endl
                  << std::endl;
        if (randBN.ModularExponentiation((*this - BigNumber("1")), *this) != BigNumber("1"))
        {
            return false;
        }
    }
    return true;
}

bool BigNumber::MillerRabinTest(size_t reliabilityParameter)
{
    if (*this < BigNumber("4"))
    {
        throw std::invalid_argument("N must be grater then 3");
    }
    if (*this % BigNumber("2") == BigNumber("0"))
    {
        return false;
    }
    BigNumber s("0");
    BigNumber r = *this - BigNumber("1");

    while (r % BigNumber("2") == BigNumber("0"))
    {
        r = r / BigNumber("2");
        s += BigNumber("1");
    }

    for (size_t i = 0; i < reliabilityParameter; ++i)
    {
        auto randBN = Generator(length_, BigNumber("2"), (*this - BigNumber("2")));

        BigNumber y = randBN.ModularExponentiation(r, *this);

        if (!((y == BigNumber("1")) || (y == *this - BigNumber("1"))))
        {
            BigNumber j("1");
            while (j < s && !(y == *this - BigNumber("1")))
            {
                y = y.ModularExponentiation(BigNumber("2"), *this);
                if (y == BigNumber("1"))
                {
                    return false;
                }
                j += BigNumber("1");
            }
            if (!(y == *this - BigNumber("1")))
            {
                return false;
            }
        }
    }

    return true;
}

bool BigNumber::SoloveyStrassenTest(size_t reliabilityParameter)
{
    if (*this < BigNumber("4"))
    {
        throw std::invalid_argument("N must be grater then 3");
    }

    if (*this % BigNumber("2") == BigNumber("0"))
    {
        return false;
    }
    for (size_t i = 0; i < reliabilityParameter; ++i)
    {
        auto randBN = Generator(length_, BigNumber("2"), (*this - BigNumber("2")));
        auto r = randBN.ModularExponentiation(((*this - BigNumber("1")) / BigNumber("2")), *this);

        if (!((r == BigNumber("1")) || (r == *this - BigNumber("1"))))
        {
            return false;
        }
        auto jacobiNumber = JacobiNumbers(randBN, *this);
        BigNumber s = (jacobiNumber == -1) ? *this - BigNumber("1") : BigNumber(std::to_string(jacobiNumber));

        if (r != s)
        {
            return false;
        }
    }
    return true;
}
