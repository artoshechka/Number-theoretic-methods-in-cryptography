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

    // Присвоим результат обратно текущему объекту
    return result;
}
