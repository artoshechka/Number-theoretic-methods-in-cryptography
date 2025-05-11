#include "big_number.hpp"
#include <sstream>

namespace big_number
{

// Конструктор: если parameter не равен 0, записываем его в первый коэффициент
BigNumber::BigNumber(int max_length, int parameter) : length_(1), maxLength_(max_length)
{
    coefficients_.resize(maxLength_, 0);
    if (parameter != 0)
    {
        coefficients_[0] = static_cast<BaseType>(parameter);
    }
}

// Конструктор копирования
BigNumber::BigNumber(const BigNumber &other)
    : coefficients_(other.coefficients_), length_(other.length_), maxLength_(other.maxLength_)
{
}

// Конструктор из строки (десятичное представление)
BigNumber::BigNumber(const std::string &s)
    : BigNumber(s.size()) // используем конструктор с max_length, приблизительно равным размеру строки
{
    std::istringstream iss(s);
    iss >> *this;
}

// Новый конструктор из числа
BigNumber::BigNumber(unsigned long long value) : BigNumber(1, 0) // инициализируем нулём; maxLength_ изначально равен 1
{
    // Очищаем вектор и заполняем коэффициенты, пока value > 0.
    coefficients_.clear();
    if (value == 0)
    {
        coefficients_.push_back(0);
        length_ = 1;
        maxLength_ = 1;
    }
    else
    {
        DoubleBaseType base = static_cast<DoubleBaseType>(1) << BASE_SIZE;
        while (value > 0)
        {
            BaseType coef = static_cast<BaseType>(value % base);
            coefficients_.push_back(coef);
            value /= base;
        }
        length_ = static_cast<int>(coefficients_.size());
        maxLength_ = length_;
    }
}

// Геттеры
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

// Сеттеры
void BigNumber::SetLength(int newLength)
{
    if (newLength > maxLength_)
    {
        throw std::invalid_argument("New length exceeds maximum length.");
    }
    length_ = newLength;
}

void BigNumber::SetMaxLength(int newMaxLength)
{
    if (newMaxLength < length_)
    {
        throw std::invalid_argument("New maximum length is less than current length.");
    }
    maxLength_ = newMaxLength;
    coefficients_.resize(maxLength_, 0);
}

void BigNumber::SetCoefficient(int index, BaseType value)
{
    if (index >= 0 && index < length_)
    {
        coefficients_[index] = value;
    }
}

// Удаляет незначащие нули (старшие коэффициенты, равные 0)
void BigNumber::NormalizeLength()
{
    while (length_ > 1 && coefficients_[length_ - 1] == 0)
        --length_;
}

// Операторы сравнения
bool BigNumber::operator==(const BigNumber &other) const
{
    if (length_ != other.length_)
        return false;
    for (int i = 0; i < length_; ++i)
    {
        if (coefficients_[i] != other.coefficients_[i])
            return false;
    }
    return true;
}

bool BigNumber::operator!=(const BigNumber &other) const
{
    return !(*this == other);
}

bool BigNumber::operator<(const BigNumber &other) const
{
    if (length_ != other.length_)
        return length_ < other.length_;
    for (int i = length_ - 1; i >= 0; --i)
    {
        if (coefficients_[i] != other.coefficients_[i])
            return coefficients_[i] < other.coefficients_[i];
    }
    return false;
}

bool BigNumber::operator>(const BigNumber &other) const
{
    return other < *this;
}

bool BigNumber::operator<=(const BigNumber &other) const
{
    return !(*this > other);
}

bool BigNumber::operator>=(const BigNumber &other) const
{
    return !(*this < other);
}

// Оператор присваивания
BigNumber &BigNumber::operator=(const BigNumber &other)
{
    if (this != &other)
    {
        coefficients_ = other.coefficients_;
        length_ = other.length_;
        maxLength_ = other.maxLength_;
    }
    return *this;
}

// Операция сложения
BigNumber BigNumber::operator+(const BigNumber &other) const
{
    int maxLen = std::max(length_, other.length_);
    BigNumber result(maxLen + 1);
    BaseType carry = 0;
    int i = 0;
    for (; i < std::min(length_, other.length_); ++i)
    {
        DoubleBaseType sum = static_cast<DoubleBaseType>(coefficients_[i]) + other.coefficients_[i] + carry;
        result.coefficients_[i] = static_cast<BaseType>(sum);
        carry = static_cast<BaseType>(sum >> BASE_SIZE);
    }
    while (i < length_)
    {
        DoubleBaseType sum = static_cast<DoubleBaseType>(coefficients_[i]) + carry;
        result.coefficients_[i] = static_cast<BaseType>(sum);
        carry = static_cast<BaseType>(sum >> BASE_SIZE);
        ++i;
    }
    while (i < other.length_)
    {
        DoubleBaseType sum = static_cast<DoubleBaseType>(other.coefficients_[i]) + carry;
        result.coefficients_[i] = static_cast<BaseType>(sum);
        carry = static_cast<BaseType>(sum >> BASE_SIZE);
        ++i;
    }
    result.coefficients_[maxLen] = carry;
    result.length_ = maxLen + 1;
    result.NormalizeLength();
    return result;
}

BigNumber &BigNumber::operator+=(const BigNumber &other)
{
    *this = *this + other;
    return *this;
}

// Операция вычитания (предполагается, что *this >= other)
BigNumber BigNumber::operator-(const BigNumber &other) const
{
    if (*this < other)
        throw std::invalid_argument("Subtraction result would be negative.");
    BigNumber result(length_);
    int borrow = 0;
    int i = 0;
    for (; i < other.length_; ++i)
    {
        DoubleBaseType temp = (static_cast<DoubleBaseType>(1) << BASE_SIZE) | coefficients_[i];
        temp = temp - other.coefficients_[i] - borrow;
        result.coefficients_[i] = static_cast<BaseType>(temp);
        borrow = (temp >> BASE_SIZE) ? 0 : 1;
    }
    while (i < length_)
    {
        DoubleBaseType temp = (static_cast<DoubleBaseType>(1) << BASE_SIZE) | coefficients_[i];
        temp -= borrow;
        result.coefficients_[i] = static_cast<BaseType>(temp);
        borrow = (temp >> BASE_SIZE) ? 0 : 1;
        ++i;
    }
    result.length_ = length_;
    result.NormalizeLength();
    return result;
}

BigNumber &BigNumber::operator-=(const BigNumber &other)
{
    *this = *this - other;
    return *this;
}

// Умножение на скаляр
BigNumber BigNumber::operator*(const BaseType &value) const
{
    BigNumber result(length_ + 1);
    BaseType carry = 0;
    int i = 0;
    for (; i < length_; ++i)
    {
        DoubleBaseType prod = static_cast<DoubleBaseType>(coefficients_[i]) * value + carry;
        result.coefficients_[i] = static_cast<BaseType>(prod);
        carry = static_cast<BaseType>(prod >> BASE_SIZE);
    }
    result.coefficients_[i] = carry;
    result.length_ = length_ + 1;
    result.NormalizeLength();
    return result;
}

BigNumber &BigNumber::operator*=(const BaseType &value)
{
    *this = *this * value;
    return *this;
}

// Умножение на большое число (школьный алгоритм)
BigNumber BigNumber::operator*(const BigNumber &other) const
{
    if ((other.length_ == 1) && (other.coefficients_[0] == 0))
        return BigNumber();
    BigNumber result(length_ + other.length_);
    for (int j = 0; j < other.length_; ++j)
    {
        if (other.coefficients_[j] != 0)
        {
            BaseType carry = 0;
            for (int i = 0; i < length_; ++i)
            {
                DoubleBaseType prod = static_cast<DoubleBaseType>(coefficients_[i]) * other.coefficients_[j] +
                                      result.coefficients_[i + j] + carry;
                result.coefficients_[i + j] = static_cast<BaseType>(prod);
                carry = static_cast<BaseType>(prod >> BASE_SIZE);
            }
            result.coefficients_[length_ + j] = carry;
        }
    }
    result.length_ = length_ + other.length_;
    result.NormalizeLength();
    return result;
}

BigNumber &BigNumber::operator*=(const BigNumber &other)
{
    *this = *this * other;
    return *this;
}

// Деление на скаляр
BigNumber BigNumber::operator/(const BaseType &value) const
{
    if (value == 0)
        throw std::invalid_argument("Division by zero.");
    BigNumber result(length_);
    BaseType rem = 0;
    for (int i = length_ - 1; i >= 0; --i)
    {
        DoubleBaseType cur = (static_cast<DoubleBaseType>(rem) << BASE_SIZE) + coefficients_[i];
        result.coefficients_[i] = static_cast<BaseType>(cur / value);
        rem = static_cast<BaseType>(cur % value);
    }
    result.length_ = length_;
    result.NormalizeLength();
    return result;
}

// Остаток от деления на скаляр
BigNumber BigNumber::operator%(const BaseType &value) const
{
    if (value == 0)
        throw std::invalid_argument("Division by zero.");
    BaseType rem = 0;
    for (int i = length_ - 1; i >= 0; --i)
    {
        DoubleBaseType cur = (static_cast<DoubleBaseType>(rem) << BASE_SIZE) + coefficients_[i];
        rem = static_cast<BaseType>(cur % value);
    }
    BigNumber result(1);
    result.coefficients_[0] = rem;
    result.length_ = 1;
    return result;
}

// -------------------------
// Реализация алгоритма Кнута для деления (DivideKnuth)
// -------------------------
std::pair<BigNumber, BigNumber> BigNumber::DivideKnuth(const BigNumber &u_orig, const BigNumber &v_orig)
{
    if ((v_orig.length_ == 1) && (v_orig.coefficients_[0] == 0))
        throw std::invalid_argument("Division by zero.");

    if (u_orig < v_orig)
        return {BigNumber(u_orig.maxLength_), u_orig};

    if (v_orig.length_ == 1)
        return {u_orig / v_orig.coefficients_[0], u_orig % v_orig.coefficients_[0]};

    int n = v_orig.length_;
    int m = u_orig.length_ - n;
    DoubleBaseType base = (static_cast<DoubleBaseType>(1) << BASE_SIZE);

    BaseType d = static_cast<BaseType>(base / (v_orig.coefficients_[n - 1] + 1));
    BigNumber u = u_orig * d;
    BigNumber v = v_orig * d;

    if (u.length_ == u_orig.length_)
    {
        u.coefficients_.push_back(0);
        u.length_++;
        u.maxLength_++;
    }

    BigNumber Q(m + 1);
    Q.length_ = m + 1;

    for (int j = m; j >= 0; --j)
    {
        DoubleBaseType numerator =
            (static_cast<DoubleBaseType>(u.coefficients_[j + n]) << BASE_SIZE) + u.coefficients_[j + n - 1];
        BaseType qhat = static_cast<BaseType>(numerator / v.coefficients_[n - 1]);
        BaseType rhat = static_cast<BaseType>(numerator % v.coefficients_[n - 1]);

        while ((qhat == static_cast<BaseType>(base)) ||
               (static_cast<DoubleBaseType>(qhat) * v.coefficients_[n - 2] >
                ((static_cast<DoubleBaseType>(rhat) << BASE_SIZE) + u.coefficients_[j + n - 2])))
        {
            --qhat;
            rhat += v.coefficients_[n - 1];
            if (rhat >= base)
                break;
        }

        int borrow = 0;
        for (int i = 0; i < n; ++i)
        {
            DoubleBaseType p = static_cast<DoubleBaseType>(qhat) * v.coefficients_[i];
            int sub = static_cast<int>(u.coefficients_[j + i]) - static_cast<int>(p & (base - 1)) - borrow;
            borrow = (sub < 0) ? 1 : 0;
            u.coefficients_[j + i] = static_cast<BaseType>((sub + (borrow ? base : 0)) & (base - 1));
            borrow += static_cast<int>(p >> BASE_SIZE);
        }
        int sub = static_cast<int>(u.coefficients_[j + n]) - borrow;
        if (sub < 0)
        {
            --qhat;
            int carry = 0;
            for (int i = 0; i < n; ++i)
            {
                int sum = static_cast<int>(u.coefficients_[j + i]) + static_cast<int>(v.coefficients_[i]) + carry;
                if (sum >= static_cast<int>(base))
                {
                    sum -= static_cast<int>(base);
                    carry = 1;
                }
                else
                {
                    carry = 0;
                }
                u.coefficients_[j + i] = static_cast<BaseType>(sum);
            }
            u.coefficients_[j + n] = static_cast<BaseType>(static_cast<int>(u.coefficients_[j + n]) + carry);
        }
        else
        {
            u.coefficients_[j + n] = static_cast<BaseType>(sub);
        }
        Q.coefficients_[j] = qhat;
    }
    Q.NormalizeLength();

    BigNumber R(n);
    R.length_ = n;
    for (int i = 0; i < n; ++i)
        R.coefficients_[i] = u.coefficients_[i];
    R = R / d;
    R.NormalizeLength();

    return {Q, R};
}

BigNumber BigNumber::operator/(const BigNumber &other) const
{
    auto divRes = BigNumber::DivideKnuth(*this, other);
    return divRes.first;
}

BigNumber BigNumber::operator%(const BigNumber &other) const
{
    auto divRes = BigNumber::DivideKnuth(*this, other);
    return divRes.second;
}

// Оператор вывода в поток (вывод в десятичном виде)
std::ostream &operator<<(std::ostream &out, const BigNumber number)
{
    BigNumber zero(1, 0);
    if (number == zero)
    {
        out << "0";
        return out;
    }
    BigNumber temp = number;
    std::string str;
    while (!(temp == zero))
    {
        BigNumber rem = temp % 10;
        char digit = static_cast<char>(rem.coefficients_[0] + '0');
        str.push_back(digit);
        temp = temp / 10;
    }
    std::reverse(str.begin(), str.end());
    out << str;
    return out;
}

// Оператор ввода из потока (ожидается десятичное представление)
std::istream &operator>>(std::istream &in, BigNumber &number)
{
    std::string s;
    in >> s;
    BigNumber result(s.size());
    result = BigNumber(1, 0);
    BigNumber ten(1, 10);
    for (char ch : s)
    {
        if (ch < '0' || ch > '9')
            throw std::invalid_argument("Invalid digit in input.");
        result = result * 10;
        BigNumber digit(1, ch - '0');
        result = result + digit;
    }
    number = result;
    return in;
}

// Вывод числа в 16-ричной форме
void BigNumber::PrintHex() const
{
    for (int i = length_ - 1; i >= 0; --i)
    {
        std::cout.width(BASE_SIZE / 4);
        std::cout.fill('0');
        std::cout << std::hex << coefficients_[i] << " ";
    }
    std::cout << std::dec;
}

// Чтение числа в 16-ричной форме
void BigNumber::ReadHex()
{
    std::string input;
    std::getline(std::cin, input);
    int inputLength = input.size();
    length_ = (inputLength - 1) / (BASE_SIZE / 4) + 1;
    maxLength_ = length_;
    coefficients_.assign(maxLength_, 0);

    int k = 0, j = 0;
    for (int idx = inputLength - 1; idx >= 0; --idx)
    {
        unsigned int temp = 0;
        char ch = input[idx];
        if (ch >= '0' && ch <= '9')
            temp = ch - '0';
        else if (ch >= 'a' && ch <= 'f')
            temp = ch - 'a' + 10;
        else if (ch >= 'A' && ch <= 'F')
            temp = ch - 'A' + 10;
        else
            throw std::invalid_argument("Invalid hexadecimal digit.");
        coefficients_[j] |= temp << k;
        k += 4;
        if (k >= BASE_SIZE)
        {
            k = 0;
            ++j;
        }
    }
    NormalizeLength();
}

} // namespace big_number
