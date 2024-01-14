// implementation of class DArray
//小作业3由于不知道怎么用vs和cmake多开一个项目所以就直接写文件了（放在practice的src里面），不知道咋搞实在是
#include<iostream>
#include<cassert>
#include"DArray.h"
using namespace std;
//使用了模板的类需要加一个<T(你自己设的参数名字)> 
// default constructor
template<class T>//模板头 
DArray<T>::DArray() {
	Init();
}

// set an array with default values
template<class T> 
DArray<T>::DArray(int nSize, const T& dValue)//samples里面这样写很好看
	:m_pData(new T[nSize]),m_nSize(nSize),m_nMax(nSize)
{
	for (int i = 0; i < nSize; i++) {
		m_pData[i] = dValue;
	}
}
template<class T> 
DArray<T>::DArray(const DArray& arr) 
	:m_pData(new T[arr.m_nSize]),m_nSize(arr.m_nSize),m_nMax(arr.m_nSize)
{
	for (int i = 0; i < m_nSize; i++) {
		m_pData[i] = arr.m_pData[i];
	}
}

// deconstructor
template<class T> 
DArray<T>::~DArray() {
	Free();
}

// display the elements of the array
template<class T> 
void DArray<T>::Print() const {
	cout << "size= " << m_nSize << ":";
	for (int i = 0; i < m_nSize; i++) {
		cout << " " << m_pData[i];
	}
	cout << endl;
}

// initilize the array
template<class T> 
void DArray<T>::Init() {
	m_pData=nullptr;
	m_nMax = 0;
	m_nSize = 0;
}

// free the array
template<class T> 
void DArray<T>::Free() {
	delete[] m_pData;
	m_pData = nullptr;
	m_nSize = 0;
	m_nMax = 0;//不要忘记m_nMax了
}

// get the size of the array
template<class T> 
int DArray<T>::GetSize() const {
	return m_nSize; // you should return a correct value
}

// allocate enough memory
template<class T> 
void DArray<T>::Reserve(int nSize) {//这个在practice里面没有，我自己加进来了
	//这个函数的意思就是把m_nMax设置好，但就allocate enough memory也看不出来要干嘛，就直接看了sample了
	if ( m_nMax>= nSize) return;
	while (m_nMax < nSize) {
		//这一步是我觉得写的最精妙但是莫名其妙的一部分，正常来说想要让m_nMax比nSize大，
		//直接让m_nMax=nSize+1就行了，不太理解为啥要这么写
		m_nMax = (m_nMax == 0) ? 1 : 2 * m_nMax;
	}
	T* pData = new T[m_nMax];//这样m_pData就满足我们想要的大小了，现在就赋值过去
	memcpy(pData, m_pData, m_nSize * sizeof(T));//老朋友了，但是不如vector里面直接赋值好用
	//这里不能用for循环赋值！！！！！，原因是我们开的是m_nMax长度的array，不一定比m_nSize大，这里只是比nSize大！！！，这个和第一个小练习里面的是不一样的
	delete[] m_pData;
	m_pData = pData;
}

// set the size of the array
//这个函数开始就显现出设置m_nMax前后的区别了，细细体会
template<class T> 
void DArray<T>::SetSize(int nSize) {
	//就是开一个nSize长度的array
	if (m_nSize == nSize) return;
	Reserve(nSize);//m_nMax设置好，这样后面怎么赋值都不会超了
	for (int i = m_nSize; i < nSize; i++) {
		m_pData[i] = 0;
	}
	m_nSize = nSize;
}

// get an element at an index
template<class T> 
const T& DArray::GetAt(int nIndex) const {
	assert(nIndex >= 0 && nIndex < m_nSize);
	
	return m_pData[nIndex]; // you should return a correct value
}

// set the value of an element 
template<class T> 
void DArray<T>::SetAt(int nIndex, const T& dValue) {
	assert(nIndex >= 0 && nIndex < m_nSize);
	m_pData[nIndex] = dValue;
}

// overload operator '[]'
template<class T> 
T& DArray<T>::operator[](int nIndex) {
	assert(nIndex >= 0 && nIndex < m_nSize);
	return m_pData[nIndex]; // you should return a correct value
}

// overload operator '[]'
template<class T> 
const T& DArray<T>::operator[](int nIndex) const {
	assert(nIndex >= 0 && nIndex < m_nSize);
	return m_pData[nIndex]; // you should return a correct value
}

// add a new element at the end of the array
template<class T> 
void DArray<T>::PushBack(const T& dValue) {
	Reserve(m_nSize + 1);
	//到这里我理解是什么意思了，这个Reserve中的翻倍操作就是
	//在时间和空间两者之间寻求一个平衡，真的精妙
	m_pData[m_nSize] = dValue;
	m_nSize += 1;
}

// delete an element at some index
template<class T> 
void DArray<T>::DeleteAt(int nIndex) {//这个nIndex是从0开始算的，不是说第几个
	assert(nIndex >= 0 && nIndex < m_nSize);
	for (int i = nIndex + 1; i < m_nSize; i++) {
		m_pData[i - 1] = m_pData[i];
	}
	m_nSize--;
}

// insert a new element at some index
template<class T> 
void DArray<T>::InsertAt(int nIndex, double dValue) {
	assert(nIndex >= 0 && nIndex <= m_nSize);
	Reserve(m_nSize + 1);
	for (int i = m_nSize-1; i >= nIndex; i--) {
		m_pData[i + 1] = m_pData[i];
	}
	m_pData[nIndex] = dValue;
	m_nSize++;
}

// overload operator '='
template<class T> 
DArray& DArray<T>::operator = (const DArray& arr) {
	Reserve(arr.m_nSize);
	m_nSize = arr.m_nSize;
	memcpy(m_pData, arr.m_pData, arr.m_nSize * sizeof(T));
	return *this;
}
