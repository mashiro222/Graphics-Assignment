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
		coff(deg[i]) = cof[i];//意思就是deg处
	}
}

PolynomialMap::PolynomialMap(const vector<int>& deg, const vector<double>& cof) {
	assert(deg.size() == cof.size());
	for (int i = 0; i < deg.size(); i++) {
		coff(deg[i]) = cof[i];//意思应该就是创建一个键是i的，值是cof[i]的map元素
	}
}

double PolynomialMap::coff(int i) const {
	auto target = m_Polynomial.find(i);//find是map的函数
	if (target == m_Polynomial.end()) {//相当于没找到
		return 0;
	}
	return target->second;//返回coff值
}

double& PolynomialMap::coff(int i) {
	return m_Polynomial[i];//意思就是如果没有键i，就创建一个
}

void PolynomialMap::compress() {
	map<int, double> tmpoly = m_Polynomial;
	m_Polynomial.clear();
	auto itr = tmpoly.begin();
	for (; itr != tmpoly.end(); itr++) {
		if (fabs(itr->second) > 1e-10) {
			coff(itr->first) = itr->second;
			//这个地方太有意思了，我们先重置一下map，然后遍历，如果遇到不是0的项，就重新放进map里面
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
		coff(deg) += cof;//就是用这个函数来加入元素到map里面
	}

	inp.close();

	return true;
}
