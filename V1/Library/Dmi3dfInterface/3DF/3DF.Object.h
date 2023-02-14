#pragma once

#include "3DF.h"

OPEN_3DF_NAMESPACE

class API_3DF Object
{
public:
	Object();
	virtual ~Object();

protected:
	PrivateImpl * m_pcImpl = nullptr;
};

CLOSE_3DF_NAMESPACE