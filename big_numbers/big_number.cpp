#include "big_number.hpp"
#include <memory>

namespace big_number
{

	void BigNumber::NormalizeLength()
	{
		while (length > 1 && coefficients[length - 1] == 0)
		{
			--length;
		}
	}
	// Конструктор
	BigNumber::BigNumber(int maxLen, int parameter) : length(1), maxLength(maxLen)
	{
		coefficients = std::make_unique<BaseType[]>(maxLen);
		std::fill(coefficients.get(), coefficients.get() + maxLen, 0);
		// Если параметр не равен 0, инициализируем случайными значениями
		if (parameter != 0)
		{
			length = maxLength;
			for (int i = 0; i < maxLength; i++)
			{
				coefficients[i] = rand();
			}
			if (sizeof(BaseType) == sizeof(unsigned int))
			{ // увеличинная генерация больших чтсел в основании с int
				for (int i = 0; i < maxLength; ++i)
				{
					coefficients[i] <<= 16;
					coefficients[i] |= rand();
				}
			}
		}
	}

	// Конструктор копирования
	BigNumber::BigNumber(const BigNumber &other) : length(other.length), maxLength(other.maxLength)
	{
		coefficients = std::make_unique<BaseType[]>(maxLength);
		std::copy(other.coefficients.get(), other.coefficients.get() + length, coefficients.get());
	}

	int BigNumber::GetLength()
	{
		return length;
	}

	int BigNumber::GetMaxLength()
	{
		return maxLength;
	}

	BaseType *BigNumber::GetCoefficients()
	{
		return coefficients.get();
	}

	void BigNumber::SetLength(int olength)
	{
		length = olength;
	}

	void BigNumber::SetMaxLength(int omaxLength)
	{
		maxLength = omaxLength;
	}

	void BigNumber::SetCoefficients(BaseType *ocoefficients)
	{
		std::copy(ocoefficients, ocoefficients + length, coefficients.get());
	}
	// Оператор присваивания
	BigNumber &BigNumber::operator=(const BigNumber &other)
	{
		if (this != &other)
		{
			maxLength = other.maxLength;
			length = other.length;
			coefficients = std::make_unique<BaseType[]>(maxLength);
			std::copy(other.coefficients.get(), other.coefficients.get() + length, coefficients.get());
		}
		return *this;
	}

	// Печать числа в шестнадцатеричном формате
	void BigNumber::PrintHex() const
	{
		int i = length - 1;
		while (i >= 0)
		{
			std::cout.width(BASE_SIZE / 4);
			std::cout.fill('0');
			std::cout << std::hex << static_cast<int>(coefficients[i]) << " ";
			i--;
		}
	}

	// Ввод числа в шестнадцатеричном формате
	void BigNumber::ReadHex()
	{
		std::string inputString;
		std::getline(std::cin, inputString);
		int inputStringLength = inputString.length();
		int k = 0, j = 0;
		length = (inputStringLength - 1) / (BASE_SIZE / 4) + 1;
		maxLength = length;
		coefficients = std::make_unique<BaseType[]>(maxLength);
		int i = 0;
		while (i < length)
		{
			coefficients[i] = 0;
			i++;
		}

		// Преобразование строки в шестнадцатеричные коэффициенты
		int idx = inputStringLength - 1;
		while (idx >= 0)
		{
			unsigned int temp = 0;
			if ('0' <= inputString[idx] && inputString[idx] <= '9')
			{
				temp = inputString[idx] - '0';
			}
			else if ('a' <= inputString[idx] && inputString[idx] <= 'f')
			{
				temp = inputString[idx] - 'a' + 10;
			}
			else if ('A' <= inputString[idx] && inputString[idx] <= 'F')
			{
				temp = inputString[idx] - 'A' + 10;
			}
			else
			{
				throw std::invalid_argument("Invalid arguments.");
			}

			coefficients[j] |= temp << k;
			k += 4;
			if (k >= BASE_SIZE)
			{
				k = 0;
				j++;
			}
			idx--;
		}
		NormalizeLength();
	}

