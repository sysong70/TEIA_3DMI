#include "StdAfx.h"

#include "3DF.Factory.h"

#include <HBaseModel.h>

#include "3DF.Canvas.h"
#include "Impl/CanvasImpl.h"

#include "../3DF/Selection.h"
#include "../3DF/Impl/Selection.Impl.h"
#include "../3DF/Highlight.h"
#include "../3DF/Impl/HighlightImpl.h"

#include "../3DF/Impl/WindowImpl.h"

#include "3DF.View.h"
#include "Impl/3DF.View.Impl.h"

#include "3DF.Model.h"
#include "Impl/ModelImpl.h"

#include "../3DF/3DF.Utility.h"

#include <memory>

#define TheKenel TheAppOptions.Kernel
#define ThePreset TheAppOptions.Preset

using namespace H3DF;

Canvas * H3DF::Factory::CreateCanvas(H3DF::WindowHandle nInWindowHandle, char const * chInName, H3DF::ApplicationWindowOptionsKit const & cInOptions)
{
	SegmentKey cTestSegment;

	Canvas * pcCanvas = new Canvas();

	CanvasImpl * pcCanvasImpl = (CanvasImpl *)pcCanvas->GetImpl();;
	if (nullptr == pcCanvasImpl) {
		assert(false);
	}

	pcCanvasImpl->m_nInWindowHandle = nInWindowHandle;

	if (nullptr != chInName) {
		Utility::CopyString(chInName, pcCanvasImpl->m_pchName);
	}

	pcCanvasImpl->m_cApplicationWindowOptionsKit = cInOptions;

	// HBaseView를 생성하고 초기화 한다. 그 값은 WindowsKey에 저장한다.
	WindowKeyImpl * pcWindowImpl = dynamic_cast<WindowKeyImpl *>(pcCanvasImpl->m_cWindow.GetImpl());
	if (nullptr == pcWindowImpl) {
		DEBUG_STOP;
		return nullptr;
	}

	ModelImpl * pcModelImpl = static_cast<ModelImpl *>(pcCanvasImpl->m_pcModel->GetImpl());
	DEBUG_VALID(pcModelImpl);

	// 내부에서 BaseView를 생성한다. (HBaseView 기반 Class)
	if (false == pcWindowImpl->Init(pcCanvasImpl->m_pcModel, Utility::ToChar(TheKenel.General.Display.Driver), chInName, nInWindowHandle)) {
		DEBUG_STOP;
		return nullptr;
	}

	// Navigation Cube 설정
	pcWindowImpl->m_pcNaviCube = new NavigationCube(pcWindowImpl->m_pcBaseView, &pcCanvasImpl->m_cWindow);

	WindowKeyImpl::SetSelectionControl(pcCanvasImpl->m_cWindow);
	WindowKeyImpl::SetHighlightControl(pcCanvasImpl->m_cWindow);

	pcWindowImpl->SetType(H3DF::Type::WindowKey);

	pcCanvasImpl->SetType(H3DF::Type::Canvas);

	return pcCanvas;
}

View H3DF::Factory::CreateView(CStringA strInName)
{
	View cView;

	ViewImpl * pcImpl = (ViewImpl *) cView.GetImpl();
	if (nullptr == pcImpl) {
		assert(false);
	}

	pcImpl->m_strName = strInName;

	return cView;
}
