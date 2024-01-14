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
{//samples是用的初始化列表
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
	m_pData = nullptr; //NULL只是一个宏定义，而nullptr是一个C++关键字，咱们之后都用nullptr
}

// free the array
void DArray::Free() {
	//delete[] m_pData;
	delete[] m_pData;//这个步骤是为了把之前的数列清除，重新initilize
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
	//这里也是不能直接赋值的，类里面是有一个数组的，数组的长度已经定下来了，要改长度得重新来一遍
	if (m_nSize == nSize) {
		return;
	}
	double* pData = new double[nSize];
	//这里还讨论了nSize和m_nSize的大小关系进行处理
	int copynum = nSize < m_nSize ? nSize : m_nSize;
	for (int i = 0; i < copynum; i++) {
		pData[i] = m_pData[i];
	}
	for (int i = copynum; i < nSize; i++) {
		pData[i] = 0.;
	}
	//把新的指针造好了之后再放回去
	delete[] m_pData;
	m_pData = pData;
	m_nSize = nSize;
}

// get an element at an index
const double& DArray::GetAt(int nIndex) const {
	assert(nIndex >= 0 && nIndex < m_nSize);//assert比一般的if判断不满足条件的变量更加强大
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
//	m_pData[m_nSize - 1] = dValue;//只这样写是有问题的，因为我们已经开了一个固定长度的空间，不能说size变了data就能变
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
DArray& DArray::operator = (const DArray& arr) {//括号里面是参数，加不加const都无所谓
	delete[] m_pData;

	m_nSize = arr.m_nSize;
	m_pData = new double[m_nSize];//没加double*是因为m_pData在class里面已经有类型定义了

	for (int i = 0; i < m_nSize; i++)
		m_pData[i] = arr[i];

	return *this;//就是把目前的信息return
}
