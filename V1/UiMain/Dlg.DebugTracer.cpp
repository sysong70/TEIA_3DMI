#include "stdafx.h"

#include "Dlg.DebugTracer.h"
#include "Wnd.Application.h"

#include <Signal2d.h>

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
		This = WM_USER + 1,
		Clear,
		Save,
	};
}

//**************************************************************************************************

IMPLEMENT_DYNAMIC(DlgDebugTracer, CBCGPDialog)

BEGIN_MESSAGE_MAP(DlgDebugTracer, CBCGPDialog)
	ON_WM_CONTEXTMENU()
END_MESSAGE_MAP()



void DlgDebugTracer::ReceiveSignal2d(Json::Object* pData)
{
	Json::Object& data = *pData;

	SgnMainFrame::Action action = (SgnMainFrame::Action)data.GetInteger(SKW_ACTION);
	switch (action) {
	case SgnMainFrame::Action::AddTraceLog:		AddLog(data);	break;
	case SgnMainFrame::Action::ClearTraceLog:	ClearLog();		break;
	case SgnMainFrame::Action::SaveTraceLog:	SaveLog(data);	break;

	default:
		DEBUG_STOP;
	}
}



void DlgDebugTracer::DoDataExchange(CDataExchange* pDX)
{
	CBCGPDialog::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_DMI_CONTROL_01, LogCtl);
}



BOOL DlgDebugTracer::OnInitDialog()
{
	CBCGPDialog::OnInitDialog();

	EnableVisualManagerStyle(TRUE, TRUE);
	EnableLayout();

	ConstructBody({});

	return TRUE;
}



void DlgDebugTracer::OnContextMenu(CWnd* pWnd, CPoint point)
{
	CMenu menu;
	menu.CreatePopupMenu();
	menu.AppendMenu(MF_STRING, (UINT)Id::Clear, L"Clear");
	menu.AppendMenu(MF_STRING, (UINT)Id::Save, L"Save...");

	// CHECK - visual theme
	UINT id = TheApp.GetContextMenuManager()->TrackPopupMenu(menu.Detach(), point.x, point.y, this);
	OnContextCommand(id);
}



void DlgDebugTracer::OnContextCommand(UINT id)
{
	switch (id) {
	case (UINT)Id::Clear:	ClearLog(); break;
	case (UINT)Id::Save:	SaveLog(L"", true); break;

	default:
		DEBUG_STOP;
		break;
	}
}



void DlgDebugTracer::ConstructBody(const CRect& boundary)
{
	const int extraHeight = globalUtils.ScaleByDPI(2);

	LogCtl.m_bVisualManagerStyle = TRUE;
	LogCtl.SetAlternateRowColor();
	LogCtl.SetItemExtraHeight(extraHeight);

	CBCGPStaticLayout* pLayout = (CBCGPStaticLayout*)GetLayout();
	DEBUG_VALID(pLayout);
	// WARNING - create before using (why?)
	pLayout->Create(this);
	pLayout->AddAnchor(IDC_DMI_CONTROL_01, CBCGPStaticLayout::e_MoveTypeNone, CBCGPStaticLayout::e_SizeTypeBoth);
}



void DlgDebugTracer::AddLog(Json::Object& data)
{
	AddLog(data.GetString(SKW_VALUE));
}

void DlgDebugTracer::AddLog(const wchar_t* pFormat, ...)
{
	CString stream;
	va_list argList;

	va_start(argList, pFormat);
	stream.FormatV(pFormat, argList);
	va_end(argList);

	AddLog(stream);
}

void DlgDebugTracer::AddLog(CString log)
{
	LogCtl.AddString(log);
}



void DlgDebugTracer::ClearLog()
{
	LogCtl.ResetContent();
}



void DlgDebugTracer::SaveLog(Json::Object& data)
{
	SaveLog(data.GetString(SKW_VALUE), data.GetBoolean(SKW_CLEAR));
}

void DlgDebugTracer::SaveLog(CString path, bool clear)
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
		for (int i = 0; i < LogCtl.GetCount(); i++) {
			CString log;
			LogCtl.GetText(i, log);
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
