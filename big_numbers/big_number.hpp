#ifndef BIG_NUMBER_HPP
#define BIG_NUMBER_HPP

#include <algorithm>
#include <cstring>
#include <ctime>
#include <iostream>
#include <limits>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>

namespace big_number
{
using BaseType = unsigned char;               ///< Основной тип для хранения коэффициентов
using DoubleBaseType = unsigned int;          ///< Тип для хранения удвоенных значений
using FoursBaseType = unsigned long long int; ///< Тип для хранения учетверенных значений

constexpr int BASE_SIZE = sizeof(BaseType) * 8;        ///< Размер BaseType в битах
constexpr int DBASE_SIZE = sizeof(DoubleBaseType) * 8; ///< Размер DoubleBaseType в битах
constexpr int QBASE_SIZE = sizeof(FoursBaseType) * 8;  ///< Размер FoursBaseType в битах

/// @brief Класс для работы с большими числами на основе массива коэффициентов
class BigNumber
{
  protected:
    std::vector<BaseType> coefficients_; ///< Коэффициенты числа (младший разряд имеет индекс 0)
    int length_;                         ///< Фактическая длина числа (количество значащих коэффициентов)
    int maxLength_;                      ///< Максимально возможная длина массива коэффициентов

  public:
    /// @brief Конструктор, создающий число с заданной максимальной длиной и начальным значением
    /// @param[in] max_length Максимальная длина числа
    /// @param[in] parameter Начальное значение (записывается в первый коэффициент, остальные – 0)
    explicit BigNumber(int max_length = 1, int parameter = 0);

    /// @brief Конструктор копирования
    /// @param[in] other Объект для копирования
    BigNumber(const BigNumber &other);

    /// @brief Конструктор из строки (десятичное представление числа)
    /// @param[in] s Строка с десятичным представлением числа
    explicit BigNumber(const std::string &s);

    /// @brief Конструктор из числа (unsigned long long)
    /// @param[in] value Число, которое необходимо представить в виде BigNumber
    explicit BigNumber(unsigned long long value);

    /// @brief Деструктор
    ~BigNumber() = default;

    /// @brief Получает фактическую длину числа
    /// @return Количество значащих коэффициентов
    int GetLength();

    /// @brief Получает максимальную длину числа
    /// @return Максимально возможное количество коэффициентов
    int GetMaxLength();

    /// @brief Получает указатель на массив коэффициентов
    /// @return Указатель на массив коэффициентов
    BaseType *GetCoefficients();

    /// @brief Устанавливает текущую длину числа
    /// @param[in] newLength Новое значение длины
    /// @throw std::invalid_argument если newLength больше maxLength
    void SetLength(int newLength);

    /// @brief Устанавливает максимальную длину числа
    /// @param[in] newMaxLength Новая максимальная длина
    /// @throw std::invalid_argument если newMaxLength меньше текущей длины
    void SetMaxLength(int newMaxLength);

    /// @brief Устанавливает новые коэффициенты
    /// @param[in] index Индекс вставки
    /// @param[in] value Значение вставки
    void SetCoefficient(int index, BaseType value);

    /// @brief Нормализует длину числа, удаляя незначащие нули
    void NormalizeLength();

    /// @brief Оператор сравнения на равенство
    /// @param[in] other Число для сравнения
    /// @return true, если числа равны, иначе false
    bool operator==(const BigNumber &other) const;

    /// @brief Оператор сравнения на неравенство
    /// @param[in] other Число для сравнения
    /// @return true, если числа не равны, иначе false
    bool operator!=(const BigNumber &other) const;

    /// @brief Оператор сравнения "меньше"
    /// @param[in] other Число для сравнения
    /// @return true, если текущее число меньше, иначе false
    bool operator<(const BigNumber &other) const;

    /// @brief Оператор сравнения "больше"
    /// @param[in] other Число для сравнения
    /// @return true, если текущее число больше, иначе false
    bool operator>(const BigNumber &other) const;

    /// @brief Оператор сравнения "меньше или равно"
    /// @param[in] other Число для сравнения
    /// @return true, если текущее число меньше или равно, иначе false
    bool operator<=(const BigNumber &other) const;

    /// @brief Оператор сравнения "больше или равно"
    /// @param[in] other Число для сравнения
    /// @return true, если текущее число больше или равно, иначе false
    bool operator>=(const BigNumber &other) const;

