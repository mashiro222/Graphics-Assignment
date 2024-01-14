// implementation of class DArray
//С��ҵ3���ڲ�֪����ô��vs��cmake�࿪һ����Ŀ���Ծ�ֱ��д�ļ��ˣ�����practice��src���棩����֪��զ��ʵ����
#include<iostream>
#include<cassert>
#include"DArray.h"
using namespace std;
//ʹ����ģ�������Ҫ��һ��<T(���Լ���Ĳ�������)> 
// default constructor
template<class T>//ģ��ͷ 
DArray<T>::DArray() {
	Init();
}

// set an array with default values
template<class T> 
DArray<T>::DArray(int nSize, const T& dValue)//samples��������д�ܺÿ�
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
	m_nMax = 0;//��Ҫ����m_nMax��
}

// get the size of the array
template<class T> 
int DArray<T>::GetSize() const {
	return m_nSize; // you should return a correct value
}

// allocate enough memory
template<class T> 
void DArray<T>::Reserve(int nSize) {//�����practice����û�У����Լ��ӽ�����
	//�����������˼���ǰ�m_nMax���úã�����allocate enough memoryҲ��������Ҫ�����ֱ�ӿ���sample��
	if ( m_nMax>= nSize) return;
	while (m_nMax < nSize) {
		//��һ�����Ҿ���д������Ī�������һ���֣�������˵��Ҫ��m_nMax��nSize��
		//ֱ����m_nMax=nSize+1�����ˣ���̫���ΪɶҪ��ôд
		m_nMax = (m_nMax == 0) ? 1 : 2 * m_nMax;
	}
	T* pData = new T[m_nMax];//����m_pData������������Ҫ�Ĵ�С�ˣ����ھ͸�ֵ��ȥ
	memcpy(pData, m_pData, m_nSize * sizeof(T));//�������ˣ����ǲ���vector����ֱ�Ӹ�ֵ����
	//���ﲻ����forѭ����ֵ������������ԭ�������ǿ�����m_nMax���ȵ�array����һ����m_nSize������ֻ�Ǳ�nSize�󣡣���������͵�һ��С��ϰ������ǲ�һ����
	delete[] m_pData;
	m_pData = pData;
}

// set the size of the array
//���������ʼ�����ֳ�����m_nMaxǰ��������ˣ�ϸϸ���
template<class T> 
void DArray<T>::SetSize(int nSize) {
	//���ǿ�һ��nSize���ȵ�array
	if (m_nSize == nSize) return;
	Reserve(nSize);//m_nMax���úã�����������ô��ֵ�����ᳬ��
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
	//�������������ʲô��˼�ˣ����Reserve�еķ�����������
	//��ʱ��Ϳռ�����֮��Ѱ��һ��ƽ�⣬��ľ���
	m_pData[m_nSize] = dValue;
	m_nSize += 1;
}

// delete an element at some index
template<class T> 
void DArray<T>::DeleteAt(int nIndex) {//���nIndex�Ǵ�0��ʼ��ģ�����˵�ڼ���
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
