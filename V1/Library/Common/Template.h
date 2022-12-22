#pragma once

#include <vector>

template<typename VT> VT Max(VT a, VT b)
{
	return (a > b ? a : b);
}



template <class valueType> class CLimit
{
public:

	CLimit() : m_min(valueType()), m_max(valueType()) {};

	~CLimit() {};

	valueType Evaluate(valueType vValue)
	{
		if (vValue < m_min) {
			return m_min;
		}
		else if (vValue > m_max) {
			return m_max;
		}
		else {
			return vValue;
		}
	}

	valueType GetMax()
	{
		return m_max;
	}

	valueType GetMin()
	{
		return m_min;
	}

	bool IsInBound(valueType vValue)
	{
		return (m_min <= vValue && vValue <= m_max);
	}

	void Set(valueType vMin, valueType vMax)
	{
		m_min = vMin;
		m_max = vMax;
	}

	void SetMax(valueType vValue)
	{
		m_max = vValue;
	}

	void SetMin(valueType vValue)
	{
		m_min = vValue;
	}

private:

	valueType m_min;

	valueType m_max;
};



template <class valueType>
class CLocalPtr
{
public:

	CLocalPtr() : m_pObject((valueType*)0), m_bArray(false) {}

	CLocalPtr(unsigned int nSize)
	{
		Set(nSize);
	}

	CLocalPtr(valueType* pObject, bool bArray = true)
	{
		Set(pObject, bArray);
	}

	~CLocalPtr()
	{
		Clear();
	}

	valueType* operator ->()
	{
		return static_cast<valueType*>(m_pObject);
	}

	const valueType* operator ->() const
	{
		return static_cast<const valueType*>(m_pObject);
	}

	operator valueType* ()
	{
		return static_cast<valueType*>(m_pObject);
	}

	valueType& operator [] (int i)
	{
		return m_pObject[i];
	}

	valueType const& operator [] (int i) const
	{
		return m_pObject[i];
	}

	void Clear()
	{
		if (IsValid()) {
			if (m_bArray) {
				delete[] m_pObject;
			}
			else {
				delete m_pObject;
			}

			m_pObject = ((valueType*)0);
			m_bArray = false;
		}
	}

	valueType*& Get()
	{
		return m_pObject;
	}

	void Init()
	{
		Clear();

		m_pObject = (valueType*)0;
		m_bArray = false;
	}

	bool IsValid()
	{
		return (m_pObject != (valueType*)0);
	}

	void Set(valueType* pObject, bool bArray = false)
	{
		m_pObject = pObject;
		m_bArray = bArray;
	}

	void Set(unsigned int nSize)
	{
		if (nSize == 0) {
			m_bArray = false;
			m_pObject = new valueType;
		}
		else {
			m_bArray = true;
			m_pObject = new valueType[nSize];
			::memset(m_pObject, 0, sizeof(valueType) * nSize);
		}
	}

private:

	valueType* m_pObject;

	bool m_bArray;
};



template <class valueType>
class CLocalState
{
public:

	CLocalState(valueType& variable) : m_value(variable)
	{
		m_oldValue = m_value;
	}

	CLocalState(valueType& variable, const valueType& newValue) : m_value(variable)
	{
		m_oldValue = m_value;
		m_value = newValue;
	}

	~CLocalState()
	{
		m_value = m_oldValue;
	}
	// Returns the saved value of the variable.
	operator const valueType& () const
	{
		return m_oldValue;
	}

private:

	// variable
	valueType& m_value;
	// old value
	valueType m_oldValue;
};



template<class LEFT, class RIGHT>
class CSimplePairs
{
protected:

	struct TPair
	{
		// left hand side
		const LEFT lhs;
		// right hand side
		RIGHT rhs;
	};

	typedef std::vector<TPair> TPairs;

	TPairs m_pairs;

public:

	void Add(LEFT lhs, RIGHT rhs)
	{
		TPair pair = { lhs, rhs };
		m_pairs.push_back(pair);
	}

	bool GetLeft(LEFT& lhs, const RIGHT rhs)
	{
		for (const auto& pair : m_pairs) {
			if (pair.rhs == rhs) {
				lhs = pair.lhs;
				return true;
			}
		}

		return false;
	}

	bool GetRight(const LEFT lhs, RIGHT& rhs)
	{
		for (const auto& pair : m_pairs) {
			if (pair.lhs == lhs) {
				rhs = pair.rhs;
				return true;
			}
		}

		return false;
	}

	bool HasLeft(const LEFT lhs)
	{
		for (const auto& pair : m_pairs) {
			if (pair.lhs == lhs) {
				return true;
			}
		}

		return false;
	}

	bool HasRight(const RIGHT rhs)
	{
		for (const auto& pair : m_pairs) {
			if (pair.rhs == rhs) {
				return true;
			}
		}

		return false;
	}
};
