#include "StdAfx.h"

#include "3DF.h"
#include "3DF.Object.h"

#include <iostream>
#include <string>
#include <vector>

/*
#ifdef max
#	undef max
#	include <boost/pool/pool_alloc.hpp>
#	define max(a,b) (((a) > (b)) ? (a) : (b))
#else
#	include <boost / pool / pool_alloc.hpp>
#endif

template<class T>
class CBoostPool
{
public:
	void * operator new(size_t _size)
	{
		return m_bpool.malloc();
	}
	void operator delete(void * _p)
	{
		m_bpool.free(_p);
	}
protected:
	static boost::pool<> m_bpool;
};
template<class T>
boost::pool<> CBoostPool<T>::m_bpool(sizeof(T));
*/

// using IntArray = std::vector<int, boost::pool_allocator<int>>;
// using FloatArray = std::vector<float, boost::pool_allocator<float>>;
// using PointArray = std::vector<_3DF::Point, boost::pool_allocator<Point>>;
// using VectorArray = std::vector<_3DF::Vector, boost::pool_allocator<Vector>>;


USING_3DF_NAMESPACE

//== 메모리 관리 =====================================================================================

void * Memory::Allocate(size_t nInBytes, bool bInClearMemory)
{
	void * pByte = new BYTE[nInBytes];// s_pool.Allocate(nInBytes);
	if (true == bInClearMemory) {
		memset(pByte, 0, nInBytes);
	}

	return pByte;
}

void Memory::Free(void * pInPointer)
{
 	delete[] pInPointer;
	pInPointer = nullptr;
}

Memory::Memory() {}


//== 객체 관리 ======================================================================================
TDF::Type PrivateImpl::Type() const {
	return m_eType;
}

void PrivateImpl::SetType(TDF::Type eType) {
	m_eType = eType;
}

void PrivateImpl::SetImpl(Object * pcObject, PrivateImpl * pcImpl)
{
	if (nullptr == pcImpl) {
		DEBUG_RETURN;
	}

	if(nullptr != pcObject->m_pcImpl) {
		delete pcObject->m_pcImpl;
	}

	pcObject->m_pcImpl = pcImpl;
}