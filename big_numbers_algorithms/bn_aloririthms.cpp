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
    
    // Получаем бинарное представление экспоненты
    std::vector<bool> binary_exp;
    BigNumber zero(1, 0);
    
    // Преобразование экспоненты в двоичную форму без деления
    while (exponent > zero)
    {
        // Проверяем четность через последний коэффициент
        binary_exp.push_back(exponent.coefficients_[0] % 2 == 1);
        
        // Деление на 2 через сдвиг вправо
        BigNumber shifted(exponent.length_);
        BaseType carry = 0;
        
        for (int i = exponent.length_ - 1; i >= 0; i--)
        {
            DoubleBaseType current = exponent.coefficients_[i] + static_cast<DoubleBaseType>(carry) * BASE_SIZE;
            shifted.coefficients_[i] = static_cast<BaseType>(current / 2);
            carry = current % 2;
        }
        
        shifted.NormalizeLength();
        exponent = shifted;
    }
    
    // Алгоритм быстрого возведения в степень с использованием битового представления
    for (int i = 0; i < binary_exp.size(); i++)
    {
        if (binary_exp[i])
        {
            result *= base;
        }
        
        // Продолжаем возводить в квадрат, кроме последней итерации
        if (i < binary_exp.size() - 1)
        {
            base = base.FastSquare();
        }
    }
    
    return result;
}

BigNumber BigNumber::BarretAlgo(BigNumber &mod)
{
    // Проверка на корректность модуля
    if (mod <= BigNumber(1, 0)) {
        throw std::invalid_argument("Модуль должен быть положительным числом.");
    }

    // Если число меньше модуля, возвращаем его как есть
    if (*this < mod) {
        return *this;
    }

    // Определяем значение k - длина модуля + 1
    int k = mod.length_ + 1;
    
    // Вычисляем b^(2k) - степень основания системы счисления
    BigNumber base_power_2k(1, 1);
    BigNumber base(1, BASE_SIZE);  // Основание системы счисления
    
    // Быстрое вычисление степени через дихотомическое возведение в степень
    BigNumber exponent(1, 2 * k);
    base_power_2k = base.DichatomicExponentiation(exponent);
    
    // Вычисляем μ = ⌊b^(2k)/m⌋
    BigNumber mu = base_power_2k / mod;
    
    // Вычисляем q' = ⌊(x·μ)/b^(2k)⌋
    BigNumber q_prime = (*this * mu) / base_power_2k;
    
    // Проверка: если q_prime * mod > *this, уменьшаем q_prime на 1
    BigNumber product = q_prime * mod;
    if (product > *this) {
        BigNumber one(1, 1);
        q_prime = q_prime - one;
        product = q_prime * mod;
    }
    
    // Вычисляем r = x - q'·m
    BigNumber r;
    if (*this >= product) {
        r = *this - product;
    } else {
        // Если всё ещё возникает проблема, используем стандартное вычисление остатка
        r = *this % mod;
        return r;
    }
    
    // Нормализация результата: пока r >= m, вычитаем m
    while (r >= mod) {
        r = r - mod;
    }
    
    return r;
}
