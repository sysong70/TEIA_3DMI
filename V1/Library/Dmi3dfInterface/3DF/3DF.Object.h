#pragma once

#include "3DF.h"

OPEN_3DF_NAMESPACE

class API_3DF Object
{
public:
	Object();
	virtual ~Object();

	virtual _3DF::Type Type() const;

	_3DF::Type ObjectType() const { return _3DF::Type::None; };

protected:
	friend class PrivateImpl;
	PrivateImpl * m_pcImpl = nullptr;
};

CLOSE_3DF_NAMESPACE