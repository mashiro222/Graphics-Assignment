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
        AddOneTerm(Term(deg[i], cof[i]));//这里不懂
    }
}

PolynomialList::PolynomialList(const vector<int>& deg, const vector<double>& cof) {
    assert(deg.size() == cof.size());//assert真的好用，这里就是要求必须相等
    for (int i = 0; i < deg.size(); i++) {
        AddOneTerm(Term(deg[i], cof[i]));
    }

}

//这两个函数我没搞懂要干什么，应该也是不太重要的
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

//这个函数是用来消去系数很小的项
void PolynomialList::compress() {
    auto itr = m_Polynomial.begin();
    //开始迭代
    while (itr != m_Polynomial.end()) {
        if (fabs(itr->cof) < 1e-10) {
            itr = m_Polynomial.erase(itr);//这些内置的函数都会返回一些合适的迭代器
        }
        else itr++;
    }
}

PolynomialList PolynomialList::operator+(const PolynomialList& right) const {
    PolynomialList poly(*this);//poly表示+号左边的多项式
    //这里用到了AddOneTerm函数的性质，其返回的是插入或者合并的位置
    list<Term> List;
    List = right.m_Polynomial;
    for (auto itr = List.begin(); itr != List.end(); itr++) {
        poly.AddOneTerm(Term(itr->deg, itr->cof));
    }

    poly.compress();//消去一下很小的项

    return poly; // you should return a correct value
}

PolynomialList PolynomialList::operator-(const PolynomialList& right) const {
    PolynomialList poly(*this);//poly表示+号左边的多项式
    //这里用到了AddOneTerm函数的性质，其返回的是插入或者合并的位置
    list<Term> List;
    List = right.m_Polynomial;
    for (auto itr = List.begin(); itr != List.end(); itr++) {
        poly.AddOneTerm(Term(itr->deg, -itr->cof));
    }

    poly.compress();//消去一下很小的项

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
    m_Polynomial.clear();//先把list清空
    //这是打开文件导出的方法，搞不懂，直接copy了
    ifstream inp;
    inp.open(file.c_str());
    if (!inp.is_open()) {//如果没有打开
        cout << "ERROR::PolynomialList::ReadFromFile:" << endl
            << "\t" << "file [" << file << "] opens failed" << endl;
        return false;
    }
    //现在来正式的读取
    char ch; int n;
    inp >> ch;//ch是读取的P没有用
    inp >> n;//n是多少项
    for (int i = 0; i < n; i++) {
        Term nd;
        inp >> nd.deg;
        inp >> nd.cof;
        AddOneTerm(nd);
    }
    inp.close();//关掉

    return true; // you should return a correct value
}

//m_Plynomial是term的list，所以现在是要把我们想要的term放进合适的地方
//输出的时候次数从小到大，所以比较的时候比较方便
//这个返回没啥作用yysy
PolynomialList::Term& PolynomialList::AddOneTerm(const Term& term) {
    auto itr = m_Polynomial.begin();//auto真的好用，这个是开一个迭代器
    for (; itr != m_Polynomial.end(); itr++) {
        if (itr->deg == term.deg) {
            itr->cof += term.cof;
            return *itr;
        }
        if (itr->deg > term.deg) {
            break;
        }//找到插入位置了
    }
    //返回的是当前的迭代器，insert是插入在itr前面的空格处
    return *m_Polynomial.insert(itr,term); // you should return a correct value
}
