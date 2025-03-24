#pragma once

//--------------------------------------------------------------------------------------------------

namespace Uio
{
}



struct UserIoParams
{
	CString Command;	// "LINE "
	CString Prompt;		// "Specify next point or "
	CString Keyword;	// [ Undo(U) Close(C) ]:
	int Options = 0;

	WStringArray Keywords;

	void Clear();

	bool FindKeyword(const CString& value);
};
