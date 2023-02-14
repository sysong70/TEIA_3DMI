#include "StdAfx.h"

#include "3DF.Include.h"

USING_3DF_NAMESPACE

IncludeKey::IncludeKey(HC_KEY nInKey) :
	Key(nInKey)
{
	
}

IncludeKey::IncludeKey(IncludeKey const & cInThat):
	Key(cInThat.KeyValue())
{
}

void IncludeKey::Set(IncludeKey const & cInThat)
{
	Key::Set(cInThat);
}

IncludeKey & IncludeKey::operator = (IncludeKey const & cInThat)
{
	Key::Set(cInThat);
	return *this;
}