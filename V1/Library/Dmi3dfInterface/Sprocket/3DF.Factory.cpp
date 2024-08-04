#include "StdAfx.h"

#include "3DF.Factory.h"

#include <HBaseModel.h>

#include "3DF.Canvas.h"
#include "Impl/CanvasImpl.h"

#include "3DF.View.h"
#include "Impl/3DF.View.Impl.h"

#include "../3DF/3DF.Utility.h"

using namespace H3DF;

Canvas * H3DF::Factory::CreateCanvas(H3DF::WindowHandle nInWindowHandle, char const * chInName, H3DF::ApplicationWindowOptionsKit const & cInOptions)
{
	Canvas * pcCanvas = new Canvas();

	CanvasImpl * pcImpl = (CanvasImpl *)pcCanvas->GetImpl();;
	if (nullptr == pcImpl) {
		assert(false);
	}

	pcImpl->m_nInWindowHandle = nInWindowHandle;

	if (nullptr != chInName) {
		Utility::CopyString(chInName, pcImpl->m_pchName);
	}

	pcImpl->m_cApplicationWindowOptionsKit = cInOptions;

	return pcCanvas;
}

View * H3DF::Factory::CreateView(CStringA strInName)
{
	View * pcView = new View();

	ViewImpl * pcImpl = (ViewImpl *)pcView->GetImpl();
	if (nullptr == pcImpl) {
		assert(false);
	}

	pcImpl->m_strName = strInName;

	return pcView;
}
