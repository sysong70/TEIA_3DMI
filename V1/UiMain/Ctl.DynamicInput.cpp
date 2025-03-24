#include "stdafx.h"

#include "Cnt.h"
#include "Ctl.DynamicInput.h"
#include "Wnd.Application.h"
#include "Wnd.MainFrame.h"
#include "Wnd.View2d.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//**************************************************************************************************

#define theView     (*(WndView2d*)TheApp.GetMainFrame().ActiveView)
#define theDelivery (*theView.GetDelivery2d())



namespace
{
    CSize GetMargin()
    {
        return globalUtils.ScaleByDPI(CSize(6, 6));
    }
}

//**************************************************************************************************

BEGIN_MESSAGE_MAP(CtlDynamicEdit, CEdit)
    ON_WM_CHAR()
END_MESSAGE_MAP()



CtlDynamicEdit::CtlDynamicEdit()
    : CBCGPEdit()
{
    m_bVisualManagerStyle = TRUE;
}



CtlDynamicEdit::~CtlDynamicEdit()
{
    DestroyWindow();
}



void CtlDynamicEdit::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
    CtlDioWrapper* pParent = (CtlDioWrapper*)GetParent();
    DEBUG_VALID(pParent);

    if (pParent->OnChar(nChar, nRepCnt, nFlags)) {
        return;
    }

    __super::OnChar(nChar, nRepCnt, nFlags);
}

//**************************************************************************************************

BEGIN_MESSAGE_MAP(CtlDioWrapper, CWnd)
    ON_WM_ERASEBKGND()
    ON_WM_MOUSEMOVE()
    ON_WM_MOUSEWHEEL()
    ON_WM_SIZE()
END_MESSAGE_MAP()



CtlDioWrapper::~CtlDioWrapper()
{
    REMOVE_POINTER(Child);
    DestroyWindow();
}



bool CtlDioWrapper::Initialize(CWnd* pParentWnd, const RECT& rect, Dio::EControlId id)
{
    // Create this window (parent)

    static bool Registered = false;
    static CString ClassName(L"CtlDioWrapper");

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

    const DWORD dwStyle = WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_POPUP;
    const DWORD dwExStyle = 0;
    BOOL success = CWnd::CreateEx(dwExStyle, ClassName, L"", dwStyle, rect, pParentWnd, (UINT)id);
    if (success == FALSE) {
        RETURN_FALSE;
    }

    Params.Id = id;

    // Create control (child)

    switch (id) {
    case Dio::EControlId::Length:
    case Dio::EControlId::Angle:
    case Dio::EControlId::CoordX:
    case Dio::EControlId::CoordY:
    case Dio::EControlId::CoordZ: {
        const DWORD dwStyle = WS_VISIBLE | WS_CHILD | WS_CLIPCHILDREN | WS_CLIPSIBLINGS;
        CtlDynamicEdit* pControl = new CtlDynamicEdit();
        success = pControl->Create(dwStyle, rect, this, (UINT)id);
        if (success == TRUE) {
            pControl->m_bVisualManagerStyle = TRUE;
            pControl->SetFont(&globalData.fontDefaultGUI, FALSE);
            //:CHECK - style
            pControl->ModifyStyleEx(0, WS_EX_STATICEDGE);
            Child = pControl;
        }

        //const DWORD dwStyle = WS_VISIBLE | WS_CHILD | WS_CLIPCHILDREN | WS_CLIPSIBLINGS;
        //CStatic* pControl = new CStatic();
        //success = pControl->Create(L"", dwStyle, rect, this, (UINT)id);
        //if (success == TRUE) {
        //    pControl->SetFont(&globalData.fontDefaultGUI, FALSE);
        //    pControl->ModifyStyleEx(0, WS_EX_STATICEDGE);
        //    Child = pControl;
        //}
    } break;

    case Dio::EControlId::Prompt: {
        const DWORD dwStyle = WS_VISIBLE | WS_CHILD | WS_CLIPCHILDREN | WS_CLIPSIBLINGS;
        CtlDynamicEdit* pControl = new CtlDynamicEdit();
        success = pControl->Create(dwStyle, rect, this, (UINT)id);
        if (success == TRUE) {
            pControl->m_bVisualManagerStyle = TRUE;
            pControl->SetFont(&globalData.fontDefaultGUI, FALSE);
            pControl->ModifyStyleEx(0, WS_EX_STATICEDGE);
            pControl->EnableWindow(FALSE);
            Child = pControl;
        }
    } break;

    default:
        DEBUG_STOP;
        break;
    }

    ASSERT(success);
    return success;
}



