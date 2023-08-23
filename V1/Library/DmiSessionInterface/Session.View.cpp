#include <StdAfx.h>

#include "Session.View.h"

#include "Session.Signal.Connector.h"

#include "../Signal/Signal.h"
#include "../Common/Common_Define.h"

using namespace SESSION;

View::View()
{

}

void View::ExecuteSignal(Json::Object & cInObject)
{
	int nAction = cInObject.GetInteger(SKW_ACTION);
	int nViewId = cInObject.GetInteger(SKW_VIEWID);

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
		case Signal::View::Action::OnLButtonDown:
		case Signal::View::Action::OnLButtonUp:
		case Signal::View::Action::OnMButtonDown:
		case Signal::View::Action::OnMButtonUp:
		case Signal::View::Action::OnRButtonDown:
		case Signal::View::Action::OnRButtonUp:
		case Signal::View::Action::OnMouseWheel:
			m_cView.ExecuteMouseSignal(nAction, cInObject);
			break;

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