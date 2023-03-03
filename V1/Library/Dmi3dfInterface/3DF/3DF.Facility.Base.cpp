#include "stdafx.h"

#include "3DF.Facility.Base.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


USING_3DF_NAMESPACE

Facility::Base::Base(CString name)
	: Name(name)
{
}

Facility::Base::~Base()
{
}
