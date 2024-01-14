#include <iostream>

using namespace std;

#if !defined _DYNAMICARRAY_H_
#define _DYNAMICARRAY_H_

#define ARRAYINCREMENT 20
#define TRUE	1
#define FALSE	0

template<class DataType>
class CDArray {
private:
	DataType *m_pData;
	int m_nSize;
	int m_nMax;		// 预留给动态数组的内存大小

private:
	void Init();
	void Free();
	inline bool InvalidateIndex(int nIndex);	// 判断下标的合法性

public:
	CDArray();
	CDArray(int nSize, DataType dValue = 0);
	CDArray(const CDArray& arr);
	~CDArray();

	void Print();
	int GetSize();
	int SetSize(int nSize);
	DataType GetAt(int nIndex);
	DataType* GetFPoint();
	DataType operator[](int nIndex) const;
	int SetAt(int nIndex, DataType dValue);
	int PushBack(DataType dValue);
	DataType PopBack();
	int DeleteAt(int nIndex);
	int InsertAt(int nIndex, DataType dValue);
	CDArray& operator=(const CDArray& array);
};

template<class DataType>
void CDArray<DataType>::Init() {
	m_nSize = 0;
	m_pData = NULL;
	m_nMax = 0;
}
template<class DataType>
void  CDArray<DataType>::Free() {
	if (m_pData) {
		delete[]m_pData;
		m_pData = NULL;
		m_nSize = 0;
	}
}
template<class DataType>
inline bool  CDArray<DataType>::InvalidateIndex(int nIndex) {
	if (nIndex < 0 || nIndex > m_nSize) return true;
	else return false;
}

template<class DataType>
CDArray<DataType>::CDArray() {
	Init();
}
template<class DataType>
CDArray<DataType>::CDArray(int nSize, DataType dValue) {
	m_pData = new DataType[nSize];
	for (int i = 0; i < nSize; i++) {
		m_pData[i] = dValue;
	}
	m_nSize = nSize;
	m_nMax = nSize;
}
template<class DataType>
CDArray<DataType>::CDArray(const CDArray& arr) {
	m_nSize = arr.m_nSize;
	m_pData = new DataType[m_nSize];
	for (int i = 0; i < m_nSize; i++) {
		m_pData[i] = arr.m_pData[i];
	}
	m_nMax = arr.m_nMax;
}
template<class DataType>
CDArray<DataType>::~CDArray() {
	Free();
}

template<class DataType>
void  CDArray<DataType>::Print() {
	cout << "size= " << m_nSize << ":" << endl;
	for (int i = 0; i < m_nSize; i++) {
		cout << " " << m_pData[i];
	}
	cout << endl;
}
template<class DataType>
int  CDArray<DataType>::GetSize() {
	return m_nSize;
}
template<class DataType>
int  CDArray<DataType>::SetSize(int nSize) {
	DataType zero;
	if (m_pData == NULL) {
		m_pData = new DataType[nSize];
		if (m_pData == NULL) {
			cout << "Can not alloc memory in SetSize()!" << endl;
			return FALSE;
		}
		for (int i = 0; i < nSize; i++) {
			m_pData[i] = zero;
		}
		m_nSize = nSize;
	}
	else if (m_nSize >= nSize) {
		DataType *pTemp = NULL;
		pTemp = new DataType[nSize];
		if (pTemp == NULL) {
			cout << "Can not alloc memory in DeleteAt()!" << endl;
			return FALSE;
		}

		for (int i = 0; i < nSize; i++) {
			pTemp[i] = m_pData[i];
		}
		delete[]m_pData;
		m_pData = pTemp;
		m_nSize = nSize;
	}
	else {
		DataType *pTemp = NULL;
		pTemp = new DataType[nSize];
		if (pTemp == NULL) {
			cout << "Can not alloc memory in SetSize()!" << endl;
			return FALSE;
		}

		for (int i = 0; i < m_nSize; i++) {
			pTemp[i] = m_pData[i];
		}
		delete[]m_pData;
		m_pData = pTemp;
		for (int i = m_nSize; i < nSize; i++) {
			m_pData[i] = zero;
		}
		m_nSize = nSize;
	}
	m_nMax = nSize;
	return TRUE;
}
template<class DataType>
DataType  CDArray<DataType>::GetAt(int nIndex) {
	if (InvalidateIndex(nIndex)) {
		cout << "Index is invalide in GetAt()!" << endl;
		return DataType();
	}
	else return m_pData[nIndex];
}
template<class DataType>
int  CDArray<DataType>::SetAt(int nIndex, DataType dValue) {
	if (InvalidateIndex(nIndex)) {
		cout << "Index is invalide in SetAt()!" << endl;
		return FALSE;
	}
	else {
		m_pData[nIndex] = dValue;
		return TRUE;
	}
}
template<class DataType>
int  CDArray<DataType>::PushBack(DataType dValue) {
	if (m_nSize == m_nMax) {
		DataType *p;
		p = (DataType*)realloc(m_pData, (m_nSize + ARRAYINCREMENT) * sizeof(DataType));
		if (!p) {
			cout << "Can not alloc memory in PushBack()!" << endl;
			return FALSE;
		}
		m_pData = p;
		m_nMax = m_nSize + ARRAYINCREMENT;
	}
	m_pData[m_nSize++] = dValue;
	return TRUE;
}
template<class DataType>
DataType  CDArray<DataType>::PopBack() {
	if (m_nSize == 0) {
		cout << "Index is invalide in PopBack()!" << endl;
		return DataType();
	}
	m_nSize--;
	return m_pData[m_nSize+1];
}
template<class DataType>
int  CDArray<DataType>::DeleteAt(int nIndex) {
	if (InvalidateIndex(nIndex)) {
		cout << "Index is invalide in DeleteAt()!" << endl;
		return FALSE;
	}
	else {
		for (int i = nIndex; i < m_nSize - 1; i++) {
			m_pData[i] = m_pData[i + 1];
		}
		m_nSize--;
	}
	return TRUE;
}
template<class DataType>
int  CDArray<DataType>::InsertAt(int nIndex, DataType dValue) {
	if (InvalidateIndex(nIndex)) {
		cout << "Index is invalide in InsertAt()!" << endl;
		return FALSE;
	}
	else {
		if (m_nSize == m_nMax) {
			DataType *p;
			p = (DataType*)realloc(m_pData, (m_nSize + ARRAYINCREMENT) * sizeof(DataType));
			if (!p) {
				cout << "Can not alloc memory in InsertAt()!" << endl;
				return FALSE;
			}
			m_pData = p;
			m_nMax = m_nSize + ARRAYINCREMENT;
		}
		for (int i = m_nSize; i > nIndex; i--) {
			m_pData[i] = m_pData[i - 1];
		}
		m_pData[nIndex] = dValue;
		m_nSize++;
	}
	return TRUE;
}

template<class DataType>
CDArray<DataType>&  CDArray<DataType>::operator=(const CDArray& array) {
	m_nSize = array.m_nSize;
	m_pData = new DataType[m_nSize];
	for (int i = 0; i < m_nSize; i++) {
		m_pData[i] = array.m_pData[i];
	}
	m_nMax = array.m_nMax;
	return *this;
}
template<class DataType>
DataType  CDArray<DataType>::operator[](int nIndex) const {
	return *(m_pData + nIndex);
}

template<class DataType>
DataType* CDArray<DataType>::GetFPoint(){
	return m_pData;
}

#endif