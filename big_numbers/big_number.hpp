#include <algorithm>
#include <cstring>
#include <ctime>
#include <iostream>
#include <limits>
#include <stdexcept>
#include <string>
#include <memory>

using BaseType = unsigned int;				   ///< Основной тип для хранения коэффициентов
using DoubleBaseType = unsigned long long int; ///< Тип для хранения удвоенных значений

constexpr int BASE_SIZE = sizeof(BaseType) * 8;		   ///< Размер BaseType в битах
constexpr int DBASE_SIZE = sizeof(DoubleBaseType) * 8; ///< Размер DoubleBaseType в битах

namespace big_number
{
	/// @brief Класс для работы с большими числами на основе массива коэффициентов
	class BigNumber
	{
	protected:
		std::unique_ptr<BaseType[]> coefficients; ///< Коэффициенты числа
		int length;								  ///< Фактическая длина числа (количество значащих коэффициентов)
		int maxLength;							  ///< Максимально возможная длина массива коэффициентов

	public:
		/// @brief Конструктор, создающий число с заданной максимальной длиной и начальным значением
		/// @param max_length Максимальная длина числа
		/// @param parameter Начальное значение (заполняет первый коэффициент)
		explicit BigNumber(int max_length = 1, int parameter = 0);

		/// @brief Конструктор копирования
		/// @param other Объект для копирования
		BigNumber(const BigNumber &other);

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

		/// @brief Устанавливает фактическую длину числа
		/// @param newLength Новое значение длины
		void SetLength(int newLength);

		/// @brief Устанавливает максимальную длину числа
		/// @param newMaxLength Новая максимальная длина
		void SetMaxLength(int newMaxLength);

		/// @brief Устанавливает новые коэффициенты
		/// @param newCoefficients Указатель на новый массив коэффициентов
		void SetCoefficients(BaseType *newCoefficients);

		/// @brief Нормализует длину числа, удаляя незначащие нули
		void NormalizeLength();

		/// @brief Оператор сравнения на равенство
		/// @param other Второй операнд
		/// @return true, если числа равны, иначе false
		bool operator==(const BigNumber &other) const;

		/// @brief Оператор сравнения на неравенство
		/// @param other Второй операнд
		/// @return true, если числа не равны, иначе false
		bool operator!=(const BigNumber &other) const;

		/// @brief Оператор сравнения "меньше"
		/// @param other Второй операнд
		/// @return true, если текущее число меньше, иначе false
		bool operator<(const BigNumber &other) const;

		/// @brief Оператор сравнения "больше"
		/// @param other Второй операнд
		/// @return true, если текущее число больше, иначе false
		bool operator>(const BigNumber &other) const;

		/// @brief Оператор сравнения "меньше или равно"
		/// @param other Второй операнд
		/// @return true, если текущее число меньше или равно, иначе false
		bool operator<=(const BigNumber &other) const;

		/// @brief Оператор сравнения "больше или равно"
		/// @param other Второй операнд
		/// @return true, если текущее число больше или равно, иначе false
		bool operator>=(const BigNumber &other) const;

		/// @brief Оператор присваивания
		/// @param other Второй операнд
		/// @return Ссылка на измененный объект
		BigNumber &operator=(const BigNumber &other);

		/// @brief Оператор сложения
		/// @param other Второй операнд
		/// @return Сумма двух чисел
		BigNumber operator+(const BigNumber &other) const;

		/// @brief Оператор сложения с присваиванием
		/// @param other Второй операнд
		/// @return Ссылка на измененный объект
		BigNumber &operator+=(const BigNumber &other);

		/// @brief Оператор вычитания
		/// @param other Второй операнд
		/// @return Разность двух чисел
		BigNumber operator-(const BigNumber &other) const;

		/// @brief Оператор вычитания с присваиванием
		/// @param other Второй операнд
		/// @return Ссылка на измененный объект
		BigNumber &operator-=(const BigNumber &other);

		/// @brief Оператор умножения на скаляр
		/// @param value Число, на которое умножаем
		/// @return Результат умножения
		BigNumber operator*(const BaseType &value) const;

		/// @brief Оператор умножения на скаляр с присваиванием
		/// @param value Число, на которое умножаем
		/// @return Ссылка на измененный объект
		BigNumber &operator*=(const BaseType &value);

		/// @brief Оператор умножения на большое число
		/// @param other Второй операнд
		/// @return Произведение двух чисел
		BigNumber operator*(const BigNumber &other) const;

		/// @brief Оператор умножения на большое число с присваиванием
		/// @param other Второй операнд
		/// @return Ссылка на измененный объект
		BigNumber &operator*=(const BigNumber &other);

		/// @brief Оператор деления на скаляр
		/// @param value Число, на которое делим
		/// @return Частное от деления
		BigNumber operator/(const BaseType &value) const;

		/// @brief Оператор остатка от деления на скаляр
		/// @param value Число, на которое делим
		/// @return Остаток от деления
		BigNumber operator%(const BaseType &value) const;

		/// @brief Оператор деления на большое число
		/// @param other Второй операнд
		/// @return Частное от деления
		BigNumber operator/(const BigNumber &other) const;

		/// @brief Оператор остатка от деления на большое число
		/// @param other Второй операнд
		/// @return Остаток от деления
		BigNumber operator%(const BigNumber &other) const;

		/// @brief Вывод числа в 16-ричной системе
		void PrintHex() const;

		/// @brief Чтение числа в 16-ричной системе
		void ReadHex();

		/// @brief Оператор ввода из потока
		/// @param in Входной поток
		/// @param number Число для ввода
		/// @return Измененный входной поток
		friend std::istream &operator>>(std::istream &in, BigNumber &number);

		/// @brief Оператор вывода в поток
		/// @param out Выходной поток
		/// @param number Число для вывода
		/// @return Измененный выходной поток
		friend std::ostream &operator<<(std::ostream &out, const BigNumber number);
	};

} // namespace big_number
