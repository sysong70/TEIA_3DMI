#include "stdafx.h"
#include "Dialog.DebugTracer.h"
#include "Facility.AppResources.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif



#define PRESET PresetDebugTracer

namespace PresetDebugTracer
{
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

IMPLEMENT_DYNAMIC(DebugTracer, CBCGPDialog)

BEGIN_MESSAGE_MAP(DebugTracer, CBCGPDialog)
END_MESSAGE_MAP()



Dialog::DebugTracer::DebugTracer(CWnd* pParent /*=nullptr*/)
	: Standard(IDD_DMI_DEBUG_TRACER, "DebugTracer", pParent)
{
}



Dialog::DebugTracer::~DebugTracer()
{
}



Signal::Target Dialog::DebugTracer::GetSignalTargetId()
{
	return Signal::Target::DebugTracer;
}



void Dialog::DebugTracer::ReceiveSignal(Json::Object* pData)
{
	Json::Object& data = *pData;

	Signal::Application::Action action = (Signal::Application::Action)data.GetInteger(SKW_ACTION);
	switch (action) {
	case Signal::Application::Action::AddTraceLog:		AddLog(data);	break;
	case Signal::Application::Action::ClearTraceLog:	ClearLog();		break;
	case Signal::Application::Action::SaveTraceLog:		SaveLog(data);	break;

	default:
		DEBUG_STOP;
	}
}



void Dialog::DebugTracer::DoDataExchange(CDataExchange* pDX)
{
	CBCGPDialog::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_DMI_CONTROL_01, m_wndLog);
}



BOOL Dialog::DebugTracer::OnInitDialog()
{
	CBCGPDialog::OnInitDialog();

	EnableVisualManagerStyle(TRUE, TRUE);
	EnableLayout();

	ConstructBody({});

	return TRUE;
}



void Dialog::DebugTracer::ConstructBody(const CRect& boundary)
{
	m_wndLog.m_bVisualManagerStyle = TRUE;
	m_wndLog.SetAlternateRowColor();
	m_wndLog.SetItemExtraHeight(PRESET::ExtraHeight());

	CBCGPStaticLayout* pLayout = (CBCGPStaticLayout*)GetLayout();
	DEBUG_VALID(pLayout);
	//:WARNING - create before using (why?)
	pLayout->Create(this);
	pLayout->AddAnchor(IDC_DMI_CONTROL_01, CBCGPStaticLayout::e_MoveTypeNone, CBCGPStaticLayout::e_SizeTypeBoth);
}



BOOL Dialog::DebugTracer::DestroyWindow()
{
	return CBCGPDialog::DestroyWindow();
}



void Dialog::DebugTracer::AddLog(Json::Object& data)
{
	m_wndLog.AddString(data.GetString(SKW_VALUE));
}



void Dialog::DebugTracer::ClearLog()
{
	m_wndLog.ResetContent();
}

#include "Fio.h"

void Dialog::DebugTracer::SaveLog(Json::Object& data)
{
	Json::Value* pValue = data.FindValue(SKW_VALUE);
	CString path;

	if (pValue != nullptr) {
		path = pValue->AsString();
	}
	else {
		CFileDialog dlg(FALSE, NULL, NULL, 0, NULL, this);
		if (dlg.DoModal() == IDOK) {
			path = dlg.GetPathName();
		}
		else {
			return;
		}
	}

	Fio::TextFile file;
	if (file.Open(path.GetBuffer(), Fio::EMode::Write, Fio::EEncoding::UTF16LE)) {
		for (int i = 0; i < m_wndLog.GetCount(); i++) {
			CString log;
			m_wndLog.GetText(i, log);
			file.WriteLine(log, log.GetLength());
		}

		file.Close();
	}
	else {
		DEBUG_STOP;
	}

	if (data.GetBoolean(SKW_CLEAR)) {
		ClearLog();
	}
}

#undef PRESET
