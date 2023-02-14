#include "stdafx.h"
#include "header_.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif



#define PRESET PresetClass_

namespace PresetClass_
{
}



using namespace namespace_;

BEGIN_MESSAGE_MAP(class_, CWnd)
END_MESSAGE_MAP()



NAMESPACE::class_::class_()
{
}



NAMESPACE::class_::~class_()
{
}



void NAMESPACE::class_::PostNcDestroy()
{
	__super::PostNcDestroy();
}



int NAMESPACE::class_::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (__super::OnCreate(lpCreateStruct) == -1) {
		DEBUG_STOP;
		return -1;
	}

	return 0;
}

#undef PRESET