	// Оператор сложения
	BigNumber BigNumber::operator+(const BigNumber &other) const
	{
		int maxOfLengths = std::max(length, other.length);
		int minOfLengths = std::min(length, other.length);
		int sumLength = maxOfLengths + 1; // Инициализация с учетом возможного переноса
		BigNumber sumNumber(sumLength);
		BaseType carry = 0;
		int i = 0;
		while (i < minOfLengths)
		{
			DoubleBaseType tempSum = (DoubleBaseType)coefficients[i] + (DoubleBaseType)other.coefficients[i] + carry;
			sumNumber.coefficients[i] = (BaseType)tempSum;
			carry = tempSum >> BASE_SIZE;
			i++;
		}

		while (i < length)
		{
			DoubleBaseType tempSum = (DoubleBaseType)coefficients[i] + carry;
			sumNumber.coefficients[i] = (BaseType)tempSum;
			carry = tempSum >> BASE_SIZE;
			i++;
		}

		while (i < other.length)
		{
			DoubleBaseType tempSum = (DoubleBaseType)other.coefficients[i] + carry;
			sumNumber.coefficients[i] = (BaseType)tempSum;
			carry = tempSum >> BASE_SIZE;
			i++;
		}

		sumNumber.coefficients[maxOfLengths] = carry; // Установка старшего разряда

		// Корректировка длины результата
		sumNumber.length = sumLength;
		sumNumber.NormalizeLength();

		return sumNumber;
	}

	// Оператор сложения с присваиванием
	BigNumber &BigNumber::operator+=(const BigNumber &other)
	{
		*this = *this + other;
		return *this;
	}

	// Оператор вычитания
	BigNumber BigNumber::operator-(const BigNumber &other) const
	{
		if (*this < other)
		{
			throw std::invalid_argument("Invalid argument"); // Если текущее число меньше другого, выбрасываем исключение
		}

		int j = 0;
		int borrow = 0; // Коэффициент заема
		DoubleBaseType temp;
		BigNumber subtractionNum(length);
		while (j < other.length)
		{
			// Вычисляем разность с учетом заема
			temp = ((DoubleBaseType)1 << BASE_SIZE) | coefficients[j];
			temp = temp - (DoubleBaseType)other.coefficients[j] - borrow;

			// Записываем значение разности
			subtractionNum.coefficients[j] = (BaseType)temp;

			// Определяем новый коэффициент заема
			borrow = !(temp >> BASE_SIZE);

			j++;
		}

		while (j < length)
		{
			// Учитываем заем и вычитаем его
			temp = ((DoubleBaseType)1 << BASE_SIZE) | coefficients[j];
			temp -= borrow;

			// Записываем значение
			subtractionNum.coefficients[j] = (BaseType)temp;

			// Определяем новый коэффициент заема
			borrow = !(temp >> BASE_SIZE);

			j++;
		}

		// Корректируем фактическую длину результата
		subtractionNum.length = length;
		subtractionNum.NormalizeLength();

		return subtractionNum; // Возвращаем результат
	}

	// Оператор вычитания с присваиванием
	BigNumber &BigNumber::operator-=(const BigNumber &other)
	{
		*this = *this - other;
		return *this;
	}

	// Умножение на отдельное значение типа BaseType
	BigNumber BigNumber::operator*(const BaseType &multiplier) const
	{
		int j = 0;
		BaseType carry = 0;
		BigNumber resNumber(length + 1); // Создаем объект для результата с максимально возможной длиной
		DoubleBaseType tmp;

		while (j < length)
		{
			tmp = (DoubleBaseType)coefficients[j] * (DoubleBaseType)multiplier + (DoubleBaseType)carry;
			resNumber.coefficients[j] = (BaseType)tmp;
			carry = (BaseType)(tmp >> BASE_SIZE);
			j++;
		}
		resNumber.coefficients[j] = carry;
		resNumber.length = length + 1;
		resNumber.NormalizeLength();
		return resNumber;
	}

	// Оператор умножения с присваиванием для другого объекта BigNumber
	BigNumber &BigNumber::operator*=(const BaseType &multiplier)
	{
		*this = *this * multiplier; // Используем операцию умножения для текущего объекта и другого BigNumber
		return *this;
	}

