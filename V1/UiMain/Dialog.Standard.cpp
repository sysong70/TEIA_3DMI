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
	const CSize defaultSize = CSize(96, 32);
	const DWORD dwStyle = WS_CHILD | WS_VISIBLE;

	if (control.GetSafeHwnd() == nullptr) {
		if (control.Create(GetTitle(data), dwStyle, {}, this, GetId(data)) == FALSE) {
			RETURN({});
		}
	}

	control.m_bVisualManagerStyle = TRUE;
	control.SizeToContent();

	return AdjustSize(&control, globalUtils.ScaleByDPI(defaultSize));
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



CRect Dialog::Standard::AdjustLayout(CWnd* pControl, CRect frame, CSize baseSize, Component::EPivot ePivot)
{
	CSize size = GetControlSize(pControl);
	size.cx = max(size.cx, baseSize.cx);
	size.cy = max(size.cy, baseSize.cy);

	CPoint pivot;

	switch (ePivot) {
	case Component::EPivot::TopLeft:
		pivot.x = frame.left;
		pivot.y = frame.top;
		break;

	case Component::EPivot::TopCenter:
		pivot.x = frame.CenterPoint().x - (size.cx / 2);
		pivot.y = frame.top;
		break;

	case Component::EPivot::TopRight:
		pivot.x = frame.right - size.cx;
		pivot.y = frame.top;
		break;

	case Component::EPivot::MiddleLeft:
		pivot.x = frame.left;
		pivot.y = frame.CenterPoint().y - (size.cy / 2);
		break;

	case Component::EPivot::MiddleCenter:
		pivot.x = frame.CenterPoint().x - (size.cx / 2);
		pivot.y = frame.CenterPoint().y - (size.cy / 2);
		break;

	case Component::EPivot::MiddleRight:
		pivot.x = frame.right - size.cx;
		pivot.y = frame.CenterPoint().y - (size.cy / 2);
		break;

	case Component::EPivot::BottomLeft:
		pivot.x = frame.left;
		pivot.y = frame.bottom - size.cy;
		break;

	case Component::EPivot::BottomCenter:
		pivot.x = frame.CenterPoint().x - (size.cx / 2);
		pivot.y = frame.bottom - size.cy;
		break;

	case Component::EPivot::BottomRight:
		pivot.x = frame.right - size.cx;
		pivot.y = frame.bottom - size.cy;
		break;

	default:
		DEBUG_STOP;
		break;
	}

	// move and resize
	pControl->SetWindowPos(nullptr, pivot.x, pivot.y, size.cx, size.cy, 0);

	return { pivot, size };
}



CRect Dialog::Standard::AdjustPosition(CWnd* pControl, CRect frame, Component::EPivot ePivot)
{
	CSize size = GetControlSize(pControl);
	CPoint pivot;

	switch (ePivot) {
	case Component::EPivot::TopLeft:
		pivot.x = frame.left;
		pivot.y = frame.top;
		break;

	case Component::EPivot::TopCenter:
		pivot.x = frame.CenterPoint().x - (size.cx / 2);
		pivot.y = frame.top;
		break;

	case Component::EPivot::TopRight:
		pivot.x = frame.right - size.cx;
		pivot.y = frame.top;
		break;

	case Component::EPivot::MiddleLeft:
		pivot.x = frame.left;
		pivot.y = frame.CenterPoint().y - (size.cy / 2);
		break;

	case Component::EPivot::MiddleCenter:
		pivot.x = frame.CenterPoint().x - (size.cx / 2);
		pivot.y = frame.CenterPoint().y - (size.cy / 2);
		break;

	case Component::EPivot::MiddleRight:
		pivot.x = frame.right - size.cx;
		pivot.y = frame.CenterPoint().y - (size.cy / 2);
		break;

	case Component::EPivot::BottomLeft:
		pivot.x = frame.left;
		pivot.y = frame.bottom - size.cy;
		break;

	case Component::EPivot::BottomCenter:
		pivot.x = frame.CenterPoint().x - (size.cx / 2);
		pivot.y = frame.bottom - size.cy;
		break;

	case Component::EPivot::BottomRight:
		pivot.x = frame.right - size.cx;
		pivot.y = frame.bottom - size.cy;
		break;

	default:
		DEBUG_STOP;
		break;
	}

	pControl->SetWindowPos(nullptr, pivot.x, pivot.y, 0, 0, SWP_NOSIZE);

	return { pivot, size };
}



CSize Dialog::Standard::AdjustSize(CWnd* pControl, CSize baseSize)
{
	CSize size = GetControlSize(pControl);
	size.cx = max(size.cx, baseSize.cx);
	size.cy = max(size.cy, baseSize.cy);

	pControl->SetWindowPos(nullptr, 0, 0, size.cx, size.cy, SWP_NOMOVE);

	return size;
}



