#include "big_number.hpp"
#include <memory>

namespace big_number
{

	void BigNumber::NormalizeLength()
	{
		while (length_ > 1 && coefficients_[length_ - 1] == 0)
		{
			--length_;
		}
	}
	// Конструктор
	BigNumber::BigNumber(int maxLen, int parameter) : length_(1), maxLength_(maxLen)
	{
		coefficients_.resize(maxLen, 0);
		// Если параметр не равен 0, инициализируем случайными значениями
		if (parameter != 0)
		{
			length_ = maxLength_;
			for (int i = 0; i < maxLength_; i++)
			{
				coefficients_[i] = rand();
			}
			if (sizeof(BaseType) == sizeof(unsigned int))
			{ // увеличинная генерация больших чтсел в основании с int
				for (int i = 0; i < maxLength_; ++i)
				{
					coefficients_[i] <<= 16;
					coefficients_[i] |= rand();
				}
			}
		}
	}

	// Конструктор копирования
	BigNumber::BigNumber(const BigNumber &other) : length_(other.length_), maxLength_(other.maxLength_)
	{
		coefficients_ = other.coefficients_;
	}

	int BigNumber::GetLength()
	{
		return length_;
	}

	int BigNumber::GetMaxLength()
	{
		return maxLength_;
	}

	BaseType *BigNumber::GetCoefficients()
	{
		return coefficients_.data();
	}

	void BigNumber::SetLength(int olength)
	{
		if (olength > maxLength_)
		{
			throw std::invalid_argument("New length exceeds maximum length.");
		}
		length_ = olength;
	}

	void BigNumber::SetMaxLength(int omaxLength)
	{
		if (omaxLength < length_)
		{
			throw std::invalid_argument("New maximum length is less than current length.");
		}
		maxLength_ = omaxLength;
		coefficients_.resize(maxLength_, 0);
	}

	void BigNumber::SetCoefficients(BaseType *ocoefficients)
	{
		std::copy(ocoefficients, ocoefficients + length_, coefficients_.begin());
	}
	// Оператор присваивания
	BigNumber &BigNumber::operator=(const BigNumber &other)
	{
		if (this != &other)
		{
			maxLength_ = other.maxLength_;
			length_ = other.length_;
			coefficients_ = other.coefficients_;
		}
		return *this;
	}

