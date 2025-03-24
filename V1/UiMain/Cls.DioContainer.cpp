#include "stdafx.h"

#include "Cls.DioContainer.h"
#include "Wnd.Application.h"
#include "Wnd.MainFrame.h"
#include "Wnd.View.h"

#include <Signal2d.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//**************************************************************************************************

#define theParams	UioManager.Params



namespace
{
    std::vector<CtlDioWrapper*> Wrappers;

    CPoint GetOffset()
    {
        return globalUtils.ScaleByDPI(CPoint(10, 10));
    }

    bool IsCursorInRect(CRect rect)
    {
        return false;
    }
}

//**************************************************************************************************

ClsDioContainer::ClsDioContainer(ClsUserIoManager& manager)
    : UioManager(manager)
{
    Wrappers.push_back(&LengthCtl);
    Wrappers.push_back(&AngleCtl);
    Wrappers.push_back(&PromptCtl);
    Wrappers.push_back(&CoordXCtl);
    Wrappers.push_back(&CoordYCtl);
    Wrappers.push_back(&CoordZCtl);
}



bool ClsDioContainer::Initialize(CWnd* pParentWnd)
{
    bool success = true;

    success &= LengthCtl.Initialize(pParentWnd, {}, Dio::EControlId::Length);
    success &= AngleCtl.Initialize(pParentWnd, {}, Dio::EControlId::Angle);
    success &= PromptCtl.Initialize(pParentWnd, {}, Dio::EControlId::Prompt);
    success &= CoordXCtl.Initialize(pParentWnd, {}, Dio::EControlId::CoordX);
    success &= CoordYCtl.Initialize(pParentWnd, {}, Dio::EControlId::CoordY);
    success &= CoordZCtl.Initialize(pParentWnd, {}, Dio::EControlId::CoordZ);

    ASSERT(success);
    return Valid = success;
}



void ClsDioContainer::ReceiveSignal(Json::Object* pData)
{
    MousePoint = Ctl::GetMousePos();

    ViewWnd = TheApp.GetMainFrame().ActiveView;
    ViewRect = ViewWnd->GetClientArea();
    ViewWnd->ClientToScreen(ViewRect);

    DwpHandle = ::BeginDeferWindowPos(1);

    Json::Object& data = pData->GetAt(SKW_OPTIONS);
    Json::Value* pLength = data.FindValue(SKW_LENGTH);
    Json::Value* pAngle = data.FindValue(SKW_ANGLE);

    SetAngle(pAngle);
    SetLength(pLength);

    AdjustLayout();
}



void ClsDioContainer::AdjustLayout()
{
    if (Valid == false) {
        return;
    }

    BOOL result = ::EndDeferWindowPos(DwpHandle);
    DwpHandle = nullptr;

    for (auto wrapper : Wrappers) {
        if (wrapper->Params.VisibleWait) {
            wrapper->ShowWindow(SW_SHOW);
            wrapper->Params.VisibleWait = false;
        }

        if (wrapper->IsWindowVisible()) {
            wrapper->RedrawWindow(NULL, NULL, RDW_INVALIDATE | RDW_INTERNALPAINT | RDW_UPDATENOW | RDW_ALLCHILDREN);
        }
    }
}



bool ClsDioContainer::SetLength(Json::Value* pValue)
{
    if (pValue == nullptr) {
        return false;
    }

    Json::Array& values = pValue->AsArray();
    int i = 0;

    bool lock = values.GetBoolean(i++);
    double length = values.GetReal(i++);
    int pointCount = values.GetInteger(i++);
    ASSERT(pointCount == 5);

    CPoint points[5];
    int index = 0;

    while (index < pointCount) {
        points[index].x = values.GetInteger(i++);
        points[index].y = values.GetInteger(i++);
        index++;
    }

    CPoint point = points[2];
    CSize size = LengthCtl.SetText(WStr::Format(L"%.3f", length), lock);
    point.x -= size.cx / 2;
    point.y -= size.cy / 2;

    ViewWnd->ClientToScreen(&point);
    point.x = max(ViewRect.left, point.x);
    point.y = max(ViewRect.top, point.y);
    point.x = min(ViewRect.right - size.cx, point.x);
    point.y = min(ViewRect.bottom - size.cy, point.y);

    CPoint mousePoint = Ctl::GetMousePos();
    CPoint offset = GetOffset();
    CRect rect(point, size);
    rect.InflateRect(offset.x, offset.y);

    if (rect.PtInRect(mousePoint)) {
        point.x = mousePoint.x - offset.x - size.cx;
        point.y = mousePoint.y - offset.y - size.cy;
    }

    DwpHandle = ::DeferWindowPos(DwpHandle, LengthCtl, NULL,
        point.x, point.y, size.cx, size.cy,
        SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOREDRAW | SWP_NOCOPYBITS);

    return true;
}



