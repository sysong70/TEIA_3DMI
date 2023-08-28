#include <StdAfx.h>

#include "Kernel.View.h"

#include "Kernel.Signal.Connector.h"

#include "../Signal/Signal.h"
#include "../Common/Common_Define.h"

using namespace KERNEL;

View::View()
{

}

void View::ExecuteSignal(Json::Object & cInObject)
{
	int nAction = cInObject.GetInteger(SKW_ACTION);
	int nViewId = cInObject.GetInteger(SKW_VIEWID);

	int nFlag;
	int x;
	int y;

	switch ((Signal::View::Action)nAction)
	{
		case Signal::View::Action::OnMouseMove:
		case Signal::View::Action::OnLButtonDown:
		case Signal::View::Action::OnLButtonUp:
		case Signal::View::Action::OnMButtonDown:
		case Signal::View::Action::OnMButtonUp:
		case Signal::View::Action::OnRButtonDown:
		case Signal::View::Action::OnRButtonUp:
		case Signal::View::Action::OnMouseWheel:
			nFlag = cInObject.GetInteger(SKW_FLAG);
			x = cInObject.GetInteger(SKW_X);
			y = cInObject.GetInteger(SKW_Y);
			break;
	}

	switch ((Signal::View::Action)nAction)
	{
		case Signal::View::Action::OnConstruct:
			break;

			// CView Windows에서 OnInitialize 함수에서 전달 받음.
		case Signal::View::Action::OnInitialize:
			m_cView.Initialize(cInObject, Connector::GetInstance(nViewId));
			break;

		case Signal::View::Action::OnDestruct:
			m_cView.Destruct();
			break;

		case Signal::View::Action::OnPaint:
			m_cView.Paint(cInObject);
			break;

		case Signal::View::Action::OnResize:
		{
			int nX = cInObject.GetInteger(SKW_X);
			int nY = cInObject.GetInteger(SKW_Y);
			m_cView.Resize(nX, nY);
		}
		break;

		case Signal::View::Action::OnMouseMove:
			m_cView.MouseMove(nFlag, x, y);
			break;

		case Signal::View::Action::OnLButtonDown:
			m_cView.LButtonDown(nFlag, x, y);
			break;

		case Signal::View::Action::OnLButtonUp:
			m_cView.LButtonUp(nFlag, x, y);
			break;

		case Signal::View::Action::OnMButtonDown:
			m_cView.MButtonDown(nFlag, x, y);
			break;

		case Signal::View::Action::OnMButtonUp:
			m_cView.MButtonUp(nFlag, x, y);
			break;

		case Signal::View::Action::OnRButtonDown:
			m_cView.RButtonDown(nFlag, x, y);
			break;

		case Signal::View::Action::OnRButtonUp:
			m_cView.RButtonUp(nFlag, x, y);
			break;

		case Signal::View::Action::OnMouseWheel: {
			int zDelta = cInObject.GetInteger(SKW_DELTA, -120);
			Json::Array & cArray = cInObject.GetArray(SKW_RECT);
			int nLeft = cArray[0]->ToInteger();
			int nTop = cArray[1]->ToInteger();
			m_cView.MouseWheel(nFlag, zDelta, x, y, nLeft, nTop);
			} break;

			//:Ken - 20230607
		case Signal::View::Action::OnInput:
		case Signal::View::Action::OnChar:
		case Signal::View::Action::OnKeyDown:
		case Signal::View::Action::OnKeyUp:
			m_cView.ExecuteKeyboardSignal(nAction, cInObject);
			break;

		case Signal::View::Action::OnCancel:
			m_cView.CancelCommands();
			break;
	}
}

