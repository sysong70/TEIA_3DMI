#pragma once

#include <BCGCBProInc.h>
#include "Json.h"
#include <list>



namespace Dialog
{
	class Base;

	class AppSettings;
	class FileOpenOption;
	class ProgressLog;
	class PropertyList;
	class Standard;



	typedef std::list<CWnd*> Controls;



	CSize ControlGap();

	int FooterPadding();
}
