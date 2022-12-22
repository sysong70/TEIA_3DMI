#include "stdafx.h"
#include "Dialog.Standard.h"
#include "Facility.AppResources.h"

#ifdef _DEBUG
#define new DEBUG_NEW
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



CRect Dialog::Standard::SetupControl(CBCGPCircularProgressIndicatorCtrl& control, Json::Object& data, Component::EPivot ePivot, CRect rect)
{
	CSize size(data.GetInteger("cx"), data.GetInteger("cy"));
	CRect result = AdjustLayout(&control, globalUtils.ScaleByDPI(size), ePivot, rect);

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
	//control.SetItemExtraHeight(globalUtils.ScaleByDPI(4));
	control.EnableItemDescription(TRUE, 1);
	control.SetAlternateRowColor();

	return AdjustLayout(&control, CSize(rect.Width(), rect.Height()), ePivot, rect);
}

/*
	"Log":{"columns":[
		{"header":"Status|상태", "width":0.15},
		{"header":"File|파일", "width":0.85}
	]}
*/

CRect Dialog::Standard::SetupControl(CBCGPListCtrl& control, Json::Object& data, Component::EPivot ePivot, CRect rect)
{
	CRect result = AdjustLayout(&control, CSize(rect.Width(), rect.Height()), ePivot, rect);
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
	control.SetWindowText(Facility::Local(data.GetString("title")));
	control.SizeToContent();

	return AdjustLayout(&control, CSize(rect.Width(), 0), ePivot, rect);
}



CRect Dialog::Standard::AdjustLayout(CWnd* pControl, CSize maxSize, Component::EPivot ePivot, CRect boundary)
{
	CSize size = GetControlSize(pControl);
	size.cx = max(size.cx, maxSize.cx);
	size.cy = max(size.cy, maxSize.cy);

	CPoint pivot;

	switch (ePivot) {
	case Component::EPivot::TopLeft:
		pivot.x = boundary.left;
		pivot.y = boundary.top;
		break;

	case Component::EPivot::TopCenter:
		pivot.x = boundary.CenterPoint().x - (size.cx / 2);
		pivot.y = boundary.top;
		break;

	case Component::EPivot::TopRight:
		pivot.x = boundary.right - size.cx;
		pivot.y = boundary.top;
		break;

	case Component::EPivot::MiddleLeft:
		pivot.x = boundary.left;
		pivot.y = boundary.CenterPoint().y - (size.cy / 2);
		break;

	case Component::EPivot::MiddleCenter:
		pivot.x = boundary.CenterPoint().x - (size.cx / 2);
		pivot.y = boundary.CenterPoint().y - (size.cy / 2);
		break;

	case Component::EPivot::MiddleRight:
		pivot.x = boundary.right - size.cx;
		pivot.y = boundary.CenterPoint().y - (size.cy / 2);
		break;

	case Component::EPivot::BottomLeft:
		pivot.x = boundary.left;
		pivot.y = boundary.bottom - size.cy;
		break;

	case Component::EPivot::BottomCenter:
		pivot.x = boundary.CenterPoint().x - (size.cx / 2);
		pivot.y = boundary.bottom - size.cy;
		break;

	case Component::EPivot::BottomRight:
		pivot.x = boundary.right - size.cx;
		pivot.y = boundary.bottom - size.cy;
		break;

	default:
		DEBUG_STOP;
		break;
	}

	// move and resize
	pControl->SetWindowPos(nullptr, pivot.x, pivot.y, size.cx, size.cy, 0);

	return { pivot, size };
}



CSize Dialog::Standard::AdjustSize(CWnd* pControl, CSize size)
{
	CSize result = GetControlSize(pControl);
	result.cx = max(result.cx, size.cx);
	result.cy = max(result.cy, size.cy);

	pControl->SetWindowPos(nullptr, 0, 0, result.cx, result.cy, SWP_NOMOVE);

	return result;
}



