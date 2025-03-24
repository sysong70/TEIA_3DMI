#pragma once

//--------------------------------------------------------------------------------------------------

namespace Dlg
{
	using Controls = std::list<CWnd*>;

	enum class EIndex
	{
		Unknown,
		DebugTracer,
		ProgressBar,
	};



	// Control frame padding
	CSize FramePadding();
	// For non-frame widnow
	CSize WindowPadding();
}
