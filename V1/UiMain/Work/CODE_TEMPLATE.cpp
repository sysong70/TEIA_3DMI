#include "stdafx.h"
#include "CODE_TEMPLATE.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif



#define PRESET PresetCLASS

namespace PresetCLASS
{
}



using namespace NAMESPACE;

BEGIN_MESSAGE_MAP(CLASS, CWnd)
END_MESSAGE_MAP()



NAMESPACE::CLASS::CLASS()
{
}



NAMESPACE::CLASS::~CLASS()
{
}



void NAMESPACE::CLASS::PostNcDestroy()
{
	__super::PostNcDestroy();
}



int NAMESPACE::CLASS::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (__super::OnCreate(lpCreateStruct) == -1) {
		DEBUG_STOP;
		return -1;
	}

	return 0;
}
