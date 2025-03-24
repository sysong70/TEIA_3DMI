#include "stdafx.h"

#include "Uio.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//**************************************************************************************************

void UserIoParams::Clear()
{
	Command.Empty();
	Prompt.Empty();
	Keyword.Empty();
	Options = 0;

	Keywords.clear();
}



bool UserIoParams::FindKeyword(const CString& value)
{
	for (auto item : Keywords) {
		if (item.CompareNoCase(value) == 0) {
			return true;
		}
	}

	return false;
}
