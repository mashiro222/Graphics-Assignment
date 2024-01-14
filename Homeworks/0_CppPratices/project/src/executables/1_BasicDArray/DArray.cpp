// implementation of class DArray
#include "DArray.h"
#include<iostream>
#include<cassert>
using namespace std;
// default constructor
DArray::DArray() {
	Init();
}

// set an array with default values
DArray::DArray(int nSize, double dValue)
	: m_pData(new double[nSize]), m_nSize(nSize)
{//samples���õĳ�ʼ���б�
//	m_pData = new double[nSize];
//	m_nSize = nSize;
	for (int i = 0; i < nSize; i++) {
		m_pData[i] = dValue;
	}
}

DArray::DArray(const DArray& arr) 
	: m_pData(new double[arr.m_nSize]), m_nSize(arr.m_nSize)
{
//	m_pData = new double[arr.m_nSize];
//	m_nSize = arr.m_nSize;
	for (int i = 0; i < m_nSize; i++) {
		m_pData[i] = arr.m_pData[i];
	}
}

// deconstructor
DArray::~DArray() {
	Free();
}

// display the elements of the array
void DArray::Print() const {
	cout << "size = " << m_nSize << ":";
	for (int i = 0; i < m_nSize; i++)
		cout << " " << GetAt(i);

	cout << endl;
}

// initilize the array
void DArray::Init() {
	m_nSize = 0;
	m_pData = nullptr; //NULLֻ��һ���궨�壬��nullptr��һ��C++�ؼ��֣�����֮����nullptr
}

// free the array
void DArray::Free() {
	//delete[] m_pData;
	delete[] m_pData;//���������Ϊ�˰�֮ǰ���������������initilize
	m_pData = nullptr;
	m_nSize = 0;
}

// get the size of the array
int DArray::GetSize() const {
	
	return m_nSize; // you should return a correct value
}

// set the size of the array
void DArray::SetSize(int nSize) {
	//m_nSize = nSize;
	//����Ҳ�ǲ���ֱ�Ӹ�ֵ�ģ�����������һ������ģ�����ĳ����Ѿ��������ˣ�Ҫ�ĳ��ȵ�������һ��
	if (m_nSize == nSize) {
		return;
	}
	double* pData = new double[nSize];
	//���ﻹ������nSize��m_nSize�Ĵ�С��ϵ���д���
	int copynum = nSize < m_nSize ? nSize : m_nSize;
	for (int i = 0; i < copynum; i++) {
		pData[i] = m_pData[i];
	}
	for (int i = copynum; i < nSize; i++) {
		pData[i] = 0.;
	}
	//���µ�ָ�������֮���ٷŻ�ȥ
	delete[] m_pData;
	m_pData = pData;
	m_nSize = nSize;
}

// get an element at an index
const double& DArray::GetAt(int nIndex) const {
	assert(nIndex >= 0 && nIndex < m_nSize);//assert��һ���if�жϲ����������ı�������ǿ��
	return m_pData[nIndex]; // you should return a correct value
}

// set the value of an element 
void DArray::SetAt(int nIndex, double dValue) {
	assert(nIndex>=0 && nIndex<m_nSize);
	m_pData[nIndex] = dValue;
}

// overload operator '[]'
double& DArray::operator[](int nIndex){
	assert(nIndex >= 0 && nIndex < m_nSize);
	return m_pData[nIndex];
} 

// overload operator '[]'
const double& DArray::operator[](int nIndex) const {
	assert(nIndex >= 0 && nIndex < m_nSize);
	return m_pData[nIndex]; // you should return a correct value
}



// add a new element at the end of the array
void DArray::PushBack(double dValue) {
//	m_nSize++;
//	m_pData[m_nSize - 1] = dValue;//ֻ����д��������ģ���Ϊ�����Ѿ�����һ���̶����ȵĿռ䣬����˵size����data���ܱ�
	double* pTemp = new double[m_nSize + 1];
	for (int i = 0; i < m_nSize; i++) {
		pTemp[i] = m_pData[i];
	}
	pTemp[m_nSize] = dValue;
	delete[] m_pData;
	m_pData = pTemp;
	m_nSize++;
}

// delete an element at some index
void DArray::DeleteAt(int nIndex) {
	assert(nIndex >= 0 && nIndex < m_nSize);
	double* pTemp = new double[static_cast<size_t>(m_nSize) - 1];
	for (int i = 0; i < nIndex; i++)
		pTemp[i] = m_pData[i];
	for (int i = nIndex; i < m_nSize - 1; i++)
		pTemp[i] = m_pData[i + 1];
	delete[] m_pData;
	m_pData = pTemp;
	m_nSize--;
}

// insert a new element at some index
void DArray::InsertAt(int nIndex, double dValue) {
	assert(nIndex >= 0 && nIndex <= m_nSize); // nIndex == m_nSize is legal
	double* pTemp = new double[static_cast<size_t>(m_nSize) + 1];
	for (int i = 0; i < nIndex; i++)
		pTemp[i] = m_pData[i];
	pTemp[nIndex] = dValue;
	for (int i = nIndex + 1; i < m_nSize + 1; i++)
		pTemp[i] = m_pData[i - 1];
	delete[] m_pData;
	m_pData = pTemp;
	m_nSize++;
}

// overload operator '='
DArray& DArray::operator = (const DArray& arr) {//���������ǲ������Ӳ���const������ν
	delete[] m_pData;

	m_nSize = arr.m_nSize;
	m_pData = new double[m_nSize];//û��double*����Ϊm_pData��class�����Ѿ������Ͷ�����

	for (int i = 0; i < m_nSize; i++)
		m_pData[i] = arr[i];

	return *this;//���ǰ�Ŀǰ����Ϣreturn
}