	// Умножение на другой объект BigNumber
	BigNumber BigNumber::operator*(const BigNumber &other) const
	{
		if (other.length == 1 && other.coefficients[0] == 0)
		{
			return BigNumber();
		}
		BigNumber resNumber(length + other.length);
		DoubleBaseType tmp;
		int j = 0;
		while (j < other.length)
		{
			if (other.coefficients[j] != 0)
			{
				BaseType carry = 0;
				int i = 0;
				while (i < length)
				{
					tmp = (DoubleBaseType)coefficients[i] * (DoubleBaseType)other.coefficients[j] + (DoubleBaseType)resNumber.coefficients[i + j] + (DoubleBaseType)carry;
					resNumber.coefficients[i + j] = (BaseType)tmp;
					carry = (BaseType)(tmp >> BASE_SIZE);
					i++;
				}
				resNumber.coefficients[length + j] = carry;
			}
			j++;
		}
		resNumber.length = length + other.length;
		resNumber.NormalizeLength();
		return resNumber;
	}

	// Оператор умножения с присваиванием для отдельного значения BaseType
	BigNumber &BigNumber::operator*=(const BigNumber &other)
	{
		*this = *this * other;
		return *this;
	}

	// Оператор деления на BaseType
	BigNumber BigNumber::operator/(const BaseType &number) const
	{ // переопределить j
		int j = 0;
		DoubleBaseType tmp = 0;
		BaseType left = 0;
		BigNumber resNumber(length);
		while (j < length)
		{
			tmp = ((DoubleBaseType)left << BASE_SIZE) + (DoubleBaseType)coefficients[length - 1 - j];
			resNumber.coefficients[length - 1 - j] = (BaseType)(tmp / (DoubleBaseType)number);
			left = (BaseType)(tmp % (DoubleBaseType)number);
			j++;
		}

		resNumber.length = length;
		resNumber.NormalizeLength();
		return resNumber;
	}

	// Оператор взятия остатка от деления на BaseType
	BigNumber BigNumber::operator%(const BaseType &number) const
	{
		int j = 0;
		DoubleBaseType tmp = 0;
		BaseType left = 0;
		BigNumber resNumber(1);

		while (j < length)
		{
			tmp = ((DoubleBaseType)left << BASE_SIZE) + (DoubleBaseType)coefficients[length - 1 - j];
			left = (BaseType)(tmp % (DoubleBaseType)number);
			j++;
		}

		resNumber.coefficients[0] = left;
		// resNumber.NormalizeLength();

		return resNumber;
	}
	BigNumber BigNumber::operator/(const BigNumber &divisor) const
	{
		if (divisor.length == 1 && divisor.coefficients[0] == 0)
		{
			throw std::invalid_argument("Division by zero.");
		}

		if (*this < divisor)
		{
			BigNumber result(1);
			return result;
		}

		if (divisor.length == 1)
		{
			return *this / divisor.coefficients[0];
		}

		DoubleBaseType base = ((DoubleBaseType)1 << BASE_SIZE);
		DoubleBaseType d = base / (DoubleBaseType)(divisor.coefficients[divisor.length - 1] + (BaseType)1);
		int j = length - divisor.length;

		BigNumber dividend(*this);
		dividend *= d;
		BigNumber divisor_copy(divisor);
		divisor_copy *= d;

		BigNumber result(j + 1);
		result.length = j + 1;

		while (j >= 0)
		{
			DoubleBaseType q = (DoubleBaseType)(((DoubleBaseType)((DoubleBaseType)(dividend.coefficients[j + divisor_copy.length]) * (DoubleBaseType)(base)) + (DoubleBaseType)(dividend.coefficients[j + divisor_copy.length - 1])) / (DoubleBaseType)(divisor_copy.coefficients[divisor_copy.length - 1])); // можно ускорить сдвигом
			DoubleBaseType r = (DoubleBaseType)(((DoubleBaseType)((DoubleBaseType)(dividend.coefficients[j + divisor_copy.length]) * (DoubleBaseType)(base)) + (DoubleBaseType)(dividend.coefficients[j + divisor_copy.length - 1])) % (DoubleBaseType)(divisor_copy.coefficients[divisor_copy.length - 1]));

			if (q == base || (DoubleBaseType)((DoubleBaseType)(q) * (DoubleBaseType)(divisor_copy.coefficients[divisor_copy.length - 2])) > (DoubleBaseType)(((DoubleBaseType)(base) * (DoubleBaseType)(r)) + (DoubleBaseType)(dividend.coefficients[j + divisor_copy.length - 2])))
			{
				q--;
				r = (DoubleBaseType)(r) + (DoubleBaseType)(divisor_copy.coefficients[divisor_copy.length - 1]);
				if ((DoubleBaseType)(r) < base)
				{
					if (q == base || (DoubleBaseType)((DoubleBaseType)(q) * (DoubleBaseType)(divisor_copy.coefficients[divisor_copy.length - 2])) > (DoubleBaseType)(((DoubleBaseType)(base) * (DoubleBaseType)(r)) + (DoubleBaseType)(dividend.coefficients[j + divisor_copy.length - 2])))
					{
						q--;
					}
				}
			}

			BigNumber u(divisor_copy.length + 1);
			u.length = divisor_copy.length + 1;
			for (int i = 0; i < divisor_copy.length + 1; i++)
			{
				u.coefficients[i] = dividend.coefficients[j + i];
			}

			if (u < divisor_copy * (BaseType)(q))
			{ // оптимизировать умножение
				q--;
			}

			u = u - divisor_copy * (BaseType)(q);
			result.coefficients[j] = (BaseType)(q);

			for (int i = 0; i < divisor_copy.length + 1; i++)
			{
				dividend.coefficients[j + i] = u.coefficients[i];
			}

			j--;
		}

		result.NormalizeLength();

		return result;
	}

