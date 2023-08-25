#include "stdafx.h"
#include "Session.Facility.Base.h"

#ifdef _DEBUG
#	define new DEBUG_NEW
#	undef THIS_FILE
	static char THIS_FILE[] = __FILE__;
#endif


SESSION::Facility::Base::Base(CString name)
	: Name(name)
{
}


SESSION::Facility::Base::~Base()
{
}
