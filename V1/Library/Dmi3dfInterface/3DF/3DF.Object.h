#pragma once

#include "3DF.h"

OPEN_3DF_NAMESPACE

class API_3DF Object
{
public:
	Object();
	virtual ~Object();

	virtual TDF::Type Type() const;

	TDF::Type ObjectType() const { return TDF::Type::None; };
	PrivateImpl * GetImpl() { return m_pcImpl; }
	const PrivateImpl * GetImpl() const { return m_pcImpl; }

protected:
	friend class PrivateImpl;
	PrivateImpl * m_pcImpl = nullptr;
};

CLOSE_3DF_NAMESPACE