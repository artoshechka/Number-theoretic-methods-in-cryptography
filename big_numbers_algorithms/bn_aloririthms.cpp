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
        DoubleBaseType carry = doubledTemp >> BASE_SIZE;

        result.coefficients_[2 * i] = static_cast<BaseType>(doubledTemp);

        // 2.2.
        for (int j = i + 1; j < length_; j++)
        {
            FoursBaseType tripledTemp = static_cast<FoursBaseType>(result.coefficients_[i + j]) +
                                        static_cast<FoursBaseType>(2) * (static_cast<FoursBaseType>(coefficients_[i]) *
                                                                         static_cast<FoursBaseType>(coefficients_[j])) +
                                        static_cast<FoursBaseType>(carry);

            result.coefficients_[i + j] = static_cast<BaseType>(tripledTemp);

            carry = static_cast<DoubleBaseType>(tripledTemp) >> BASE_SIZE;
        }

        // После цикла по j, если есть перенос, добавим его «выше»
        result.coefficients_[i + length_] += static_cast<BaseType>(carry);
        result.coefficients_[i + length_ + 1] += static_cast<BaseType>(carry >> BASE_SIZE);
    }

    // Удалим ведущие нули, если они есть
    result.NormalizeLength();
    return result;
}

BigNumber BigNumber::DichatomicExponentiation(BigNumber &number)
{

    BigNumber result(*this);
    BigNumber oneBigNumber("1");
    BigNumber nullBigNumber("0");
    BigNumber twoBigNumber("2");
    BigNumber base = *this;
    BigNumber exp = number;

    while (exp != nullBigNumber)
    {

        if (exp % 2 != nullBigNumber)
        {
            result *= base;
            exp -= oneBigNumber;
        }
        else
        {
            result = result.FastSquare();
            exp -= twoBigNumber;
        }
    }
    return result;
}

BigNumber BigNumber::BarretAlgo(BigNumber &mod)
{
    if (*this < mod)
    {
        return *this; // Если число меньше модуля, просто возвращаем его
    }

    int k = mod.GetLength(); // Длина числа m
    BigNumber b_k(1, 1);     // Основание степени b^k
    for (int i = 0; i < k; ++i)
    {
        b_k *= 2; // b^k, так как база 2 (можно изменить для других баз)
    }

    BigNumber z = (b_k * b_k) / mod; // z = ⌊b^(2k) / m⌋

    // 1. Вычислить q' = [(x / b^(k-1)) * z] / b^(k+1)
    BigNumber x_div_b_k_1 = *this / b_k;           // x / b^(k-1)
    BigNumber q = (x_div_b_k_1 * z) / (b_k * b_k); // q'

    // 2. Вычислить r1 = x mod b^(k+1), r2 = (q' * m) mod b^(k+1)
    BigNumber b_k_plus_1 = b_k * 2; // b^(k+1)
    BigNumber r1 = *this % b_k_plus_1;
    BigNumber r2 = (q * mod) % b_k_plus_1;

    // 3. r' = r1 - r2, если r1 >= r2, иначе r' = b^(k+1) + r1 - r2
    BigNumber r_prime;
    if (r1 >= r2)
    {
        r_prime = r1 - r2;
    }
    else
    {
        r_prime = b_k_plus_1 + r1 - r2;
    }

    // 4. Пока r' >= m, уменьшаем r' на m
    while (r_prime >= mod)
    {
        r_prime -= mod;
    }

    return r_prime;
}
