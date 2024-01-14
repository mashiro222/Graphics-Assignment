// implementation of class DArray
//С��ҵ3���ڲ�֪����ô��vs��cmake�࿪һ����Ŀ���Ծ�ֱ��д�ļ��ˣ�����practice��src���棩����֪��զ��ʵ����
#include "DArray.h"
#include<iostream>
#include<cassert>

using namespace std;

// default constructor
DArray::DArray() {
	Init();
}

// set an array with default values
DArray::DArray(int nSize, double dValue)//samples��������д�ܺÿ�
	:m_pData(new double[nSize]),m_nSize(nSize),m_nMax(nSize)
{
	for (int i = 0; i < nSize; i++) {
		m_pData[i] = dValue;
	}
}

DArray::DArray(const DArray& arr) 
	:m_pData(new double[arr.m_nSize]),m_nSize(arr.m_nSize),m_nMax(arr.m_nSize)
{
	for (int i = 0; i < m_nSize; i++) {
		m_pData[i] = arr.m_pData[i];
	}
}

// deconstructor
DArray::~DArray() {
	Free();
}

// display the elements of the array
void DArray::Print() const {//�ⲽֱ��copy�ˣ�Ϊ��֮�����bugͳһģ��
	cout << "size= " << m_nSize << ":";
	for (int i = 0; i < m_nSize; i++) {
		cout << " " << m_pData[i];
	}
	cout << endl;
}

// initilize the array
void DArray::Init() {
	m_pData = nullptr;
	m_nMax = 0;
	m_nSize = 0;
}

// free the array
void DArray::Free() {
	delete[] m_pData;
	m_pData = nullptr;
	m_nSize = 0;
	m_nMax = 0;//��Ҫ����m_nMax��
}

// get the size of the array
int DArray::GetSize() const {
	return m_nSize; // you should return a correct value
}

// allocate enough memory
void DArray::Reserve(int nSize) {//�����practice����û�У����Լ��ӽ�����
	//�����������˼���ǰ�m_nMax���úã�����allocate enough memoryҲ��������Ҫ�����ֱ�ӿ���sample��
	if ( m_nMax>= nSize) return;
	while (m_nMax < nSize) {
		//��һ�����Ҿ���д������Ī�������һ���֣�������˵��Ҫ��m_nMax��nSize��
		//ֱ����m_nMax=nSize+1�����ˣ���̫���ΪɶҪ��ôд
		m_nMax = (m_nMax == 0) ? 1 : 2 * m_nMax;
	}
	double* pData = new double[m_nMax];//����m_pData������������Ҫ�Ĵ�С�ˣ����ھ͸�ֵ��ȥ
	memcpy(pData, m_pData, m_nSize * sizeof(double));//�������ˣ����ǲ���vector����ֱ�Ӹ�ֵ����
	//���ﲻ����forѭ����ֵ������������ԭ�������ǿ�����m_nMax���ȵ�array����һ����m_nSize������ֻ�Ǳ�nSize�󣡣���������͵�һ��С��ϰ������ǲ�һ����
	delete[] m_pData;
	m_pData = pData;
}

// set the size of the array
//���������ʼ�����ֳ�����m_nMaxǰ��������ˣ�ϸϸ���
void DArray::SetSize(int nSize) {
	//���ǿ�һ��nSize���ȵ�array
	if (m_nSize == nSize) return;
	Reserve(nSize);//m_nMax���úã�����������ô��ֵ�����ᳬ��
	for (int i = m_nSize; i < nSize; i++) {
		m_pData[i] = 0;
	}
	m_nSize = nSize;
}

// get an element at an index
const double& DArray::GetAt(int nIndex) const {
	assert(nIndex >= 0 && nIndex < m_nSize);
	
	return m_pData[nIndex]; // you should return a correct value
}

// set the value of an element 
void DArray::SetAt(int nIndex, double dValue) {
	assert(nIndex >= 0 && nIndex < m_nSize);
	m_pData[nIndex] = dValue;
}

// overload operator '[]'
double& DArray::operator[](int nIndex) {
	assert(nIndex >= 0 && nIndex < m_nSize);
	return m_pData[nIndex]; // you should return a correct value
}

// overload operator '[]'
const double& DArray::operator[](int nIndex) const {
	assert(nIndex >= 0 && nIndex < m_nSize);
	return m_pData[nIndex]; // you should return a correct value
}

// add a new element at the end of the array
void DArray::PushBack(double dValue) {
	Reserve(m_nSize + 1);
	//�������������ʲô��˼�ˣ����Reserve�еķ�����������
	//��ʱ��Ϳռ�����֮��Ѱ��һ��ƽ�⣬��ľ���
	m_pData[m_nSize] = dValue;
	m_nSize += 1;
}

// delete an element at some index
void DArray::DeleteAt(int nIndex) {//���nIndex�Ǵ�0��ʼ��ģ�����˵�ڼ���
	assert(nIndex >= 0 && nIndex < m_nSize);
	for (int i = nIndex + 1; i < m_nSize; i++) {
		m_pData[i - 1] = m_pData[i];
	}
	m_nSize--;
}

// insert a new element at some index
void DArray::InsertAt(int nIndex, double dValue) {
	assert(nIndex >= 0 && nIndex <= m_nSize);
	Reserve(m_nSize + 1);
	for (int i = m_nSize-1; i >= nIndex; i--) {
		m_pData[i + 1] = m_pData[i];
	}
	m_pData[nIndex] = dValue;
	m_nSize++;
}

// overload operator '='
DArray& DArray::operator = (const DArray& arr) {
	Reserve(arr.m_nSize);
	m_nSize = arr.m_nSize;
	memcpy(m_pData, arr.m_pData, arr.m_nSize * sizeof(double));
	return *this;
}
