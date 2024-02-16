#include "stdafx.h"
#include "Dialog.DebugTracer.h"
#include "Facility.AppResources.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//**************************************************************************************************

#define PRESET PresetDebugTracer

namespace PresetDebugTracer
{
	enum EControlId
	{
		Id = WM_USER + 1,
		Clear,
		Save,
	};



	int ExtraHeight()
	{
		return globalUtils.ScaleByDPI(2);
	}



	int Gap()
	{
		return globalUtils.ScaleByDPI(8);
	}
}

//**************************************************************************************************

using namespace Dialog;

IMPLEMENT_DYNAMIC(DebugTracer, CBCGPDialog)

BEGIN_MESSAGE_MAP(DebugTracer, CBCGPDialog)
	ON_WM_CONTEXTMENU()
	//ON_COMMAND_RANGE(PRESET::Clear, PRESET::Save, OnContextCommand)
END_MESSAGE_MAP()



Dialog::DebugTracer::DebugTracer(CWnd* pParent /*=nullptr*/)
	: Standard(IDD_DMI_DEBUG_TRACER, "DebugTracer", pParent)
{
}



Dialog::DebugTracer::~DebugTracer()
{
	DestroyWindow();
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

#include "Window.Application.h"

void Dialog::DebugTracer::OnContextMenu(CWnd* pWnd, CPoint point)
{
	CMenu menu;
	menu.CreatePopupMenu();
	menu.AppendMenu(MF_STRING, PRESET::Clear, L"Clear");
	menu.AppendMenu(MF_STRING, PRESET::Save, L"Save...");
	//menu.TrackPopupMenu(TPM_LEFTALIGN, pos.x, pos.y, this);

	//:CHECK - visual theme
	UINT id = TheApplication.GetContextMenuManager()->TrackPopupMenu(menu.Detach(), point.x, point.y, this);
	OnContextCommand(id);
}



void Dialog::DebugTracer::OnContextCommand(UINT id)
{
	switch (id) {
	case PRESET::Clear:	ClearLog(); break;
	case PRESET::Save:	SaveLog(L"", true); break;

	default:
		DEBUG_STOP;
		break;
	}
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



void Dialog::DebugTracer::AddLog(Json::Object& data)
{
	AddLog(data.GetString(SKW_VALUE));
}

void Dialog::DebugTracer::AddLog(const wchar_t* pFormat, ...)
{
	CString stream;
	va_list argList;

	va_start(argList, pFormat);
	stream.FormatV(pFormat, argList);
	va_end(argList);

	AddLog(stream);
}

void Dialog::DebugTracer::AddLog(CString log)
{
	m_wndLog.AddString(log);
}



void Dialog::DebugTracer::ClearLog()
{
	m_wndLog.ResetContent();
}



void Dialog::DebugTracer::SaveLog(Json::Object& data)
{
	SaveLog(data.GetString(SKW_VALUE), data.GetBoolean(SKW_CLEAR));
}

void Dialog::DebugTracer::SaveLog(CString path, bool clear)
{
	if (path.IsEmpty()) {
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

	if (clear) {
		ClearLog();
	}
}

#undef PRESET
