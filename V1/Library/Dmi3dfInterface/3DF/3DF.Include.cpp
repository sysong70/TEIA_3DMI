#include "StdAfx.h"

#include "3DF.Include.h"

USING_3DF_NAMESPACE

IncludeKey::IncludeKey()
{

}

IncludeKey::IncludeKey(HC_KEY nKey)
{
	m_nKey = nKey;
}

IncludeKey::IncludeKey(IncludeKey const & cInThat)
{
	m_nKey = cInThat.KeyValue();
}


IncludeKey & IncludeKey::operator = (IncludeKey const & cOther)
{
	m_nKey = cOther.KeyValue();
	return *this;
}