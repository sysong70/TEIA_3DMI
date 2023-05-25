#include "stdafx.h"
#include "Facility.Base.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#ifdef _3DF_EXPORT
USING_3DF_NAMESPACE
#endif



Facility::Base::Base(CString name)
	: Name(name)
{
}



Facility::Base::~Base()
{
}
