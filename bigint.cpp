#include "bigint.h"

namespace LongArithmetic
{
BigInteger::BigInteger()
{
    std::vector<int> zeroContent {0};
    content = zeroContent;
}

BigInteger::BigInteger(QString number)
{
    QRegExp validNumber("[+-]?[0-9]+");
    if (!validNumber.exactMatch(number)) {
        throw std::invalid_argument("invalid bigint value");
    }

    if (number.at(0) == '-') {
        number.remove(0, 1);
        isNegative = true;
    } else if (number.at(0) == '+') {
        number.remove(0, 1);
    }

    while (number.at(0) == '0' && number.size() != 1) {
        number.remove(0, 1);
    }
    if (number.at(0) == '0') {
        isNegative = false;
    }

    for (int i = number.size(); i > 0; i -= IntDigitsInUnit) {
        content.push_back((i < IntDigitsInUnit ?
                number.left(i) : number.mid(i - IntDigitsInUnit, IntDigitsInUnit)).toInt());
    }
 }

BigInteger::BigInteger(const int number) : BigInteger(QString::number(number))
{}

const QString BigInteger::toString() const
{
    QString result;

    if (isNegative) {
        result.append("-");
    }
    result.append(content.empty() ? "0" : QString::number(content.back()));

    for (int i = static_cast<int>(content.size()) - 2; i >= 0; i--) {
        result.append(QString::number(content[i]).rightJustified(IntDigitsInUnit, '0'));
    }

    return result;
}

const BigInteger BigInteger::abs() const
{
    BigInteger result(*this);
    result.isNegative = false;
    return result;
}

int BigInteger::toInt() const
{
    int result = 0;

    for (int i = static_cast<int>(content.size()) - 1; i >= 0; i--) {
        BigInteger temp = result;
        result *= IntBase;
        result += content[i];
    }

    if (QString::number(result) != this->abs().toString()) {
        throw std::overflow_error("integer overflow");
    }

    return isNegative ? -result : result;
}

BigInteger &BigInteger::operator =(const BigInteger &operand)
{
    content = operand.content;
    isNegative = operand.isNegative;
    return *this;
}

const BigInteger BigInteger::operator +(const BigInteger &operand) const
{
    if (this->isNegative && !operand.isNegative) {
        return operand - -*this;
    }
    if (!this->isNegative && operand.isNegative) {
        return *this - -operand;
    }

    BigInteger result(0);

    int carry = 0;
    result.content = this->content;
    for (size_t i = 0; i < std::max(this->content.size(), operand.content.size()) || carry; i++) {
        if (i == result.content.size()) {
            result.content.push_back(0);
        }
        result.content[i] += carry + (i < operand.content.size() ? operand.content[i] : 0);
        carry = result.content[i] >= IntBase;
        if (carry) {
            result.content[i] -= IntBase;
        }
    }

    result.isNegative = this->isNegative && operand.isNegative;

    return result;
}

const BigInteger BigInteger::operator -(const BigInteger &operand) const
{
    if (this->isNegative && operand.isNegative) {
        return -operand - (-*this);
    }
    if (this->isNegative && !operand.isNegative) {
        return -(-*this + operand);
    }
    if (!this->isNegative && operand.isNegative) {
        return *this + (-operand);
    }

    BigInteger result(0);
    const std::vector<int> *minuend = *this > operand ? &this->content : &operand.content;
    const std::vector<int> *subtrahend = minuend == &this->content ? &operand.content : &this->content;

    int carry = 0;
    result.content = *minuend;
    for (size_t i = 0; i < subtrahend->size() || carry; i++) {
        result.content[i] -= carry + (i < subtrahend->size() ? subtrahend->at(i) : 0);
        carry = result.content[i] < 0;
        if (carry) {
            result.content[i] += IntBase;
        }
    }
    while (result.content.size() > 1 && result.content.back() == 0) {
        result.content.pop_back();
    }

    result.isNegative = *this < operand;

    return result;
}

const BigInteger BigInteger::operator -() const
{
    BigInteger result = *this;
    result.isNegative = !result.isNegative;
    return result;
}

const BigInteger BigInteger::operator +() const
{
    return *this;
}

const BigInteger BigInteger::operator *(const BigInteger &operand) const
{
    BigInteger result(0);
    result.content.resize(this->content.size() + operand.content.size());

    for (size_t i = 0; i < this->content.size(); i++) {
        int carry = 0;
        for (size_t j = 0; j < operand.content.size() || carry; j++) {
            quint64 iterElem = result.content[i + j] +
                    this->content[i] * (j < operand.content.size() ? operand.content[j] : 0) + carry;
            result.content[i + j] = iterElem % IntBase;
            carry = iterElem / IntBase;
        }
    }

    while (result.content.size() > 1 && result.content.back() == 0) {
        result.content.pop_back();
    }
    result.isNegative = (this->isNegative ? 1 : 0) ^ (operand.isNegative ? 1 : 0);

    return result;
}

const BigInteger BigInteger::operator /(const BigInteger &operand) const
{
    if (operand.abs() > this->abs()) {
        return BigInteger(0);
    }

    BigInteger result(0);
    result.content.resize(this->content.size());

    BigInteger currentValue(0);
    currentValue.content.resize(this->content.size());
    for (int i = static_cast<int>(this->content.size()) - 1; i >= 0; i--) {
        currentValue *= IntBase;
        currentValue.content[0] = this->content[i];

        int maxDivisor = 0;
        int bsLeft = 0;
        int bsRight = IntBase;

        while (bsLeft <= bsRight) {
            int bsMid = (bsLeft + bsRight) / 2;
            BigInteger bsIterValue = operand.abs() * BigInteger(bsMid);
            if (bsIterValue <= currentValue) {
                maxDivisor = bsMid;
                bsLeft = bsMid + 1;
            } else {
                bsRight = bsMid - 1;
            }
        }
        result.content[i] = maxDivisor;
        currentValue -= operand.abs() * BigInteger(maxDivisor);
    }

    while (result.content.size() > 1 && result.content.back() == 0) {
        result.content.pop_back();
    }
    result.isNegative = (this->isNegative ? 1 : 0) ^ (operand.isNegative ? 1 : 0);

    return result;
}

const BigInteger &BigInteger::operator ++()
{
    *this = *this + 1;
    return *this;
}

const BigInteger &BigInteger::operator --()
{
    *this = *this - 1;
    return *this;
}

const BigInteger BigInteger::operator %(const BigInteger &operand) const
{
    if (operand.abs() > this->abs()) {
        return *this;
    }

    BigInteger result(0);
    result.content.resize(this->content.size());

    BigInteger currentValue(0);
    currentValue.content.resize(this->content.size());
    for (int i = static_cast<int>(this->content.size()) - 1; i >= 0; i--) {
        currentValue *= IntBase;
        currentValue.content[0] = this->content[i];

        int maxDivisor = 0;
        int bsLeft = 0;
        int bsRight = IntBase;

        while (bsLeft <= bsRight) {
            int bsMid = (bsLeft + bsRight) / 2;
            BigInteger bsIterValue = operand.abs() * BigInteger(bsMid);
            if (bsIterValue <= currentValue) {
                maxDivisor = bsMid;
                bsLeft = bsMid + 1;
            } else {
                bsRight = bsMid - 1;
            }
        }
        result.content[i] = maxDivisor;
        currentValue -= operand.abs() * BigInteger(maxDivisor);
    }

    return currentValue;
}

const BigInteger BigInteger::operator ++(int)
{
    BigInteger result(*this);
    ++(*this);
    return result;
}

const BigInteger BigInteger::operator --(int)
{
    BigInteger result(*this);
    --(*this);
    return result;
}

const BigInteger &BigInteger::operator +=(const BigInteger &operand)
{
    return *this = *this + operand;
}

const BigInteger &BigInteger::operator -=(const BigInteger &operand)
{
    return *this = *this - operand;
}

const BigInteger &BigInteger::operator *=(const BigInteger &operand)
{
    return *this = *this * operand;
}

const BigInteger &BigInteger::operator /=(const BigInteger &operand)
{
    return *this = *this / operand;
}

const BigInteger &BigInteger::operator %=(const BigInteger &operand)
{
    return *this = *this % operand;
}

bool BigInteger::operator >(const BigInteger &operand) const
{
    if (this->isNegative != operand.isNegative) {
        return !this->isNegative && operand.isNegative;
    }
    if (this->content.size() == operand.content.size()) {
        for (int i = static_cast<int>(this->content.size()) - 1; i >= 0; i--) {
            if (this->content[i] > operand.content[i]) {
                return !this->isNegative;
            }
            if (this->content[i] < operand.content[i]) {
                return this->isNegative;
            }
        }
        return this->isNegative;
    } else {
        return this->content.size() < operand.content.size() ? this->isNegative : !this->isNegative;
    }
}

bool BigInteger::operator <(const BigInteger &operand) const
{
    return !(*this >= operand);
}

bool BigInteger::operator >=(const BigInteger &operand) const
{
    return *this == operand || *this > operand;
}

bool BigInteger::operator <=(const BigInteger &operand) const
{
    return !(*this > operand);
}

bool BigInteger::operator ==(const BigInteger &operand) const
{
    return this->isNegative == operand.isNegative && this->toString() == operand.toString();
}

bool BigInteger::operator !=(const BigInteger &operand) const
{
    return !(*this == operand);
}

std::vector<int> BigInteger::getContent() const
{
    return this->content;
}
}
