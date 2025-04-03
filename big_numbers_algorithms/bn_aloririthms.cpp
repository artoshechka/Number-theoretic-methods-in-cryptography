#include "big_number.hpp"

using big_number::BigNumber;

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