bool ClsDioContainer::SetAngle(Json::Value* pValue)
{
    if (pValue == nullptr) {
        return false;
    }

    Json::Array& values = pValue->AsArray();
    int i = 0;

    bool lock = values.GetBoolean(i++);
    double angle = values.GetReal(i++);
    int pointCount = values.GetInteger(i++);
    ASSERT(pointCount == 4);

    CPoint points[4];
    int index = 0;

    while (index < pointCount) {
        points[index].x = values.GetInteger(i++);
        points[index].y = values.GetInteger(i++);
        index++;
    }

    CPoint point = points[2];
    CSize size = AngleCtl.SetText(WStr::Format(L"%0.f", angle, lock));
    point.x -= size.cx / 2;
    point.y -= size.cy / 2;

    ViewWnd->ClientToScreen(&point);
    point.x = max(ViewRect.left, point.x);
    point.y = max(ViewRect.top, point.y);
    point.x = min(ViewRect.right - size.cx, point.x);
    point.y = min(ViewRect.bottom - size.cy, point.y);

    CPoint mousePoint = Ctl::GetMousePos();
    CPoint offset = GetOffset();
    CRect rect(point, size);
    rect.InflateRect(offset.x, offset.y);

    if (rect.PtInRect(mousePoint)) {
        point.x = mousePoint.x + offset.x;
        point.y = mousePoint.y - offset.y - size.cy;
    }

    DwpHandle = ::DeferWindowPos(DwpHandle, AngleCtl, NULL,
        point.x, point.y, size.cx, size.cy,
        SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOREDRAW | SWP_NOCOPYBITS);

    return true;
}

//--------------------------------------------------------------------------------------------------

bool ClsDioContainer::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
    for (auto wrapper : Wrappers) {
        if (wrapper->Params.Activated) {
            wrapper->Child->SendMessage(WM_CHAR, (WPARAM)nChar, (LPARAM)nRepCnt);
            return true;
        }
    }

    return false;
}



bool ClsDioContainer::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
    RETURN_FALSE;
}



bool ClsDioContainer::OnMouseLeave()
{
    if (PromptCtl.IsWindowVisible()) {
        PromptCtl.Params.VisibleWait = false;
        PromptCtl.ShowWindow(SW_HIDE);
    }

    return true;
}



bool ClsDioContainer::OnMouseMove(UINT nFlags, CPoint point)
{
    if (DwpHandle == nullptr) {
        DwpHandle = ::BeginDeferWindowPos(1);
    }

    SetPrompt();
    AdjustLayout();

    return true;
}

//--------------------------------------------------------------------------------------------------

bool ClsDioContainer::SetPrompt()
{
    CString value = theParams.Prompt;
    if (value.IsEmpty()) {
        if (PromptCtl.IsWindowVisible()) {
            PromptCtl.Params.VisibleWait = false;
            PromptCtl.ShowWindow(SW_HIDE);
        }

        return false;
    }

    WndView* pView = TheApp.GetMainFrame().ActiveView;
    ViewRect = pView->GetClientArea();
    pView->ClientToScreen(ViewRect);

    CPoint point = Ctl::GetMousePos();
    CPoint offset = GetOffset();
    point += offset;

    PromptCtl.GetWindowText(value);
    CSize size = value != theParams.Prompt ? PromptCtl.SetText(theParams.Prompt) : PromptCtl.CalcSize();

    DwpHandle = ::DeferWindowPos(DwpHandle, PromptCtl, NULL, point.x, point.y, size.cx, size.cy, SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOREDRAW | SWP_NOCOPYBITS);

    return true;
}



bool ClsDioContainer::SetEcho(const CString& value)
{
    return true;
}



bool ClsDioContainer::SetError(const CString& value)
{
    return true;
}



bool ClsDioContainer::StandbyCommand(const CString& prompt)
{
    for (auto wrapper : Wrappers) {
        wrapper->Params.Activated = false;
        wrapper->Params.VisibleWait = false;
        wrapper->ShowWindow(SW_HIDE);
    }

    return true;
}

#undef theParams