CRect Dialog::Standard::AlignControls(Controls controls, CPoint basePoint, Component::EAlign eAlign)
{
	CRect totalBound;
	CRect controlBound;

	for (auto pControl : controls) {
		pControl->GetClientRect(controlBound);
		CSize controlSize = controlBound.Size();
		CPoint pivot = controlBound.TopLeft();

		switch (eAlign) {
		case Component::EAlign::HorizontalLeft:
			pivot.x = basePoint.x;
			break;

		case Component::EAlign::HorizontalCenter:
			pivot.x = basePoint.x - (controlSize.cx / 2);
			break;

		case Component::EAlign::HorizontalRight:
			pivot.x = basePoint.x - controlSize.cx;
			break;

		case Component::EAlign::VerticalTop:
			pivot.y = basePoint.y;
			break;

		case Component::EAlign::VerticalCenter:
			pivot.y = basePoint.y - (controlSize.cy / 2);
			break;

		case Component::EAlign::VerticalBottom:
			pivot.y = basePoint.y - controlSize.cy;
			break;

		default:
			DEBUG_STOP;
			break;
		}

		pControl->SetWindowPos(nullptr, pivot.x, pivot.y, 0, 0, SWP_NOSIZE);
		// recalculate boundary 
		pControl->GetClientRect(controlBound);

		totalBound.left = min(totalBound.left, controlBound.left);
		totalBound.top = min(totalBound.top, controlBound.top);
		totalBound.right = max(totalBound.right, controlBound.right);
		totalBound.bottom = max(totalBound.bottom, controlBound.bottom);
	}

	return totalBound;
}



CRect Dialog::Standard::DestributeControls(Controls controls, CPoint basePoint, int gap, Component::EDirection eDir)
{
	CRect boundary;
	CRect controlFrame;
	CPoint pivot = basePoint;

	for (auto pControl : controls) {
		pControl->GetWindowRect(controlFrame);
		CSize size = controlFrame.Size();
		CPoint offset;

		switch (eDir) {
		case Component::EDirection::ToRight:
			pivot.y = basePoint.y - size.cy / 2;
			offset.x += size.cx + gap;
			break;

		case Component::EDirection::ToLeft:
			pivot.x -= size.cx;
			pivot.y = basePoint.y - size.cy / 2;
			offset.x = -(size.cx + gap);
			break;

		case Component::EDirection::ToBottom:
			offset.y = size.cx + gap;
			break;

		case Component::EDirection::ToTop:
			pivot.y -= size.cy;
			offset.y = -(size.cy + gap);
			break;

		default:
			DEBUG_STOP;
			break;
		}

		pControl->SetWindowPos(nullptr, pivot.x, pivot.y, 0, 0, SWP_NOSIZE);
		// recalculate boundary 
		pControl->GetWindowRect(controlFrame);

		boundary.left = min(boundary.left, controlFrame.left);
		boundary.top = min(boundary.top, controlFrame.top);
		boundary.right = max(boundary.right, controlFrame.right);
		boundary.bottom = max(boundary.bottom, controlFrame.bottom);

		pivot = basePoint + offset;
	}

	return boundary;
}



UINT Dialog::Standard::GetId(Json::Object& data)
{
	return data.GetInteger("id");
}



void Dialog::Standard::GetListItems(Json::Object& data, std::vector<CString>& list)
{
	Json::Value& value = data.GetValue("list");

	if (value.IsArray() == false) {
		return;
	}

	for (Json::Value* pValue : value.AsArray().GetBuffer()) {
		list.push_back(Facility::Local(pValue->AsString()));
	}
}



Component::EPivot Dialog::Standard::GetPivot(Json::Object& data)
{
	return (Component::EPivot)data.GetInteger("pivot");
}



CRect Dialog::Standard::GetRect(Json::Object& data)
{
	CRect rect;

	Json::Value* pValue = data.FindValue("rect");
	if (pValue != nullptr && pValue->IsArray()) {
		Json::Array& rectValue = pValue->AsArray();
		rect.left = rectValue[0]->ToInteger();
		rect.top = rectValue[1]->ToInteger();
		rect.right = rectValue[2]->ToInteger();
		rect.bottom = rectValue[3]->ToInteger();
	}

	return rect;
}



CString Dialog::Standard::GetTitle(Json::Object& data)
{
	return Facility::Local(data.GetString("title"));
}



Json::Object& Dialog::Standard::SetData(Json::Object& data, UINT id, const CString& title, Component::EPivot ePivot, const CRect& rect)
{
	if (data.FindValue("id") == nullptr && id > 0) {
		data.SetInteger("id", id);
	}
	if (data.FindValue("title") == nullptr && title.IsEmpty() == false) {
		data.SetString("title", title);
	}
	if (data.FindValue("pivot") == nullptr && ePivot != Component::EPivot::Unknown) {
		data.SetInteger("pivot", (int)ePivot);
	}
	if (data.FindValue("rect") == nullptr && (rect.Width() > 0 || rect.Height() > 0)) {
		Json::Array* pRect = new Json::Array();
		pRect->AddInteger(rect.left);
		pRect->AddInteger(rect.top);
		pRect->AddInteger(rect.right);
		pRect->AddInteger(rect.bottom);

		data.SetArray("rect", pRect);
	}

	return data;
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