	BigNumber BigNumber::operator%(const BigNumber &divisor) const
	{
		if (divisor.length == 1 && divisor.coefficients[0] == 0)
		{
			throw std::invalid_argument("Division by zero.");
		}

		if (*this < divisor)
		{
			return *this;
		}

		if (divisor.length == 1)
		{
			return *this % divisor.coefficients[0];
		}

		int result_len = length - divisor.length;
		int base_size = BASE_SIZE;
		DoubleBaseType base = ((DoubleBaseType)1 << base_size);
		BaseType d = (BaseType)((DoubleBaseType)base / (DoubleBaseType)(divisor.coefficients[divisor.length - 1] + 1));
		int j = result_len;
		int k = 0;

		BigNumber dividend(*this);
		dividend *= d;
		BigNumber divisor_copy(divisor);
		divisor_copy *= d;

		if (dividend.length == length)
		{
			dividend.maxLength++;
			dividend.length = maxLength;
			dividend.coefficients = std::make_unique<BaseType[]>(maxLength);
			for (int i = 0; i < length; i++)
			{
				dividend.coefficients[i] = coefficients[i];
			}
			dividend *= d;
			dividend.length++;
			dividend.coefficients[dividend.length - 1] = 0;
		}

		while (j >= 0)
		{
			DoubleBaseType q = (DoubleBaseType)(((DoubleBaseType)((DoubleBaseType)(dividend.coefficients[j + divisor_copy.length]) * (DoubleBaseType)(base)) + (DoubleBaseType)(dividend.coefficients[j + divisor_copy.length - 1])) / (DoubleBaseType)(divisor_copy.coefficients[divisor_copy.length - 1]));
			DoubleBaseType r = (DoubleBaseType)(((DoubleBaseType)((DoubleBaseType)(dividend.coefficients[j + divisor_copy.length]) * (DoubleBaseType)(base)) + (DoubleBaseType)(dividend.coefficients[j + divisor_copy.length - 1])) % (DoubleBaseType)(divisor_copy.coefficients[divisor_copy.length - 1]));

			if (q == base || (DoubleBaseType)((DoubleBaseType)(q) * (DoubleBaseType)(divisor_copy.coefficients[divisor_copy.length - 2])) > (DoubleBaseType)(((DoubleBaseType)(base) * (DoubleBaseType)(r)) + (DoubleBaseType)(dividend.coefficients[j + divisor_copy.length - 2])))
			{
				q--;
				r = (DoubleBaseType)(r) + (DoubleBaseType)(divisor_copy.coefficients[divisor_copy.length - 1]);
				if ((DoubleBaseType)(r) < base)
				{
					if (q == base || (DoubleBaseType)((DoubleBaseType)(q) * (DoubleBaseType)(divisor_copy.coefficients[divisor_copy.length - 2])) > (DoubleBaseType)(((DoubleBaseType)(base) * (DoubleBaseType)(r)) + (DoubleBaseType)(dividend.coefficients[j + divisor_copy.length - 2])))
					{
						q--;
					}
				}
			}

			BigNumber u(divisor_copy.length + 1);
			u.length = divisor_copy.length + 1;
			for (int i = 0; i < divisor_copy.length + 1; i++)
			{
				u.coefficients[i] = dividend.coefficients[j + i];
			}

			if (u < divisor_copy * (BaseType)(q))
			{
				q--;
			}

			u = u - (divisor_copy * (BaseType)(q));

			for (int i = 0; i < divisor_copy.length + 1; i++)
			{
				dividend.coefficients[j + i] = u.coefficients[i];
			}

			j--;
		}

		dividend.NormalizeLength();

		return dividend / d;
	}

