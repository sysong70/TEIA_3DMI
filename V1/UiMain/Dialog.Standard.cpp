#include "stdafx.h"
#include "Dialog.Standard.h"
#include "Facility.AppResources.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif



using namespace Dialog;

IMPLEMENT_DYNAMIC(Standard, Base)

BEGIN_MESSAGE_MAP(Standard, Base)
END_MESSAGE_MAP()



Dialog::Standard::Standard(CStringA dialogName, CWnd* pParent /*=nullptr*/)
	: Base(IDD_DMI_STANDARD, pParent)
	, m_pDialogData(nullptr)
{
	if (dialogName.IsEmpty() == false) {
		m_pDialogData = &(TheAppResources.GetDialog(dialogName));
	}
}

Dialog::Standard::Standard(UINT nIDTemplate, CStringA dialogName, CWnd* pParent)
	: Base(nIDTemplate, pParent)
	, m_pDialogData(nullptr)
{
	if (dialogName.IsEmpty() == false) {
		m_pDialogData = &(TheAppResources.GetDialog(dialogName));
	}
}



Dialog::Standard::~Standard()
{
}



BOOL Dialog::Standard::OnInitDialog()
{
	__super::OnInitDialog();

	SetWindowText(GetWinTitle());

	return TRUE;
}



CSize Dialog::Standard::SetupControl(CBCGPButton& control, Json::Object& data)
{
	const CSize defaultSize = CSize(64, 0);
	const DWORD dwStyle = WS_CHILD | WS_VISIBLE;

	if (control.GetSafeHwnd() == nullptr) {
		if (control.Create(Facility::GetTitle(data), dwStyle, {}, this, Facility::GetId(data)) == FALSE) {
			RETURN({});
		}
	}

	control.m_bVisualManagerStyle = TRUE;
	control.SizeToContent();

	return Control::AdjustSize(&control, globalUtils.ScaleByDPI(defaultSize));
}



CRect Dialog::Standard::SetupControl(CBCGPStatic& control, Json::Object& data, Control::EPivot ePivot, CRect rect)
{
	DEBUG_VALID(control.GetSafeHwnd());

	control.SetWindowText(Facility::GetTitle(data));
	control.SizeToContent();

	return AdjustLayout(&control, rect, CSize(rect.Width(), 0), ePivot);
}



CRect Dialog::Standard::AlignControls(Controls controls, CPoint basePoint, Control::EAlign eAlign)
{
	CRect boundary;

	for (auto pControl : controls) {
		CRect controlFrame = GetControlRect(pControl);
		CSize controlSize = controlFrame.Size();
		CPoint pivot;

		switch (eAlign) {
		case Control::EAlign::HorizontalLeft:
			pivot.x = basePoint.x;
			pivot.y = controlFrame.top;
			break;

		case Control::EAlign::HorizontalCenter:
			pivot.x = basePoint.x - (controlSize.cx / 2);
			pivot.y = controlFrame.top;
			break;

		case Control::EAlign::HorizontalRight:
			pivot.x = basePoint.x - controlSize.cx;
			pivot.y = controlFrame.top;
			break;

		case Control::EAlign::VerticalTop:
			pivot.x = controlFrame.left;
			pivot.y = basePoint.y;
			break;

		case Control::EAlign::VerticalCenter:
			pivot.x = controlFrame.left;
			pivot.y = basePoint.y - (controlSize.cy / 2);
			break;

		case Control::EAlign::VerticalBottom:
			pivot.x = controlFrame.left;
			pivot.y = basePoint.y - controlSize.cy;
			break;

		default:
			DEBUG_STOP;
			break;
		}

		pControl->SetWindowPos(NULL, pivot.x, pivot.y, 0, 0, SWP_NOSIZE);
		// recalculate boundary 
		controlFrame = GetControlRect(pControl);

		boundary.left = min(boundary.left, controlFrame.left);
		boundary.top = min(boundary.top, controlFrame.top);
		boundary.right = max(boundary.right, controlFrame.right);
		boundary.bottom = max(boundary.bottom, controlFrame.bottom);
	}

	return boundary;
}



CRect Dialog::Standard::DestributeControls(Controls controls, CPoint basePoint, int gap, Control::EDirection eDir)
{
	CRect boundary;
	CPoint offset = basePoint;

	for (auto pControl : controls) {
		CRect controlFrame = GetControlRect(pControl);
		CSize size = controlFrame.Size();
		CPoint pivot;

		switch (eDir) {
		case Control::EDirection::ToRight:
			pivot.x = offset.x;
			pivot.y = controlFrame.top;
			offset.x += size.cx + gap;
			break;

		case Control::EDirection::ToLeft:
			pivot.x = offset.x - size.cx;
			pivot.y = controlFrame.top;
			offset.x -= size.cx + gap;
			break;

		case Control::EDirection::ToBottom:
			pivot.x = controlFrame.left;
			pivot.y = offset.y;
			offset.y += size.cy + gap;
			break;

		case Control::EDirection::ToTop:
			pivot.x = controlFrame.left;
			pivot.y = offset.y - size.cy;
			offset.y -= size.cy + gap;
			break;

		default:
			DEBUG_STOP;
			break;
		}

		pControl->SetWindowPos(NULL, pivot.x, pivot.y, 0, 0, SWP_NOSIZE);
		// recalculate boundary 
		controlFrame = GetControlRect(pControl);

		boundary.left = min(boundary.left, controlFrame.left);
		boundary.top = min(boundary.top, controlFrame.top);
		boundary.right = max(boundary.right, controlFrame.right);
		boundary.bottom = max(boundary.bottom, controlFrame.bottom);
	}

	return boundary;
}



CRect Dialog::Standard::GetControlRect(CWnd* pControl)
{
	DEBUG_VALID(pControl);

	CRect rect;
	if (pControl != nullptr && pControl->GetSafeHwnd() != nullptr) {
		pControl->GetWindowRect(&rect);
		ScreenToClient(rect);
	}
	else {
		DEBUG_STOP;
	}

	return rect;
}



Json::Object& Dialog::Standard::GetDefaultButtons()
{
	return TheAppResources.GetDialog("DefaultButtons");
}



Json::Object& Dialog::Standard::GetUiData()
{
	DEBUG_VALID(m_pDialogData);
	return *m_pDialogData;
}



CSize Dialog::Standard::GetWinSize()
{
	Json::Value* pValue = GetUiData().FindValue("size");
	if (pValue != nullptr) {
		Json::Object& data = pValue->AsObject();
		return globalUtils.ScaleByDPI(Facility::GetSize(data));
	}
	else {
		DEBUG_STOP;
		return {};
	}
}



CString Dialog::Standard::GetWinTitle()
{
	Json::Value* pValue = GetUiData().FindValue("title");
	if (pValue != nullptr) {
		return Facility::Local(pValue->ToString());
	}
	else {
		DEBUG_STOP;
		return L"";
	}
}
