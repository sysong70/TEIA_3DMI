#include "stdafx.h"

#include "Ctl.h"
#include "Dlg.ProgressBar.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//**************************************************************************************************

namespace
{
	enum class Id
	{
		Unknown = WM_USER,
		Message = IDC_DMI_CONTROL_01,
		Progress = IDC_DMI_CONTROL_02,
		Log = IDC_DMI_CONTROL_03,
	};

	const COLORREF ItemColors[] = {
		(COLORREF)Ctl::EColor::Gray,
		(COLORREF)Ctl::EColor::Green,
		(COLORREF)Ctl::EColor::Yellow,
		(COLORREF)Ctl::EColor::Red,
	};
}

//**************************************************************************************************

IMPLEMENT_DYNAMIC(DlgProgressBar, DlgStandard)

BEGIN_MESSAGE_MAP(DlgProgressBar, DlgStandard)
END_MESSAGE_MAP()

//--------------------------------------------------------------------------------------------------

#include <Signal2d.h>

void DlgProgressBar::ReceiveSignal2d(Json::Object* pData)
{
	Json::Object& data = *pData;

	SgnMainFrame::Action action = (SgnMainFrame::Action)data.GetInteger(SKW_ACTION);
	switch (action) {
	case SgnMainFrame::Action::AddProgressLog:      AddLog(data);        break;
	case SgnMainFrame::Action::ClearProgressLog:    ClearLog();          break;
	case SgnMainFrame::Action::SetProgressMessage:  SetMessage(data);    break;
	case SgnMainFrame::Action::SetProgressPosition: SetPosition(data);   break;
	case SgnMainFrame::Action::SetProgressRange:    SetRange(data);      break;
	case SgnMainFrame::Action::SetProgressStatus:	SetLogStatus(data);  break;

	default:
		DEBUG_STOP;
		break;
	}

	REMOVE_POINTER(pData);
}

#include <Signal3d.h>

void DlgProgressBar::ReceiveSignal3d(Json::Object* pData)
{
	Json::Object& data = *pData;

	Signal::Progress::Action action = (Signal::Progress::Action)data.GetInteger(SKW_ACTION);
	switch (action) {
	case Signal::Progress::Action::SetRange:     SetRange(data);      break;
	case Signal::Progress::Action::SetPosition:  SetPosition(data);   break;
	case Signal::Progress::Action::SetMessage:   SetMessage(data);    break;
	case Signal::Progress::Action::AddLog:       AddLog(data);        break;
	case Signal::Progress::Action::SetLogStatus: SetLogStatus(data);  break;
	case Signal::Progress::Action::ClearLog:     ClearLog();          break;

	default:
		DEBUG_STOP;
		break;
	}

	REMOVE_POINTER(pData);
}



BOOL DlgProgressBar::DestroyWindow()
{
	EnableParent();

	return CBCGPDialog::DestroyWindow();
}



void DlgProgressBar::DoDataExchange(CDataExchange* pDX)
{
#define DDX_CONTROL(x) DDX_Control(pDX, (int)Id::x, x##Ctl);

	CBCGPDialog::DoDataExchange(pDX);

	DDX_CONTROL(Progress);
	DDX_CONTROL(Message);
	DDX_CONTROL(Log);

#undef DDX_CONTROL
}



BOOL DlgProgressBar::OnInitDialog()
{
	__super::OnInitDialog();

	CSize frame = GetFrameThickness();
	CSize size = GetWinSize();
	CRect body = { 0, frame.cy, size.cx, size.cy };

	ConstructBody(body);

	WindowSize = AdjustWindowSize(size);
	SetSizeLimit(true, true);

	return TRUE;
}



void DlgProgressBar::ConstructBody(const CRect& boundary)
{
	Json::Object& data = GetUiData().GetAt("body");

	CRect area = boundary;

	const int messageHeight = globalUtils.ScaleByDPI(24);
	const int progressHeight = globalUtils.ScaleByDPI(8);
	const int extraHeight = globalUtils.ScaleByDPI(2);
	const int gap = globalUtils.ScaleByDPI(8);

	// Message

	area.bottom = area.top + messageHeight;
	CRect result = Ctl::Setup(MessageCtl, data.GetAt("Message"), Ctl::EPivot::TopLeft, area);

	// Progress
	/*
	area.top = result.bottom + gap;
	area.bottom = area.top + progressHeight;
	Ctl::SetSize(&m_wndProgress, { area.Size().cx, progressHeight });
	result = Ctl::AdjustLayout(&m_wndProgress, area, area.Size(), Control::EPivot::TopLeft);

	m_wndProgress.m_bSetPosSmoothAnimation = TRUE;
	m_wndProgress.m_bSetPosLighting = TRUE;
	m_wndProgress.m_bDrawFrame = TRUE;
	*/
	ProgressCtl.ShowWindow(SW_HIDE);

	// Log

	area.top = result.bottom + gap;
	area.bottom = boundary.bottom;

	LogCtl.ModifyStyle(0, LBS_NOSEL);
	LogCtl.SetAlternateRowColor();
	LogCtl.SetItemExtraHeight(extraHeight);

	Ctl::AdjustLayout(&LogCtl, area, area.Size(), Ctl::EPivot::TopLeft);
}



void DlgProgressBar::SetRange(Json::Object& data)
{
	ProgressCtl.SetRange32(data.GetInteger(SKW_MIN), data.GetInteger(SKW_MAX));
	ProgressCtl.UpdateWindow();
}



void DlgProgressBar::SetPosition(Json::Object& data)
{
	ProgressCtl.SetPos(data.GetInteger(SKW_POSITION));
	ProgressCtl.UpdateWindow();
}



void DlgProgressBar::SetMessage(Json::Object& data)
{
	CString message = data.GetString(SKW_MESSAGE);
	MessageCtl.SetWindowText(message);

	RedrawWindow();
}



void DlgProgressBar::AddLog(Json::Object& data)
{
	int index = LogCtl.AddString(data.GetString(SKW_TITLE));
	LogCtl.SetItemDescription(index, data.GetString(SKW_DESCRIPTION));
	LogCtl.SetItemToolTip(index, data.GetString(SKW_TOOLTIP));

	Signal::Progress::Status status = (Signal::Progress::Status)data.GetInteger(SKW_STATUS);
	LogCtl.SetItemColorBar(index, ItemColors[(int)status]);

	RedrawWindow();
}



void DlgProgressBar::SetLogStatus(Json::Object& data)
{
	int status = data.GetInteger(SKW_STATUS);
	int index = LogCtl.GetCount();
	LogCtl.SetItemColorBar(index - 1, ItemColors[status]);

	RedrawWindow();
}



void DlgProgressBar::ClearLog()
{
	LogCtl.ResetContent();

	RedrawWindow();
}