	bool BigNumber::operator==(const BigNumber &other) const
	{
		if (length != other.length)
		{
			return false;
		}
		for (int i = 0; i < length; ++i)
		{
			if (coefficients[i] != other.coefficients[i])
			{
				return false;
			}
		}
		return true;
	}

	bool BigNumber::operator!=(const BigNumber &other) const
	{
		if (length != other.length)
		{
			return true;
		}
		for (int i = 0; i < length; ++i)
		{
			if (coefficients[i] != other.coefficients[i])
			{
				return true;
			}
		}
		return false;
	}

	bool BigNumber::operator<(const BigNumber &other) const
	{
		if (length < other.length)
		{
			return true;
		}
		if (length > other.length)
		{
			return false;
		}
		for (int i = length - 1; i >= 0; --i)
		{
			if (coefficients[i] < other.coefficients[i])
			{
				return true;
			}
			if (coefficients[i] > other.coefficients[i])
			{
				return false;
			}
		}
		return false;
	}

	bool BigNumber::operator>(const BigNumber &other) const
	{
		if (length > other.length)
		{
			return true;
		}

		if (length < other.length)
		{
			return false;
		}
		for (int i = length - 1; i >= 0; --i)
		{
			if (coefficients[i] > other.coefficients[i])
			{
				return true;
			}
			if (coefficients[i] < other.coefficients[i])
			{
				return false;
			}
		}
		return false;
	}

	bool BigNumber::operator<=(const BigNumber &other) const
	{
		if (length > other.length)
		{
			return false;
		}
		if (length < other.length)
		{
			return true;
		}
		for (int i = length - 1; i >= 0; --i)
		{
			if (coefficients[i] < other.coefficients[i])
			{
				return true;
			}
			if (coefficients[i] > other.coefficients[i])
			{
				return false;
			}
		}
		return true;
	}

	bool BigNumber::operator>=(const BigNumber &other) const
	{
		if (length < other.length)
		{
			return false;
		}
		if (length > other.length)
		{
			return true;
		}
		for (int i = length - 1; i >= 0; --i)
		{
			if (coefficients[i] > other.coefficients[i])
			{
				return true;
			}
			if (coefficients[i] < other.coefficients[i])
			{
				return false;
			}
		}
		return true;
	}

	std::istream &operator>>(std::istream &in, BigNumber &bNum)
	{
		int j = 0;
		std::string inputString;
		std::getline(in, inputString);
		int inputStringLength = inputString.length();
		BaseType t = 0;

		BigNumber temp((inputStringLength - 1) / (BASE_SIZE / 4) + 1);

		while (j < inputStringLength)
		{
			if ('0' > inputString[j] || inputString[j] > '9')
			{
				throw std::invalid_argument("Invalid arguments");
			}
			t = inputString[j] - '0';
			temp = temp * ((BaseType)10);

			BigNumber newNum;
			auto newNumCoefficients = newNum.GetCoefficients();
			newNum.GetCoefficients()[0] = (BaseType)t;
			temp += newNum;
			j++;
		}

		temp.SetLength(temp.GetMaxLength());
		temp.NormalizeLength();

		bNum = temp;

		return in;
	}

	std::ostream &operator<<(std::ostream &out, BigNumber bN)
	{
		BigNumber newNum = bN; // Используем переданный объект bN
		BigNumber zero(newNum.GetLength());
		std::string s;
		while (newNum != zero)
		{
			BigNumber t = newNum % 10;
			s.push_back(t.GetCoefficients()[0] + '0');
			newNum = newNum / 10;
		}
		reverse(s.begin(), s.end());
		out << s; // Выводим строку s в поток out
		return out;
	}
}