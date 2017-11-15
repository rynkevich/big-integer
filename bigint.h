#ifndef BIGINT_H
#define BIGINT_H

#include <QString>
#include <QRegExp>
#include <cmath>
#include <vector>
#include <stdexcept>

namespace LongArithmetic
{
const int IntDigitsInUnit = 4;
const int IntBase = std::pow(10, IntDigitsInUnit);

class BigInteger
{
private:
    std::vector<int> content;
    bool isNegative = false;
    std::vector<int> getContent() const;
public:
    BigInteger();
    BigInteger(QString number);
    BigInteger(const int number);
    const QString toString() const;
    const BigInteger abs() const;
    int toInt() const; // reminder: integer overflow means undefined behaviour!
    BigInteger &operator =(const BigInteger &operand);
    const BigInteger operator +(const BigInteger &operand) const;
    const BigInteger operator -(const BigInteger &operand) const;
    const BigInteger operator +() const;
    const BigInteger operator -() const;
    const BigInteger operator *(const BigInteger &operand) const;
    const BigInteger operator /(const BigInteger &operand) const;
    const BigInteger &operator ++();
    const BigInteger &operator --();
    const BigInteger operator ++(int);
    const BigInteger operator --(int);
    const BigInteger operator %(const BigInteger &operand) const;
    const BigInteger &operator +=(const BigInteger &operand);
    const BigInteger &operator -=(const BigInteger &operand);
    const BigInteger &operator *=(const BigInteger &operand);
    const BigInteger &operator /=(const BigInteger &operand);
    const BigInteger &operator %=(const BigInteger &operand);
    bool operator >(const BigInteger &operand) const;
    bool operator <(const BigInteger &operand) const;
    bool operator <=(const BigInteger &operand) const;
    bool operator >=(const BigInteger &operand) const;
    bool operator ==(const BigInteger &operand) const;
    bool operator !=(const BigInteger &operand) const;
};
}

#endif // BIGINT_H
