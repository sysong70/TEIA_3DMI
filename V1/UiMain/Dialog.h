#pragma once

#include <Json.h>
#include <list>
#include <BCGCBProInc.h>



namespace Dialog
{
	class AppOptions;
	class Base;
	class Standard;
	class ObjectSnaps;
	class ProgressLog;
	class Standard;



	using Controls = std::list<CWnd*>;



	CSize ControlGap();

	int FooterPadding();
	// control frame padding
	CSize FramePadding();
	// for non-frame widnow
	CSize WindowPadding();
}
