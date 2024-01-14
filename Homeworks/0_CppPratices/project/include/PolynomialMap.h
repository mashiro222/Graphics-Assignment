#pragma once

#include <map>
#include <string>
#include <vector>

class PolynomialMap
{
public:
    //几种构造函数
    PolynomialMap() { }
    PolynomialMap(const PolynomialMap& other);
    PolynomialMap(const std::string& file); // initialization using file
    PolynomialMap(const double* cof, const int* deg, int n);
    PolynomialMap(const std::vector<int>& deg, const std::vector<double>& cof);
    //扎两个我真的不知道是干嘛的
    double& coff(int i);
    double coff(int i) const;

    void compress();

    // overload
    PolynomialMap operator+(const PolynomialMap& right) const; //Overload operator +
    PolynomialMap operator-(const PolynomialMap& right) const; //Overload operator -
    PolynomialMap operator*(const PolynomialMap& right) const; //Overload operator *
    PolynomialMap& operator=(const PolynomialMap& right); //Overload operator =

    void Print() const;

private:
    bool ReadFromFile(const std::string& file);
    //没有struct了，那个struct的目的就是因为list是存储的向量，而每个项都有次数和系数两个量，所以要用struct把他们括起来，map每个元素刚好是键值两个量
private:
    std::map<int, double> m_Polynomial; // deg -> cof//也就是说deg是键，cof是值
};
