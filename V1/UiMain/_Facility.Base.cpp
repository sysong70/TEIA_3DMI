#include "stdafx.h"
#include "Facility.Base.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif



Facility::Base::Base(CString name)
	: Name(name)
{
}



Facility::Base::~Base()
{
}
