#include "StdAfx.h"

#include "OPERATOR.Select.h"

#include "Impl/OperatorImpl.h"

#include "Signal.Connector.h"
#include "../Signal/Signal.h"
#include "../Common/Common_Define.h"

#include "../../UiMain/Command.Resource.h"

#include <3DF.View.h>
#include <3DF/Window.h>
#include <3DF/Selection.h>
#include <3DF/Facility.AppOptions.h>

#include <Json.h>

using namespace KERNEL;

//== SelectImpl 관련 함수 ============================================================================

namespace KERNEL
{
	namespace Operator
	{
		class SelectImpl : public OperatorImpl
		{
		public:
			SelectImpl(const H3DF::View * pcInView, const Signal::Delivery * pcInDelivery);

			void Copy(SelectImpl * pcInThat) {
				OperatorImpl::Copy(pcInThat);
			}

			DWORD m_nSelectPickCount;
			DWORD m_nMouseDownTickCount;
			HPoint m_cMouseDownPoint;

			H3DF::SelectionResults m_cNewHighlightSelection;
			H3DF::SelectionResults m_cOldHighlightSelection;
			H3DF::SelectionResults m_cHighlightSelection;

			HPoint m_cClickPoint;
		};
	}
}

KERNEL::Operator::SelectImpl::SelectImpl(const H3DF::View * pcInView, const Signal::Delivery * pcInDelivery)
	: OperatorImpl(pcInView, pcInDelivery)
{
}

//== Select 관련 함수 ================================================================================

KERNEL::Operator::Select::Select(const H3DF::View * pcInView, const Signal::Delivery * pcInDelivery)
{
	SelectImpl * pcImpl = new SelectImpl(pcInView, pcInDelivery);
	DEBUG_VALID(pcImpl);

	m_pcImpl = pcImpl;
}

int KERNEL::Operator::Select::MouseMove(HEventInfo & cInEvent)
{
	return 0;
}

int KERNEL::Operator::Select::LButtonDown(HEventInfo & cInEvent)
{
	auto * pcImpl = dynamic_cast<SelectImpl *>(m_pcImpl);
	DEBUG_VALID(pcImpl);

	pcImpl->m_cMouseDownPoint = cInEvent.GetMousePixelPos();
	pcImpl->m_nMouseDownTickCount = GetTickCount();

	return 0;
}

int KERNEL::Operator::Select::LButtonUp(HEventInfo & cInEvent)
{
	auto * pcImpl = dynamic_cast<SelectImpl *>(m_pcImpl);
	DEBUG_VALID(pcImpl);

	DWORD nMouseUpTickCount = GetTickCount();
	DWORD nTickCount = nMouseUpTickCount - pcImpl->m_nMouseDownTickCount;

	// 2 Pixel이하 200 Tick이하에서만 선택하는 것으로 판정한다.
	if (pcImpl->m_nSelectPickCount > nTickCount) {
		const HPoint & cMoustPoint = cInEvent.GetMousePixelPos();
		HVector cVector = cMoustPoint - pcImpl->m_cMouseDownPoint;
		double dLength = HC_Compute_Vector_Length(&cVector);

		if (2.0 > dLength) {
			H3DF::Point cPoint;
			cPoint.x = cInEvent.GetMouseWindowPos().x;
			cPoint.y = cInEvent.GetMouseWindowPos().y;

			H3DF::SelectionOptionsKit cSelectOption;
			cSelectOption.SetLevel(H3DF::Selection::Level::Entity).SetRelatedLimit(15).SetProximity(0.2f);

			H3DF::SelectionResults cResult;
			size_t nResult = pcImpl->Window().GetSelectionControl().SelectByPoint(cPoint, cSelectOption, cResult);
		}
	}

	pcImpl->m_cClickPoint = cInEvent.GetMouseWorldPos();

	return 0;
}