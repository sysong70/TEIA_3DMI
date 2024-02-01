#include "stdafx.h"
#include "Dialog.ProgressLog.h"
#include "Control.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif



#define DDX_CONTROL(x) DDX_Control(pDX, (int)PRESET::x, m_wnd##x);

#define PRESET PresetProgressLog

namespace PresetProgressLog
{
	enum EControlId
	{
		Unknown = WM_USER,
		Message = IDC_DMI_CONTROL_01,
		Progress = IDC_DMI_CONTROL_02,
		Log = IDC_DMI_CONTROL_03,
	};

	enum EListIndex
	{
		Status = 0,
		File,
	};

	const COLORREF ItemColors[] = {
		(COLORREF)Control::EColor::Gray,
		(COLORREF)Control::EColor::Green,
		(COLORREF)Control::EColor::Yellow,
		(COLORREF)Control::EColor::Red,
	};

	int MessageHeight()
	{
		return globalUtils.ScaleByDPI(24);
	}

	int ProgressHeight()
	{
		return globalUtils.ScaleByDPI(8);
	}

	int ExtraHeight()
	{
		return globalUtils.ScaleByDPI(2);
	}

	int Gap()
	{
		return globalUtils.ScaleByDPI(8);
	}
}



using namespace Dialog;

IMPLEMENT_DYNAMIC(ProgressLog, Standard)

BEGIN_MESSAGE_MAP(ProgressLog, Standard)
END_MESSAGE_MAP()



Dialog::ProgressLog::ProgressLog(CWnd* pParent /*=nullptr*/)
	: Standard(IDD_DMI_PROGRESS_LOG, "Progress", pParent)
{
}



Dialog::ProgressLog::~ProgressLog()
{
}



Signal::Target Dialog::ProgressLog::GetSignalTargetId()
{
	return Signal::Target::Progress;
}



void Dialog::ProgressLog::ReceiveSignal(Json::Object* pData)
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



void Dialog::ProgressLog::DoDataExchange(CDataExchange* pDX)
{
	CBCGPDialog::DoDataExchange(pDX);

	DDX_CONTROL(Progress);
	DDX_CONTROL(Message);
	DDX_CONTROL(Log);
}



BOOL Dialog::ProgressLog::OnInitDialog()
{
	__super::OnInitDialog();

	CSize frame = GetFrameThickness();
	CSize size = GetWinSize();
	CRect body = { 0, frame.cy, size.cx, size.cy };

	ConstructBody(body);

	m_windowSize = AdjustWindowSize(size);
	SetSizeLimit(true, true);

	return TRUE;
}



void Dialog::ProgressLog::ConstructBody(const CRect& boundary)
{
	Json::Object& data = GetUiData().GetAt("body");

	CRect area = boundary;

	// Message

	area.bottom = area.top + PRESET::MessageHeight();
	CRect result = Control::Setup(m_wndMessage, data.GetAt("Message"), Control::EPivot::TopLeft, area);

	// Progress

	area.top = result.bottom + PRESET::Gap();
	area.bottom = area.top + PRESET::ProgressHeight();
	Control::SetSize(&m_wndProgress, { area.Size().cx, PRESET::ProgressHeight() });
	result = Control::AdjustLayout(&m_wndProgress, area, area.Size(), Control::EPivot::TopLeft);

	m_wndProgress.m_bSetPosSmoothAnimation = TRUE;
	m_wndProgress.m_bSetPosLighting = TRUE;
	m_wndProgress.m_bDrawFrame = TRUE;

	// Log

	area.top = result.bottom + PRESET::Gap();
	area.bottom = boundary.bottom;

	m_wndLog.ModifyStyle(0, LBS_NOSEL);
	m_wndLog.SetAlternateRowColor();
	m_wndLog.SetItemExtraHeight(PRESET::ExtraHeight());

	Control::AdjustLayout(&m_wndLog, area, area.Size(), Control::EPivot::TopLeft);
}



BOOL Dialog::ProgressLog::DestroyWindow()
{
	EnableParent();

	return CBCGPDialog::DestroyWindow();
}



void Dialog::ProgressLog::SetRange(Json::Object& data)
{
	m_wndProgress.SetRange32(data.GetInteger(SKW_MIN), data.GetInteger(SKW_MAX));
}



void Dialog::ProgressLog::SetPosition(Json::Object& data)
{
	m_wndProgress.SetPos(data.GetInteger(SKW_POSITION));
}



void Dialog::ProgressLog::SetMessage(Json::Object& data)
{
	CString message = data.GetString(SKW_MESSAGE);
	m_wndMessage.SetWindowText(message);

	RedrawWindow();
}



void Dialog::ProgressLog::AddLog(Json::Object& data)
{
	int index = m_wndLog.AddString(data.GetString(SKW_TITLE));
	m_wndLog.SetItemDescription(index, data.GetString(SKW_DESCRIPTION));
	m_wndLog.SetItemToolTip(index, data.GetString(SKW_TOOLTIP));

	Signal::Progress::Status status = (Signal::Progress::Status)data.GetInteger(SKW_STATUS);
	m_wndLog.SetItemColorBar(index, PRESET::ItemColors[(int)status]);

	RedrawWindow();
}



void Dialog::ProgressLog::SetLogStatus(Json::Object& data)
{
	int status = data.GetInteger(SKW_STATUS);
	int index = m_wndLog.GetCount();
	m_wndLog.SetItemColorBar(index - 1, PRESET::ItemColors[status]);

	RedrawWindow();
}



void Dialog::ProgressLog::ClearLog()
{
	m_wndLog.ResetContent();

	RedrawWindow();
}

#undef DDX_CONTROL
#undef PRESET
