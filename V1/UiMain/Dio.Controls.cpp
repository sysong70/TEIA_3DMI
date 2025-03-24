#include "stdafx.h"
#include "Dio.Controls.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//**************************************************************************************************

CtlDioEdit::~CtlDioEdit()
{
	DestroyWindow();
}



void CtlDioEdit::SetValue(const CString& value)
{
	Text = value;
	State = Dio::EState::Unchanged;
	SetWindowText(value);
}

//**************************************************************************************************

WndDioItemPopup::~WndDioItemPopup()
{
	REMOVE_POINTER(ItemPtr);
}



BOOL WndDioItemPopup::Create(Dio::EControlId id)
{
	static bool Registered = false;
	static CString ClassName(L"WndDioItemPopup");

	if (Registered == false) {
		WNDCLASS wc;
		ZeroMemory(&wc, sizeof(wc));
		{
			wc.style = CS_VREDRAW | CS_HREDRAW;
			wc.lpfnWndProc = AfxWndProc;
			wc.hInstance = AfxGetInstanceHandle();
			wc.hIcon = NULL;
			wc.hCursor = LoadCursor(NULL, IDC_ARROW);
			wc.lpszClassName = ClassName;
		}
		auto result = RegisterClass(&wc);
		ASSERT(result);

		Registered = true;
	}

	// Create this popup

	const DWORD dwStyle = WS_POPUP;
	const DWORD dwExStyle = WS_EX_TRANSPARENT;
	CWnd* pParentWnd = AfxGetMainWnd();
	BOOL success = CWnd::CreateEx(dwExStyle, ClassName, L"Dio", dwStyle, {}, pParentWnd, (UINT)id);
	ASSERT(success == TRUE);

	// Create child control

	switch (id) {
	case Dio::EControlId::Length:
	case Dio::EControlId::CoordX:
	case Dio::EControlId::CoordY:
	case Dio::EControlId::CoordZ:
	{
		CtlDioEdit* pControl = new CtlDioEdit();
		pControl->Create(WS_CHILD | WS_VISIBLE, {}, this, 1);

		ItemPtr = pControl;
		break;
	}

	case Dio::EControlId::Angle:
	case Dio::EControlId::Prompt:
	{
		CtlDioLabel* pControl = new CtlDioLabel();
		ItemPtr = pControl;
		break;
	}

	case Dio::EControlId::OSnap:
		break;

	default:
		DestroyWindow();
		RETURN_FALSE;
	}

	return success;
}



BOOL WndDioItemPopup::PreCreateWindow(CREATESTRUCT& cs)
{
	//:WARNING - CWnd::CreateEx crash!!!
	cs.hMenu = NULL;

	return __super::PreCreateWindow(cs);
}