    /// @brief Оператор присваивания
    /// @param[in] other Число для присваивания
    /// @return Ссылка на текущее число
    BigNumber &operator=(const BigNumber &other);

    /// @brief Оператор сложения
    /// @param[in] other Число для сложения
    /// @return Результат сложения
    BigNumber operator+(const BigNumber &other) const;

    /// @brief Оператор сложения с присваиванием
    /// @param[in] other Число для сложения
    /// @return Ссылка на текущее число
    BigNumber &operator+=(const BigNumber &other);

    /// @brief Оператор вычитания
    /// @param[in] other Число для вычитания
    /// @return Результат вычитания
    /// @throw std::invalid_argument если результат вычитания будет отрицательным
    BigNumber operator-(const BigNumber &other) const;

    /// @brief Оператор вычитания с присваиванием
    /// @param[in] other Число для вычитания
    /// @return Ссылка на текущее число
    BigNumber &operator-=(const BigNumber &other);

    /// @brief Оператор умножения на скаляр
    /// @param[in] value Скаляр для умножения
    /// @return Результат умножения
    BigNumber operator*(const BaseType &value) const;

    /// @brief Оператор умножения на скаляр с присваиванием
    /// @param[in] value Скаляр для умножения
    /// @return Ссылка на текущее число
    BigNumber &operator*=(const BaseType &value);

    /// @brief Оператор умножения на большое число
    /// @param[in] other Число для умножения
    /// @return Результат умножения
    BigNumber operator*(const BigNumber &other) const;

    /// @brief Оператор умножения на большое число с присваиванием
    /// @param[in] other Число для умножения
    /// @return Ссылка на текущее число
    BigNumber &operator*=(const BigNumber &other);

    /// @brief Оператор деления на скаляр
    /// @param[in] value Скаляр для деления
    /// @return Результат деления
    /// @throw std::invalid_argument если деление на ноль
    BigNumber operator/(const BaseType &value) const;

    /// @brief Оператор остатка от деления на скаляр
    /// @param[in] value Скаляр для деления
    /// @return Остаток от деления
    /// @throw std::invalid_argument если деление на ноль
    BigNumber operator%(const BaseType &value) const;

    /// @brief Оператор деления на большое число
    /// @param[in] other Число для деления
    /// @return Результат деления
    /// @throw std::invalid_argument если деление на ноль
    BigNumber operator/(const BigNumber &other) const;

    /// @brief Оператор остатка от деления на большое число
    /// @param[in] other Число для деления
    /// @return Остаток от деления
    /// @throw std::invalid_argument если деление на ноль
    BigNumber operator%(const BigNumber &other) const;

    /// @brief Вывод числа в 16-ричной системе
    void PrintHex() const;

    /// @brief Чтение числа в 16-ричной системе
    void ReadHex();

    /// @brief Оператор ввода из потока
    /// @param[in,out] in Входной поток
    /// @param[out] number Число для ввода
    /// @return Входной поток
    friend std::istream &operator>>(std::istream &in, BigNumber &number);

    /// @brief Оператор вывода в поток (вывод в десятичном виде)
    /// @param[in,out] out Выходной поток
    /// @param[in] number Число для вывода
    /// @return Выходной поток
    friend std::ostream &operator<<(std::ostream &out, const BigNumber number);

    /// @brief Быстрое возведение в квадрат на основе алгоритма с оптимизацией
    /// @return Результат возведения в квадрат
    BigNumber FastSquare();

    /// @brief Дихотомический алгоритм возведения в степень
    /// @param[in] number Число для возведения в степень
    /// @return Результат возведения в степень
    BigNumber DichatomicExponentiation(BigNumber &number);

    /// @brief Алгоритм Барретта
    /// @param[in] number Число для алгоритма Барретта
    /// @return Результат алгоритма Барретта
    BigNumber BarretAlgo(BigNumber &number);

  private:
    // Статическая вспомогательная функция деления по алгоритму Кнута,
    // которая имеет доступ к защищённым членам класса.
    /// @param[in] u_orig Делимое
    /// @param[in] v_orig Делитель
    /// @return Пара из частного и остатка
    /// @throw std::invalid_argument если деление на ноль
    static std::pair<BigNumber, BigNumber> DivideKnuth(const BigNumber &u_orig, const BigNumber &v_orig);
};

} // namespace big_number

#endif // BIG_NUMBER_HPP
