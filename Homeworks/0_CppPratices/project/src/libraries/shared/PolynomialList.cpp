#include "PolynomialList.h"
#include<iostream>
#include<list>
#include<vector>
#include<algorithm>
#include<fstream>
#include<cassert>
#include <cmath>
using namespace std;

PolynomialList::PolynomialList(const PolynomialList& other) {
    m_Polynomial = other.m_Polynomial;
}

PolynomialList::PolynomialList(const string& file) {
    ReadFromFile(file);
}

PolynomialList::PolynomialList(const double* cof, const int* deg, int n) {
    for (int i = 0; i < n; i++) {
        AddOneTerm(Term(deg[i], cof[i]));//���ﲻ��
    }
}

PolynomialList::PolynomialList(const vector<int>& deg, const vector<double>& cof) {
    assert(deg.size() == cof.size());//assert��ĺ��ã��������Ҫ��������
    for (int i = 0; i < deg.size(); i++) {
        AddOneTerm(Term(deg[i], cof[i]));
    }

}

//������������û�㶮Ҫ��ʲô��Ӧ��Ҳ�ǲ�̫��Ҫ��
double PolynomialList::coff(int i) const {
    for (const Term& term : m_Polynomial) {
        if (term.deg < i)
            break;
        if (term.deg == i)
            return term.cof;
    }

    return 0.;
}

double& PolynomialList::coff(int i) {
    return AddOneTerm(Term(i, 0)).cof;
}

//���������������ȥϵ����С����
void PolynomialList::compress() {
    auto itr = m_Polynomial.begin();
    //��ʼ����
    while (itr != m_Polynomial.end()) {
        if (fabs(itr->cof) < 1e-10) {
            itr = m_Polynomial.erase(itr);//��Щ���õĺ������᷵��һЩ���ʵĵ�����
        }
        else itr++;
    }
}

PolynomialList PolynomialList::operator+(const PolynomialList& right) const {
    PolynomialList poly(*this);//poly��ʾ+����ߵĶ���ʽ
    //�����õ���AddOneTerm���������ʣ��䷵�ص��ǲ�����ߺϲ���λ��
    list<Term> List;
    List = right.m_Polynomial;
    for (auto itr = List.begin(); itr != List.end(); itr++) {
        poly.AddOneTerm(Term(itr->deg, itr->cof));
    }

    poly.compress();//��ȥһ�º�С����

    return poly; // you should return a correct value
}

PolynomialList PolynomialList::operator-(const PolynomialList& right) const {
    PolynomialList poly(*this);//poly��ʾ+����ߵĶ���ʽ
    //�����õ���AddOneTerm���������ʣ��䷵�ص��ǲ�����ߺϲ���λ��
    list<Term> List;
    List = right.m_Polynomial;
    for (auto itr = List.begin(); itr != List.end(); itr++) {
        poly.AddOneTerm(Term(itr->deg, -itr->cof));
    }

    poly.compress();//��ȥһ�º�С����

    return poly; // you should return a correct value
}

PolynomialList PolynomialList::operator*(const PolynomialList& right) const {
    PolynomialList rst;
    list<Term> List1, List2;
    List1 = m_Polynomial; List2 = right.m_Polynomial;
    auto itr1 = List1.begin(); auto itr2 = List2.begin();
    for (; itr1 != List1.end(); itr1++) {
        for (itr2=List2.begin(); itr2 != List2.end(); itr2++) {
            double cof = itr1->cof * itr2->cof;
            int deg = itr1->deg + itr2->deg;
            rst.AddOneTerm(Term(deg, cof));
        }
    }
    rst.compress();
    return rst;
}

PolynomialList& PolynomialList::operator=(const PolynomialList& right) {
    m_Polynomial = right.m_Polynomial;
    return *this;
}

void PolynomialList::Print() const {
    PolynomialList rst;
    rst.compress();

    auto itr = m_Polynomial.begin();
    if (itr == m_Polynomial.end()) {
        cout << "0" << endl;
        return;
    }
    for (; itr != m_Polynomial.end();itr++) {
        if (itr != m_Polynomial.begin()) {
            cout << " ";
            if (itr->cof > 0) cout << "+";
        }
        cout << itr->cof;
        if (itr->deg > 0) {
            cout << "x^" << itr->deg;
        }
    }
    cout << endl;
}

bool PolynomialList::ReadFromFile(const string& file) {
    m_Polynomial.clear();//�Ȱ�list���
    //���Ǵ��ļ������ķ������㲻����ֱ��copy��
    ifstream inp;
    inp.open(file.c_str());
    if (!inp.is_open()) {//���û�д�
        cout << "ERROR::PolynomialList::ReadFromFile:" << endl
            << "\t" << "file [" << file << "] opens failed" << endl;
        return false;
    }
    //��������ʽ�Ķ�ȡ
    char ch; int n;
    inp >> ch;//ch�Ƕ�ȡ��Pû����
    inp >> n;//n�Ƕ�����
    for (int i = 0; i < n; i++) {
        Term nd;
        inp >> nd.deg;
        inp >> nd.cof;
        AddOneTerm(nd);
    }
    inp.close();//�ص�

    return true; // you should return a correct value
}

//m_Plynomial��term��list������������Ҫ��������Ҫ��term�Ž����ʵĵط�
//�����ʱ�������С�������ԱȽϵ�ʱ��ȽϷ���
//�������ûɶ����yysy
PolynomialList::Term& PolynomialList::AddOneTerm(const Term& term) {
    auto itr = m_Polynomial.begin();//auto��ĺ��ã�����ǿ�һ��������
    for (; itr != m_Polynomial.end(); itr++) {
        if (itr->deg == term.deg) {
            itr->cof += term.cof;
            return *itr;
        }
        if (itr->deg > term.deg) {
            break;
        }//�ҵ�����λ����
    }
    //���ص��ǵ�ǰ�ĵ�������insert�ǲ�����itrǰ��Ŀո�
    return *m_Polynomial.insert(itr,term); // you should return a correct value
}
