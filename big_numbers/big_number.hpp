#ifndef BIG_NUMBER_HPP
#define BIG_NUMBER_HPP

#include <algorithm>
#include <cstring>
#include <ctime>
#include <iostream>
#include <limits>
#include <stdexcept>
#include <string>
#include <memory>
#include <vector>

using BaseType = unsigned char;		   ///< Основной тип для хранения коэффициентов
using DoubleBaseType = unsigned short; ///< Тип для хранения удвоенных значений

constexpr int BASE_SIZE = sizeof(BaseType) * 8;		   ///< Размер BaseType в битах
constexpr int DBASE_SIZE = sizeof(DoubleBaseType) * 8; ///< Размер DoubleBaseType в битах

namespace big_number
{
	/// @brief Класс для работы с большими числами на основе массива коэффициентов
	class BigNumber
	{
	protected:
		std::vector<BaseType> coefficients_; ///< Коэффициенты числа (младший разряд имеет индекс 0)
		int length_;						 ///< Фактическая длина числа (количество значащих коэффициентов)
		int maxLength_;						 ///< Максимально возможная длина массива коэффициентов

	public:
		/// @brief Конструктор, создающий число с заданной максимальной длиной и начальным значением
		/// @param max_length Максимальная длина числа
		/// @param parameter Начальное значение (записывается в первый коэффициент, остальные – 0)
		explicit BigNumber(int max_length = 1, int parameter = 0);

		/// @brief Конструктор копирования
		/// @param other Объект для копирования
		BigNumber(const BigNumber &other);

		/// @brief Конструктор из строки (десятичное представление)
		/// @param s Строка с десятичным представлением числа
		explicit BigNumber(const std::string &s);

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
		/// @param newLength Новое значение длины
		/// @throw std::invalid_argument если newLength больше maxLength
		void SetLength(int newLength);

		/// @brief Устанавливает максимальную длину числа
		/// @param newMaxLength Новая максимальная длина
		/// @throw std::invalid_argument если newMaxLength меньше текущей длины
		void SetMaxLength(int newMaxLength);

		/// @brief Устанавливает новые коэффициенты
		/// @param newCoefficients Указатель на новый массив коэффициентов
		void SetCoefficients(BaseType *newCoefficients);

		/// @brief Нормализует длину числа, удаляя незначащие нули
		void NormalizeLength();

		/// @brief Оператор сравнения на равенство
		bool operator==(const BigNumber &other) const;
		/// @brief Оператор сравнения на неравенство
		bool operator!=(const BigNumber &other) const;
		/// @brief Оператор сравнения "меньше"
		bool operator<(const BigNumber &other) const;
		/// @brief Оператор сравнения "больше"
		bool operator>(const BigNumber &other) const;
		/// @brief Оператор сравнения "меньше или равно"
		bool operator<=(const BigNumber &other) const;
		/// @brief Оператор сравнения "больше или равно"
		bool operator>=(const BigNumber &other) const;

		/// @brief Оператор присваивания
		BigNumber &operator=(const BigNumber &other);

		/// @brief Оператор сложения
		BigNumber operator+(const BigNumber &other) const;
		/// @brief Оператор сложения с присваиванием
		BigNumber &operator+=(const BigNumber &other);

		/// @brief Оператор вычитания
		BigNumber operator-(const BigNumber &other) const;
		/// @brief Оператор вычитания с присваиванием
		BigNumber &operator-=(const BigNumber &other);

		/// @brief Оператор умножения на скаляр
		BigNumber operator*(const BaseType &value) const;
		/// @brief Оператор умножения на скаляр с присваиванием
		BigNumber &operator*=(const BaseType &value);

		/// @brief Оператор умножения на большое число
		BigNumber operator*(const BigNumber &other) const;
		/// @brief Оператор умножения на большое число с присваиванием
		BigNumber &operator*=(const BigNumber &other);

		/// @brief Оператор деления на скаляр
		BigNumber operator/(const BaseType &value) const;
		/// @brief Оператор остатка от деления на скаляр
		BigNumber operator%(const BaseType &value) const;

		/// @brief Оператор деления на большое число
		BigNumber operator/(const BigNumber &other) const;
		/// @brief Оператор остатка от деления на большое число
		BigNumber operator%(const BigNumber &other) const;

		/// @brief Вывод числа в 16-ричной системе
		void PrintHex() const;
		/// @brief Чтение числа в 16-ричной системе
		void ReadHex();

		/// @brief Оператор ввода из потока
		friend std::istream &operator>>(std::istream &in, BigNumber &number);
		/// @brief Оператор вывода в поток (вывод в десятичном виде)
		friend std::ostream &operator<<(std::ostream &out, const BigNumber number);

	private:
		// Статическая вспомогательная функция деления по алгоритму Кнута,
		// которая имеет доступ к защищённым членам класса.
		static std::pair<BigNumber, BigNumber> DivideKnuth(const BigNumber &u_orig, const BigNumber &v_orig);
	};

} // namespace big_number

#endif // BIG_NUMBER_HPP