CSize CtlDioWrapper::SetText(const CString& value, bool select)
{
    DEBUG_VALID(Child);
    DEBUG_VALID(Child->GetSafeHwnd())

    Params.Text = value;
    Child->SetWindowText(value);
    if (select) {
        //dynamic_cast<CtlDynamicEdit*>(Child)->SetSel(0, -1);
        Params.Activated = true;
        Child->EnableWindow(TRUE);
        Child->SetFocus();
    }
    else {
        Params.Activated = false;
        Child->EnableWindow(FALSE);
    }

    if (IsWindowVisible() == false || Child->IsWindowVisible() == false) {
        Params.VisibleWait = true;
    }

    return CalcSize();
}



CSize CtlDioWrapper::CalcSize()
{
    CString value;
    Child->GetWindowText(value);

    CDC* pDC = Child->GetDC();
    CFont* pOldFont = pDC->SelectObject(GetFont());
    CSize size = pDC->GetTextExtent(value);
    pDC->SelectObject(pOldFont);

    size += globalUtils.ScaleByDPI(CSize(8, 4));

    return size;
}

//--------------------------------------------------------------------------------------------------

BOOL CtlDioWrapper::PreCreateWindow(CREATESTRUCT& cs)
{
    //:WARNING - if not, CWnd::CreateEx crash!!!
    cs.hMenu = NULL;

    return __super::PreCreateWindow(cs);
}



bool CtlDioWrapper::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
    if (nChar == VK_ESCAPE) {
        theDelivery.UserIO.OnInput(KEY_CANCEL);
        return false;
    }

    if (nChar != VK_SPACE && nChar != VK_RETURN) {
        return false;
    }

    switch (Params.Id) {
    case Dio::EControlId::Angle: {
        CString value;
        Child->GetWindowText(value);
        //theDelivery.UserIO.OnFilterAngle(value);
    } break;

    case Dio::EControlId::Length: {
        CString value;
        Child->GetWindowText(value);
        //theDelivery.UserIO.OnFilterLength(value);
    } break;

    default:
        DEBUG_STOP;
        break;
    }

    return true;
}



BOOL CtlDioWrapper::OnEraseBkgnd(CDC* pDC)
{
    CRect rect;
    GetClientRect(rect);
    pDC->FillSolidRect(rect, RGB(255, 255, 255));

    return TRUE;
}



void CtlDioWrapper::OnMouseMove(UINT nFlags, CPoint point)
{
    ClientToScreen(&point);
    theView.ScreenToClient(&point);

    theView.OnMouseMove(nFlags, point);
}



BOOL CtlDioWrapper::OnMouseWheel(UINT nFlags, short zDelta, CPoint point)
{
    ClientToScreen(&point);
    theView.ScreenToClient(&point);

    return theView.OnMouseWheel(nFlags, zDelta, point);
}



void CtlDioWrapper::OnSize(UINT nType, int cx, int cy)
{
    __super::OnSize(nType, cx, cy);

    if (Child == nullptr || cx == 0 || cy == 0 || GetSafeHwnd() == nullptr) {
        return;
    }

    Child->MoveWindow(0, 0, cx, cy);
    Child->UpdateWindow();
}

#undef theDelivery
#undef theView
