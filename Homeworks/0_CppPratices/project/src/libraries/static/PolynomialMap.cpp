#include "PolynomialMap.h"

#include <iostream>
#include <fstream>
#include <cassert>
#include <cmath>

using namespace std;

PolynomialMap::PolynomialMap(const PolynomialMap& other) {
	m_Polynomial = other.m_Polynomial;
}

PolynomialMap::PolynomialMap(const string& file) {
    ReadFromFile(file);
}

PolynomialMap::PolynomialMap(const double* cof, const int* deg, int n) {
	for (int i = 0; i < n; i++) {
		coff(deg[i]) = cof[i];//��˼����deg��
	}
}

PolynomialMap::PolynomialMap(const vector<int>& deg, const vector<double>& cof) {
	assert(deg.size() == cof.size());
	for (int i = 0; i < deg.size(); i++) {
		coff(deg[i]) = cof[i];//��˼Ӧ�þ��Ǵ���һ������i�ģ�ֵ��cof[i]��mapԪ��
	}
}

double PolynomialMap::coff(int i) const {
	auto target = m_Polynomial.find(i);//find��map�ĺ���
	if (target == m_Polynomial.end()) {//�൱��û�ҵ�
		return 0;
	}
	return target->second;//����coffֵ
}

double& PolynomialMap::coff(int i) {
	return m_Polynomial[i];//��˼�������û�м�i���ʹ���һ��
}

void PolynomialMap::compress() {
	map<int, double> tmpoly = m_Polynomial;
	m_Polynomial.clear();
	auto itr = tmpoly.begin();
	for (; itr != tmpoly.end(); itr++) {
		if (fabs(itr->second) > 1e-10) {
			coff(itr->first) = itr->second;
			//����ط�̫����˼�ˣ�����������һ��map��Ȼ������������������0��������·Ž�map����
		}
	}
}

PolynomialMap PolynomialMap::operator+(const PolynomialMap& right) const {
	PolynomialMap poly(right);
	auto itr = m_Polynomial.begin();
	for (; itr != m_Polynomial.end(); itr++) {
		poly.coff(itr->first) += itr->second;
	}
	return poly; // you should return a correct value
}

PolynomialMap PolynomialMap::operator-(const PolynomialMap& right) const {
/*	PolynomialMap poly;
	auto itr = m_Polynomial.begin();
	for (; itr != m_Polynomial.end(); itr++) {
		poly.coff(itr->first) -= itr->second;
	}
	poly.compress();
	return poly; // you should return a correct value*/
	PolynomialMap poly(*this);
	for (const auto& term : right.m_Polynomial)
		poly.coff(term.first) -= term.second;

	poly.compress();
	return poly;
}

PolynomialMap PolynomialMap::operator*(const PolynomialMap& right) const {
	PolynomialMap poly;
	map<int, double> map1, map2;
	map1 = m_Polynomial; map2 = right.m_Polynomial;
	auto itr1 = map1.begin(); auto itr2 = map2.begin();
	//if (itr1 == map1.end()) cout << "end" << endl;
	for (; itr1 != map1.end(); itr1++) {
		for (itr2 = map2.begin(); itr2 != map2.end(); itr2++) {
			//cout << itr1->first << itr1->second << endl;
			int deg = itr1->first + itr2->first;
			double cof = itr1->second * itr2->second;
			poly.coff(deg) += cof;
		}
	}
	return poly; // you should return a correct value
	/*PolynomialMap poly;
	for (const auto& term1 : m_Polynomial) {
		for (const auto& term2 : right.m_Polynomial) {
			int deg = term1.first + term2.first;
			double cof = term1.second * term2.second;
			poly.coff(deg) += cof;
		}
	}
	return poly;*/
}

PolynomialMap& PolynomialMap::operator=(const PolynomialMap& right) {
	m_Polynomial = right.m_Polynomial;
	return *this;
}

void PolynomialMap::Print() const {
	auto itr = m_Polynomial.begin();
	if (itr == m_Polynomial.end()) {
		cout << "0" << endl;
		return;
	}
	for (; itr != m_Polynomial.end(); itr++) {
		if (itr != m_Polynomial.begin()) {
			cout << " ";
			if (itr->second > 0) cout << "+";
		}
		cout << itr->second;
		if (itr->first > 0) cout << "x^" << itr->first;
	}
	cout << endl;
}

bool PolynomialMap::ReadFromFile(const string& file) {
	m_Polynomial.clear();

	ifstream inp;
	inp.open(file.c_str());
	if (!inp.is_open()) {
		cout << "ERROR::PolynomialList::ReadFromFile:" << endl
			<< "\t" << "file [" << file << "] opens failed" << endl;
		return false;
	}

	char ch;
	int n;
	inp >> ch;
	inp >> n;
	for (int i = 0; i < n; i++) {
		int deg;
		double cof;
		inp >> deg;
		inp >> cof;
		coff(deg) += cof;//�������������������Ԫ�ص�map����
	}

	inp.close();

	return true;
}
