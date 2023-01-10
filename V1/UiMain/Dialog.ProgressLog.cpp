#include "stdafx.h"
#include "Dialog.ProgressLog.h"
#include "Component.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif



#define DDX_CONTROL(x) DDX_Control(pDX, (int)PRESET::x, m_wnd##x);

#define PRESET PresetProgressLog

namespace PresetProgressLog
{
	enum ControlId
	{
		Unknown = WM_USER,
		Indicator = IDC_DMI_CONTROL_01,
		Message = IDC_DMI_CONTROL_02,
		Log = IDC_DMI_CONTROL_03,
	};

	enum ListIndex
	{
		Status = 0,
		File,
	};

	const COLORREF ItemColors[] = {
		(COLORREF)Component::EColor::Black,
		(COLORREF)Component::EColor::Red,
	};

	int Gap()
	{
		return globalUtils.ScaleByDPI(8);
	}
}



using namespace Dialog;

IMPLEMENT_DYNAMIC(ProgressLog, Standard)

BEGIN_MESSAGE_MAP(ProgressLog, Standard)
	ON_WM_CREATE()
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
	case Signal::Progress::Action::StartMarquee: StartMarquee();      break;
	case Signal::Progress::Action::SetMessage:   SetMessage(data);    break;
	case Signal::Progress::Action::AddLog:       AddLog(data);        break;
	case Signal::Progress::Action::SetLogStatus: SetLogStatus(data);  break;
	case Signal::Progress::Action::StopMarquee:  StartMarquee(false); break;

	default:
		DEBUG_STOP;
		break;
	}

	REMOVE_POINTER(pData);
}



void Dialog::ProgressLog::DoDataExchange(CDataExchange* pDX)
{
	__super::DoDataExchange(pDX);

	DDX_CONTROL(Indicator);
	DDX_CONTROL(Message);
	DDX_CONTROL(Log);
}



BOOL Dialog::ProgressLog::OnInitDialog()
{
	__super::OnInitDialog();

	Json::Object& data = GetUiData().GetAt("size");

	CSize frame = GetFrameThickness();
	CSize winSize = globalUtils.ScaleByDPI(CSize(data.GetInteger("cx"), data.GetInteger("cy")));
	CRect body = { 0, frame.cy, winSize.cx, winSize.cy };

	ConstructBody(body);

	m_windowSize = AdjustWindowSize(winSize);
	SetSizeLimit(true, true);
	//:CHECK
	StartMarquee();

	return TRUE;
}



int Dialog::ProgressLog::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CBCGPDialog::OnCreate(lpCreateStruct) == -1) {
		DEBUG_STOP;
		return -1;
	}

	return 0;
}



void Dialog::ProgressLog::ConstructBody(const CRect& boundary)
{
	Json::Object& data = GetUiData().GetAt("body");

	int height = globalUtils.ScaleByDPI(24);
	CRect area = boundary;
	area.bottom = area.top + height;

	CRect result = SetupControl(m_wndIndicator, data.GetAt("Indicator"), Component::EPivot::TopLeft, area);

	area.left = result.right + PRESET::Gap();
	SetupControl(m_wndMessage, data.GetAt("Message"), Component::EPivot::MiddleLeft, area);

	area.left = boundary.left;
	area.top = area.bottom + GetFrameThickness().cy;
	area.bottom = boundary.bottom;
	SetupControl(m_wndLog, data.GetAt("Log"), Component::EPivot::TopLeft, area);
}



BOOL Dialog::ProgressLog::DestroyWindow()
{
	EnableParent();

	return CBCGPDialog::DestroyWindow();
}



void Dialog::ProgressLog::StartMarquee(bool start)
{
	CBCGPCircularProgressIndicatorImpl* pProgress = m_wndIndicator.GetCircularProgressIndicator();
	pProgress->StartMarquee(start);
}



void Dialog::ProgressLog::SetMessage(Json::Object& data)
{
	CString message = data.GetString(SKW_MESSAGE);
	m_wndMessage.SetWindowText(message);
}



void Dialog::ProgressLog::AddLog(Json::Object& data)
{
	int index = m_wndLog.AddString(data.GetString(SKW_TITLE));
	m_wndLog.SetItemDescription(index, data.GetString(SKW_DESCRIPTION));
	m_wndLog.SetItemToolTip(index, data.GetString(SKW_TOOLTIP));

	Signal::Progress::Status status = (Signal::Progress::Status)data.GetInteger(SKW_STATUS);
	if (status > Signal::Progress::Status::Succeed) {
		m_wndLog.SetItemColorBar(index, PRESET::ItemColors[(int)status]);
	}
}



void Dialog::ProgressLog::SetLogStatus(Json::Object& data)
{
	int status = data.GetInteger(SKW_STATUS);
	int index = m_wndLog.GetCount();
	m_wndLog.SetItemColorBar(index - 1, PRESET::ItemColors[status]);
}

#undef DDX_CONTROL
#undef PRESET