CRect Dialog::Standard::AlignControl(CWnd* pControl, Component::EPivot ePivot, CRect boundary)
{
	CRect controlBoundary = GetControlRect(pControl);
	CSize size = controlBoundary.Size();
	CPoint pivot;

	switch (ePivot) {
	case Component::EPivot::TopLeft:
		pivot.x = boundary.left;
		pivot.y = boundary.top;
		break;

	case Component::EPivot::TopCenter:
		pivot.x = boundary.CenterPoint().x - (size.cx / 2);
		pivot.y = boundary.top;
		break;

	case Component::EPivot::TopRight:
		pivot.x = boundary.right - size.cx;
		pivot.y = boundary.top;
		break;

	case Component::EPivot::MiddleLeft:
		pivot.x = boundary.left;
		pivot.y = boundary.CenterPoint().y - (size.cy / 2);
		break;

	case Component::EPivot::MiddleCenter:
		pivot.x = boundary.CenterPoint().x - (size.cx / 2);
		pivot.y = boundary.CenterPoint().y - (size.cy / 2);
		break;

	case Component::EPivot::MiddleRight:
		pivot.x = boundary.right - size.cx;
		pivot.y = boundary.CenterPoint().y - (size.cy / 2);
		break;

	case Component::EPivot::BottomLeft:
		pivot.x = boundary.left;
		pivot.y = boundary.bottom - size.cy;
		break;

	case Component::EPivot::BottomCenter:
		pivot.x = boundary.CenterPoint().x - (size.cx / 2);
		pivot.y = boundary.bottom - size.cy;
		break;

	case Component::EPivot::BottomRight:
		pivot.x = boundary.right - size.cx;
		pivot.y = boundary.bottom - size.cy;
		break;

	default:
		DEBUG_STOP;
		break;
	}

	pControl->SetWindowPos(nullptr, pivot.x, pivot.y, 0, 0, SWP_NOSIZE);

	return { pivot, size };
}



CRect Dialog::Standard::AlignControls(Controls controls, Component::EAlign eAlign, CPoint startPoint)
{
	CRect totalBound;
	CRect controlBound;
	CSize controlSize;

	for (auto pControl : controls) {
		pControl->GetClientRect(controlBound);
		controlSize = controlBound.Size();
		// set previous value first
		CPoint pivot = controlBound.TopLeft();

		switch (eAlign) {
		case Component::EAlign::HorizontalLeft:
			pivot.x = startPoint.x;
			break;

		case Component::EAlign::HorizontalCenter:
			pivot.x = startPoint.x - (controlSize.cx / 2);
			break;

		case Component::EAlign::HorizontalRight:
			pivot.x = startPoint.x - controlSize.cx;
			break;

		case Component::EAlign::VerticalTop:
			pivot.y = startPoint.y;
			break;

		case Component::EAlign::VerticalCenter:
			pivot.y = startPoint.y + (controlSize.cy / 2);
			break;

		case Component::EAlign::VerticalBottom:
			pivot.y = startPoint.y + controlSize.cy;
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



CRect Dialog::Standard::DistributeControls(Controls controls, Component::EAlign eBase, CPoint startPoint, int gap)
{
	CRect totalBound;
	CRect controlBound;
	CPoint pivot = startPoint;

	for (auto pControl : controls) {
		pControl->GetClientRect(controlBound);
		CSize controlSize = controlBound.Size();
		CPoint offset;

		switch (eBase) {
		case Component::EAlign::HorizontalLeft:
			pivot.y = controlBound.top;
			offset.x = gap;
			break;

		case Component::EAlign::HorizontalCenter:
			pivot.x -= controlSize.cx / 2;
			pivot.y = controlBound.top;
			offset.x = gap;
			break;

		case Component::EAlign::HorizontalRight:
			pivot.x -= controlSize.cx;
			pivot.y = controlBound.top;
			offset.x = gap;
			break;

		case Component::EAlign::VerticalTop:
			pivot.x = controlBound.left;
			offset.y = gap;
			break;

		case Component::EAlign::VerticalCenter:
			pivot.x = controlBound.left;
			pivot.y -= controlSize.cy / 2;
			offset.y = gap;
			break;

		case Component::EAlign::VerticalBottom:
			pivot.x = controlBound.left;
			pivot.y -= controlSize.cy;
			offset.y = gap;
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

		pivot += offset;
	}

	return totalBound;
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



CRect Dialog::Standard::GetClientArea()
{
	CRect rect;
	GetClientRect(&rect);

	return rect;
}



CRect Dialog::Standard::GetControlRect(CWnd* pControl)
{
	DEBUG_VALID(pControl);

	CRect rect;
	pControl->GetWindowRect(&rect);
	ScreenToClient(rect);

	return rect;
}



CSize Dialog::Standard::GetControlSize(CWnd* pControl)
{
	DEBUG_VALID(pControl);

	CRect rect;
	pControl->GetClientRect(&rect);

	return rect.Size();
}
