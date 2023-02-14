#include "StdAfx.h"

#include "3DF.Object.h"

USING_3DF_NAMESPACE

Object::Object()
{
}

Object::~Object()
{
	if (nullptr != m_pcImpl) {
		delete m_pcImpl;  
		m_pcImpl = nullptr;
	}
}