#include "StdAfx.h"

#include "3DF.Factory.h"

#include <HBaseModel.h>

#include "3DF.Canvas.h"
#include "Private/Canvas.Private.h"

#include "3DF.View.h"
#include "Private/View.Private.h"

#include "3DF/3DF.Utility.h"

using namespace H3DF;

Canvas H3DF::Factory::CreateCanvas(H3DF::WindowHandle nInWindowHandle, char const * chInName, H3DF::ApplicationWindowOptionsKit const & cInOptions)
{
	Canvas cCanvas;

	CanvasPrivate * pcImpl = (CanvasPrivate *)cCanvas.GetImpl();;
	if (nullptr == pcImpl) {
		assert(false);
	}

	pcImpl->m_nInWindowHandle = nInWindowHandle;

	if (nullptr != chInName) {
		Utility::CopyString(chInName, pcImpl->m_pchName);
	}

	pcImpl->m_cApplicationWindowOptionsKit = cInOptions;

	return cCanvas;
}

View H3DF::Factory::CreateView(char const * chInName)
{
	View cView;

	ViewPrivate * pcImpl = (ViewPrivate *)cView.GetImpl();
	if (nullptr == pcImpl) {
		assert(false);
	}

	if (nullptr != chInName) {
		Utility::CopyString(chInName, pcImpl->m_pchName);
	}

	return cView;
}
