#pragma once

#include "Json.h"

#include <BCGCBProInc.h>
#include <list>



namespace Dialog
{
	class Base; // CBCGPDialog

	class Standard; // Base
	class AppSettings;
	class ObjectSnaps;
	class ProgressLog;



	typedef std::list<CWnd*> Controls;



	CSize ControlGap();

	int FooterPadding();
	// control frame padding
	CSize FramePadding();
	// for non-frame widnow
	CSize WindowPadding();
}
