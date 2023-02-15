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

	int ExtraHeight() {
		return globalUtils.ScaleByDPI(6);
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

	CSize frame = GetFrameThickness();
	CSize size = GetWinSize();
	CRect body = { 0, frame.cy, size.cx, size.cy };

	ConstructBody(body);

	m_windowSize = AdjustWindowSize(size);
	SetSizeLimit(true, true);
	//:CHECK
	StartMarquee();

	return TRUE;
}



void Dialog::ProgressLog::ConstructBody(const CRect& boundary)
{
	Json::Object& data = GetUiData().GetAt("body");

	int height = globalUtils.ScaleByDPI(24);
	CRect area = boundary;
	area.bottom = area.top + height;

	// Indicator

	CSize size = Facility::GetSize(data.GetAt("Indicator"));
	CRect result = AdjustLayout(&m_wndIndicator, area, globalUtils.ScaleByDPI(size), Component::EPivot::TopLeft);
	
	CBCGPCircularProgressIndicatorImpl* pProgress = m_wndIndicator.GetCircularProgressIndicator();
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
	
	m_wndIndicator.ShowWindow(SW_SHOWNOACTIVATE);
	
	// Message

	area.left = result.right + PRESET::Gap();
	SetupControl(m_wndMessage, data.GetAt("Message"), Component::EPivot::MiddleLeft, area);

	// Log

	area.left = boundary.left;
	area.top = area.bottom + GetFrameThickness().cy;
	area.bottom = boundary.bottom;

	//m_wndLog.EnableItemDescription(TRUE, 1);
	m_wndLog.ModifyStyle(0, LBS_NOSEL);
	m_wndLog.SetAlternateRowColor();
	m_wndLog.SetItemExtraHeight(PRESET::ExtraHeight());

	AdjustLayout(&m_wndLog, area, area.Size(), Component::EPivot::TopLeft);
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

	RedrawWindow();
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

	RedrawWindow();
}



void Dialog::ProgressLog::SetLogStatus(Json::Object& data)
{
	int status = data.GetInteger(SKW_STATUS);
	int index = m_wndLog.GetCount();
	m_wndLog.SetItemColorBar(index - 1, PRESET::ItemColors[status]);

	RedrawWindow();
}

#undef DDX_CONTROL
#undef PRESET
