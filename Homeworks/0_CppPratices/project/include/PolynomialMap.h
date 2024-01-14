#pragma once

#include <map>
#include <string>
#include <vector>

class PolynomialMap
{
public:
    //���ֹ��캯��
    PolynomialMap() { }
    PolynomialMap(const PolynomialMap& other);
    PolynomialMap(const std::string& file); // initialization using file
    PolynomialMap(const double* cof, const int* deg, int n);
    PolynomialMap(const std::vector<int>& deg, const std::vector<double>& cof);
    //����������Ĳ�֪���Ǹ����
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
    //û��struct�ˣ��Ǹ�struct��Ŀ�ľ�����Ϊlist�Ǵ洢����������ÿ����д�����ϵ��������������Ҫ��struct��������������mapÿ��Ԫ�ظպ��Ǽ�ֵ������
private:
    std::map<int, double> m_Polynomial; // deg -> cof//Ҳ����˵deg�Ǽ���cof��ֵ
};
