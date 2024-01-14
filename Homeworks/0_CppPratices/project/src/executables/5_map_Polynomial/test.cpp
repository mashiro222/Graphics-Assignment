//list��map�ܽ���˵��map���������list������˲�ֹһ��������
//������100%����ѹ�������㷨�ͺܼ�
//���ܱ������Ϲ���ʽ����������ʽ�����ײ��߼��͸��ʺϽ��ж���ʽ������

#include <PolynomialList.h>
#include <PolynomialMap.h>
#include "TestPolynomial.h"

#include <string>
#include <cstdlib>
#include <ctime>

using namespace std;

void generateTestCase(int size, vector<int>& deg, vector<double>& cof) {
    deg.clear(); cof.clear();
    for (int i = 0; i < size; i++) {
        int c = rand() % 10000;
        double val = (double)(rand() % 100);
        deg.push_back(c);
        cof.push_back(val);
    }
}

int main(int argc, char** argv) {
    srand(unsigned(time(0)));
    TestPolynomial<PolynomialList> testList;
    TestPolynomial<PolynomialMap> testMap;

    vector<int> deg0, deg1;
    vector<double> cof0, cof1;

    generateTestCase(5, deg0, cof0);
    generateTestCase(5, deg1, cof1);
    cout << "Test List:" << endl;
    testList.testOperationFromGivenData(deg0, cof0, deg0, cof1, true);
    cout << "Test Map: " << endl;
    testMap.testOperationFromGivenData(deg0, cof0, deg0, cof1, true);

    generateTestCase(100, deg0, cof0);
    generateTestCase(100, deg1, cof1);
    cout << "Test List:" << endl;
    testList.testOperationFromGivenData(deg0, cof0, deg1, cof1);
    cout << "Test Map: " << endl;
    testMap.testOperationFromGivenData(deg0, cof0, deg1, cof1);

    generateTestCase(150, deg0, cof0);
    generateTestCase(150, deg1, cof1);
    cout << "Test List:" << endl;
    testList.testOperationFromGivenData(deg0, cof0, deg1, cof1);
    cout << "Test Map: " << endl;
    testMap.testOperationFromGivenData(deg0, cof0, deg1, cof1);

    return 0;
}
/*
int main(int argc, char** argv) {
    PolynomialMap p1("../data/P3.txt");
    PolynomialMap p2("../data/P4.txt");
    PolynomialMap p4("../data/P1.txt");
    p4.Print();
    PolynomialMap p3;
    p1.Print();
    p2.Print();

    p3 = p1 + p2;
    p1.Print();
    p3.Print();
    p3 = p1 - p2;
    p3.Print();
    p3 = p1 * p2;
    p3.Print();

    return 0;
}*/

