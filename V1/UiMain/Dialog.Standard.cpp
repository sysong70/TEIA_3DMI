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



Json::Object& Dialog::Standard::GetUiData()
{
	DEBUG_VALID(m_pDialogData);
	return *m_pDialogData;
}



BOOL Dialog::Standard::OnInitDialog()
{
	__super::OnInitDialog();

	SetWindowText(Facility::Local(GetUiData().GetString("title")));

	return TRUE;
}



CSize Dialog::Standard::SetupControl(CBCGPButton& control, Json::Object& data)
{
	const CSize defaultSize = CSize(96, 0);
	const DWORD dwStyle = WS_CHILD | WS_VISIBLE;

	if (control.GetSafeHwnd() == nullptr) {
		if (control.Create(Facility::GetTitle(data), dwStyle, {}, this, Facility::GetId(data)) == FALSE) {
			RETURN({});
		}
	}

	control.m_bVisualManagerStyle = TRUE;
	control.SizeToContent();

	return Component::AdjustSize(&control, globalUtils.ScaleByDPI(defaultSize));
}



CRect Dialog::Standard::SetupControl(CBCGPCircularProgressIndicatorCtrl& control, Json::Object& data, Component::EPivot ePivot, CRect rect)
{
	DEBUG_VALID(control.GetSafeHwnd());

	CSize size(data.GetInteger("cx"), data.GetInteger("cy"));
	CRect result = AdjustLayout(&control, rect, globalUtils.ScaleByDPI(size), ePivot);

	CBCGPCircularProgressIndicatorImpl* pProgress = control.GetCircularProgressIndicator();

	CBCGPCircularProgressIndicatorOptions options = pProgress->GetOptions();
	options.m_bMarqueeStyle = TRUE;
	options.m_Shape = CBCGPCircularProgressIndicatorOptions::BCGPCircularProgressIndicator_Arc;
	options.m_dblProgressWidth = (double)globalUtils.ScaleByDPI(result.Size().cx) * 0.1;
	pProgress->SetOptions(options);

	// clear fraem and background
	CBCGPCircularProgressIndicatorColors colors = pProgress->GetColors();
	colors.m_brFill = CBCGPBrush();
	colors.m_brFrameOutline = CBCGPBrush();

	pProgress->SetColors(colors);
	pProgress->Redraw();

	control.ShowWindow(SW_SHOWNOACTIVATE);

	return result;
}



CRect Dialog::Standard::SetupControl(CBCGPListBox& control, Json::Object& data, Component::EPivot ePivot, CRect rect)
{
	DEBUG_VALID(control.GetSafeHwnd());

	//control.SetItemExtraHeight(globalUtils.ScaleByDPI(4));
	control.EnableItemDescription(TRUE, 1);
	control.SetAlternateRowColor();

	return AdjustLayout(&control, rect, CSize(rect.Width(), rect.Height()), ePivot);
}

/*
	"Log":{"columns":[
		{"header":"Status|상태", "width":0.15},
		{"header":"File|파일", "width":0.85}
	]}
*/

CRect Dialog::Standard::SetupControl(CBCGPListCtrl& control, Json::Object& data, Component::EPivot ePivot, CRect rect)
{
	DEBUG_VALID(control.GetSafeHwnd());

	CRect result = AdjustLayout(&control, rect, CSize(rect.Width(), rect.Height()), ePivot);
	CSize size = result.Size();
	Json::Array& ar = data.GetArray("columns");

	for (int i = 0; i < ar.GetSize(); i++) {
		Json::Object& column = ar[i]->AsObject();
		CString header = column.GetString("header");
		int width = int(size.cx * column.GetReal("width"));

		control.InsertColumn(i, Facility::Local(header), LVCFMT_LEFT, width);
	}

	return result;
}



CRect Dialog::Standard::SetupControl(CBCGPStatic& control, Json::Object& data, Component::EPivot ePivot, CRect rect)
{
	DEBUG_VALID(control.GetSafeHwnd());

	control.SetWindowText(Facility::Local(data.GetString("title")));
	control.SizeToContent();

	return AdjustLayout(&control, rect, CSize(rect.Width(), 0), ePivot);
}



CRect Dialog::Standard::AlignControls(Controls controls, CPoint basePoint, Component::EAlign eAlign)
{
	CRect boundary;

	for (auto pControl : controls) {
		CRect controlFrame = GetControlRect(pControl);
		CSize controlSize = controlFrame.Size();
		CPoint pivot;

		switch (eAlign) {
		case Component::EAlign::HorizontalLeft:
			pivot.x = basePoint.x;
			pivot.y = controlFrame.top;
			break;

		case Component::EAlign::HorizontalCenter:
			pivot.x = basePoint.x - (controlSize.cx / 2);
			pivot.y = controlFrame.top;
			break;

		case Component::EAlign::HorizontalRight:
			pivot.x = basePoint.x - controlSize.cx;
			pivot.y = controlFrame.top;
			break;

		case Component::EAlign::VerticalTop:
			pivot.x = controlFrame.left;
			pivot.y = basePoint.y;
			break;

		case Component::EAlign::VerticalCenter:
			pivot.x = controlFrame.left;
			pivot.y = basePoint.y - (controlSize.cy / 2);
			break;

		case Component::EAlign::VerticalBottom:
			pivot.x = controlFrame.left;
			pivot.y = basePoint.y - controlSize.cy;
			break;

		default:
			DEBUG_STOP;
			break;
		}

		pControl->SetWindowPos(nullptr, pivot.x, pivot.y, 0, 0, SWP_NOSIZE);
		// recalculate boundary 
		controlFrame = GetControlRect(pControl);

		boundary.left = min(boundary.left, controlFrame.left);
		boundary.top = min(boundary.top, controlFrame.top);
		boundary.right = max(boundary.right, controlFrame.right);
		boundary.bottom = max(boundary.bottom, controlFrame.bottom);
	}

	return boundary;
}



CRect Dialog::Standard::DestributeControls(Controls controls, CPoint basePoint, int gap, Component::EDirection eDir)
{
	CRect boundary;
	CPoint offset = basePoint;

	for (auto pControl : controls) {
		CRect controlFrame = GetControlRect(pControl);
		CSize size = controlFrame.Size();
		CPoint pivot;

		switch (eDir) {
		case Component::EDirection::ToRight:
			pivot.x = offset.x;
			pivot.y = controlFrame.top;
			offset.x += size.cx + gap;
			break;

		case Component::EDirection::ToLeft:
			pivot.x = offset.x - size.cx;
			pivot.y = controlFrame.top;
			offset.x -= size.cx + gap;
			break;

		case Component::EDirection::ToBottom:
			pivot.x = controlFrame.left;
			pivot.y = offset.y;
			offset.y += size.cy + gap;
			break;

		case Component::EDirection::ToTop:
			pivot.x = controlFrame.left;
			pivot.y = offset.y - size.cy;
			offset.y -= size.cy + gap;
			break;

		default:
			DEBUG_STOP;
			break;
		}

		pControl->SetWindowPos(nullptr, pivot.x, pivot.y, 0, 0, SWP_NOSIZE);
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