	// Печать числа в шестнадцатеричном формате
	void BigNumber::PrintHex() const
	{
		int i = length_ - 1;
		while (i >= 0)
		{
			std::cout.width(BASE_SIZE / 4);
			std::cout.fill('0');
			std::cout << std::hex << static_cast<int>(coefficients_[i]) << " ";
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
		length_ = (inputStringLength - 1) / (BASE_SIZE / 4) + 1;
		maxLength_ = length_;
		coefficients_.resize(maxLength_);
		int i = 0;
		while (i < length_)
		{
			coefficients_[i] = 0;
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

			coefficients_[j] |= temp << k;
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
		int maxOfLengths = std::max(length_, other.length_);
		int minOfLengths = std::min(length_, other.length_);
		int sumLength = maxOfLengths + 1; // Инициализация с учетом возможного переноса
		BigNumber sumNumber(sumLength);
		BaseType carry = 0;
		int i = 0;
		while (i < minOfLengths)
		{
			DoubleBaseType tempSum = (DoubleBaseType)coefficients_[i] + (DoubleBaseType)other.coefficients_[i] + carry;
			sumNumber.coefficients_[i] = (BaseType)tempSum;
			carry = tempSum >> BASE_SIZE;
			i++;
		}

		while (i < length_)
		{
			DoubleBaseType tempSum = (DoubleBaseType)coefficients_[i] + carry;
			sumNumber.coefficients_[i] = (BaseType)tempSum;
			carry = tempSum >> BASE_SIZE;
			i++;
		}

		while (i < other.length_)
		{
			DoubleBaseType tempSum = (DoubleBaseType)other.coefficients_[i] + carry;
			sumNumber.coefficients_[i] = (BaseType)tempSum;
			carry = tempSum >> BASE_SIZE;
			i++;
		}

		sumNumber.coefficients_[maxOfLengths] = carry; // Установка старшего разряда

		// Корректировка длины результата
		sumNumber.length_ = sumLength;
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
		BigNumber subtractionNum(length_);
		while (j < other.length_)
		{
			// Вычисляем разность с учетом заема
			temp = ((DoubleBaseType)1 << BASE_SIZE) | coefficients_[j];
			temp = temp - (DoubleBaseType)other.coefficients_[j] - borrow;

			// Записываем значение разности
			subtractionNum.coefficients_[j] = (BaseType)temp;

			// Определяем новый коэффициент заема
			borrow = !(temp >> BASE_SIZE);

			j++;
		}

		while (j < length_)
		{
			// Учитываем заем и вычитаем его
			temp = ((DoubleBaseType)1 << BASE_SIZE) | coefficients_[j];
			temp -= borrow;

			// Записываем значение
			subtractionNum.coefficients_[j] = (BaseType)temp;

			// Определяем новый коэффициент заема
			borrow = !(temp >> BASE_SIZE);

			j++;
		}

		// Корректируем фактическую длину результата
		subtractionNum.length_ = length_;
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
		BigNumber resNumber(length_ + 1); // Создаем объект для результата с максимально возможной длиной
		DoubleBaseType tmp;

		while (j < length_)
		{
			tmp = (DoubleBaseType)coefficients_[j] * (DoubleBaseType)multiplier + (DoubleBaseType)carry;
			resNumber.coefficients_[j] = (BaseType)tmp;
			carry = (BaseType)(tmp >> BASE_SIZE);
			j++;
		}
		resNumber.coefficients_[j] = carry;
		resNumber.length_ = length_ + 1;
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
		if (other.length_ == 1 && other.coefficients_[0] == 0)
		{
			return BigNumber();
		}
		BigNumber resNumber(length_ + other.length_);
		DoubleBaseType tmp;
		int j = 0;
		while (j < other.length_)
		{
			if (other.coefficients_[j] != 0)
			{
				BaseType carry = 0;
				int i = 0;
				while (i < length_)
				{
					tmp = (DoubleBaseType)coefficients_[i] * (DoubleBaseType)other.coefficients_[j] + (DoubleBaseType)resNumber.coefficients_[i + j] + (DoubleBaseType)carry;
					resNumber.coefficients_[i + j] = (BaseType)tmp;
					carry = (BaseType)(tmp >> BASE_SIZE);
					i++;
				}
				resNumber.coefficients_[length_ + j] = carry;
			}
			j++;
		}
		resNumber.length_ = length_ + other.length_;
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
	{
		if (number == 0)
		{
			throw std::invalid_argument("Division by zero.");
		}
		int j = 0;
		DoubleBaseType tmp = 0;
		BaseType left = 0;
		BigNumber resNumber(length_);
		while (j < length_)
		{
			tmp = ((DoubleBaseType)left << BASE_SIZE) + (DoubleBaseType)coefficients_[length_ - 1 - j];
			resNumber.coefficients_[length_ - 1 - j] = (BaseType)(tmp / (DoubleBaseType)number);
			left = (BaseType)(tmp % (DoubleBaseType)number);
			j++;
		}

		resNumber.length_ = length_;
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

		while (j < length_)
		{
			tmp = ((DoubleBaseType)left << BASE_SIZE) + (DoubleBaseType)coefficients_[length_ - 1 - j];
			left = (BaseType)(tmp % (DoubleBaseType)number);
			j++;
		}

		resNumber.coefficients_[0] = left;
		// resNumber.NormalizeLength();

		return resNumber;
	}
	BigNumber BigNumber::operator/(const BigNumber &divisor) const
	{
		if (divisor.length_ == 0 || (divisor.length_ == 1 && divisor.coefficients_[0] == 0))
		{
			throw std::invalid_argument("Division by zero.");
		}

		if (*this < divisor)
		{
			BigNumber result(1);
			return result;
		}

		if (divisor.length_ == 1)
		{
			return *this / divisor.coefficients_[0];
		}

		DoubleBaseType base = ((DoubleBaseType)1 << BASE_SIZE);
		DoubleBaseType d = base / (DoubleBaseType)(divisor.coefficients_[divisor.length_ - 1] + (BaseType)1);
		int j = length_ - divisor.length_;

		BigNumber dividend(*this);
		dividend *= d;
		BigNumber divisor_copy(divisor);
		divisor_copy *= d;

		BigNumber result(j + 1);
		result.length_ = j + 1;

		while (j >= 0)
		{
			DoubleBaseType q = (DoubleBaseType)(((DoubleBaseType)((DoubleBaseType)(dividend.coefficients_[j + divisor_copy.length_]) * (DoubleBaseType)(base)) + (DoubleBaseType)(dividend.coefficients_[j + divisor_copy.length_ - 1])) / (DoubleBaseType)(divisor_copy.coefficients_[divisor_copy.length_ - 1])); // можно ускорить сдвигом
			DoubleBaseType r = (DoubleBaseType)(((DoubleBaseType)((DoubleBaseType)(dividend.coefficients_[j + divisor_copy.length_]) * (DoubleBaseType)(base)) + (DoubleBaseType)(dividend.coefficients_[j + divisor_copy.length_ - 1])) % (DoubleBaseType)(divisor_copy.coefficients_[divisor_copy.length_ - 1]));

			if (q == base || (DoubleBaseType)((DoubleBaseType)(q) * (DoubleBaseType)(divisor_copy.coefficients_[divisor_copy.length_ - 2])) > (DoubleBaseType)(((DoubleBaseType)(base) * (DoubleBaseType)(r)) + (DoubleBaseType)(dividend.coefficients_[j + divisor_copy.length_ - 2])))
			{
				q--;
				r = (DoubleBaseType)(r) + (DoubleBaseType)(divisor_copy.coefficients_[divisor_copy.length_ - 1]);
				if ((DoubleBaseType)(r) < base)
				{
					if (q == base || (DoubleBaseType)((DoubleBaseType)(q) * (DoubleBaseType)(divisor_copy.coefficients_[divisor_copy.length_ - 2])) > (DoubleBaseType)(((DoubleBaseType)(base) * (DoubleBaseType)(r)) + (DoubleBaseType)(dividend.coefficients_[j + divisor_copy.length_ - 2])))
					{
						q--;
					}
				}
			}

			BigNumber u(divisor_copy.length_ + 1);
			u.length_ = divisor_copy.length_ + 1;
			for (int i = 0; i < divisor_copy.length_ + 1; i++)
			{
				u.coefficients_[i] = dividend.coefficients_[j + i];
			}

			if (u < divisor_copy * (BaseType)(q))
			{ // оптимизировать умножение
				q--;
			}

			u = u - divisor_copy * (BaseType)(q);
			result.coefficients_[j] = (BaseType)(q);

			for (int i = 0; i < divisor_copy.length_ + 1; i++)
			{
				dividend.coefficients_[j + i] = u.coefficients_[i];
			}

			j--;
		}

		result.NormalizeLength();

		return result;
	}

	BigNumber BigNumber::operator%(const BigNumber &divisor) const
	{
		if (divisor.length_ == 0 || (divisor.length_ == 1 && divisor.coefficients_[0] == 0))
		{
			throw std::invalid_argument("Division by zero.");
		}

		if (*this < divisor)
		{
			return *this;
		}

		if (divisor.length_ == 1)
		{
			return *this % divisor.coefficients_[0];
		}

		int result_len = length_ - divisor.length_;
		int base_size = BASE_SIZE;
		DoubleBaseType base = ((DoubleBaseType)1 << base_size);
		BaseType d = (BaseType)((DoubleBaseType)base / (DoubleBaseType)(divisor.coefficients_[divisor.length_ - 1] + 1));
		int j = result_len;
		int k = 0;

		BigNumber dividend(*this);
		dividend *= d;
		BigNumber divisor_copy(divisor);
		divisor_copy *= d;

		if (dividend.length_ == length_)
		{
			dividend.maxLength_++;
			dividend.length_ = maxLength_;
			dividend.coefficients_.resize(maxLength_);
			for (int i = 0; i < length_; i++)
			{
				dividend.coefficients_[i] = coefficients_[i];
			}
			dividend *= d;
			dividend.length_++;
			dividend.coefficients_[dividend.length_ - 1] = 0;
		}

		while (j >= 0)
		{
			DoubleBaseType q = (DoubleBaseType)(((DoubleBaseType)((DoubleBaseType)(dividend.coefficients_[j + divisor_copy.length_]) * (DoubleBaseType)(base)) + (DoubleBaseType)(dividend.coefficients_[j + divisor_copy.length_ - 1])) / (DoubleBaseType)(divisor_copy.coefficients_[divisor_copy.length_ - 1]));
			DoubleBaseType r = (DoubleBaseType)(((DoubleBaseType)((DoubleBaseType)(dividend.coefficients_[j + divisor_copy.length_]) * (DoubleBaseType)(base)) + (DoubleBaseType)(dividend.coefficients_[j + divisor_copy.length_ - 1])) % (DoubleBaseType)(divisor_copy.coefficients_[divisor_copy.length_ - 1]));

			if (q == base || (DoubleBaseType)((DoubleBaseType)(q) * (DoubleBaseType)(divisor_copy.coefficients_[divisor_copy.length_ - 2])) > (DoubleBaseType)(((DoubleBaseType)(base) * (DoubleBaseType)(r)) + (DoubleBaseType)(dividend.coefficients_[j + divisor_copy.length_ - 2])))
			{
				q--;
				r = (DoubleBaseType)(r) + (DoubleBaseType)(divisor_copy.coefficients_[divisor_copy.length_ - 1]);
				if ((DoubleBaseType)(r) < base)
				{
					if (q == base || (DoubleBaseType)((DoubleBaseType)(q) * (DoubleBaseType)(divisor_copy.coefficients_[divisor_copy.length_ - 2])) > (DoubleBaseType)(((DoubleBaseType)(base) * (DoubleBaseType)(r)) + (DoubleBaseType)(dividend.coefficients_[j + divisor_copy.length_ - 2])))
					{
						q--;
					}
				}
			}

			BigNumber u(divisor_copy.length_ + 1);
			u.length_ = divisor_copy.length_ + 1;
			for (int i = 0; i < divisor_copy.length_ + 1; i++)
			{
				u.coefficients_[i] = dividend.coefficients_[j + i];
			}

			if (u < divisor_copy * (BaseType)(q))
			{
				q--;
			}

			u = u - (divisor_copy * (BaseType)(q));

			for (int i = 0; i < divisor_copy.length_ + 1; i++)
			{
				dividend.coefficients_[j + i] = u.coefficients_[i];
			}

			j--;
		}

		dividend.NormalizeLength();

		return dividend / d;
	}

	bool BigNumber::operator==(const BigNumber &other) const
	{
		if (length_ != other.length_)
		{
			return false;
		}
		for (int i = 0; i < length_; ++i)
		{
			if (coefficients_[i] != other.coefficients_[i])
			{
				return false;
			}
		}
		return true;
	}

	bool BigNumber::operator!=(const BigNumber &other) const
	{
		if (length_ != other.length_)
		{
			return true;
		}
		for (int i = 0; i < length_; ++i)
		{
			if (coefficients_[i] != other.coefficients_[i])
			{
				return true;
			}
		}
		return false;
	}

	bool BigNumber::operator<(const BigNumber &other) const
	{
		if (length_ < other.length_)
		{
			return true;
		}
		if (length_ > other.length_)
		{
			return false;
		}
		for (int i = length_ - 1; i >= 0; --i)
		{
			if (coefficients_[i] < other.coefficients_[i])
			{
				return true;
			}
			if (coefficients_[i] > other.coefficients_[i])
			{
				return false;
			}
		}
		return false;
	}

	bool BigNumber::operator>(const BigNumber &other) const
	{
		if (length_ > other.length_)
		{
			return true;
		}

		if (length_ < other.length_)
		{
			return false;
		}
		for (int i = length_ - 1; i >= 0; --i)
		{
			if (coefficients_[i] > other.coefficients_[i])
			{
				return true;
			}
			if (coefficients_[i] < other.coefficients_[i])
			{
				return false;
			}
		}
		return false;
	}

	bool BigNumber::operator<=(const BigNumber &other) const
	{
		if (length_ > other.length_)
		{
			return false;
		}
		if (length_ < other.length_)
		{
			return true;
		}
		for (int i = length_ - 1; i >= 0; --i)
		{
			if (coefficients_[i] < other.coefficients_[i])
			{
				return true;
			}
			if (coefficients_[i] > other.coefficients_[i])
			{
				return false;
			}
		}
		return true;
	}

	bool BigNumber::operator>=(const BigNumber &other) const
	{
		if (length_ < other.length_)
		{
			return false;
		}
		if (length_ > other.length_)
		{
			return true;
		}
		for (int i = length_ - 1; i >= 0; --i)
		{
			if (coefficients_[i] > other.coefficients_[i])
			{
				return true;
			}
			if (coefficients_[i] < other.coefficients_[i])
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
			newNumCoefficients[0] = (BaseType)t;
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