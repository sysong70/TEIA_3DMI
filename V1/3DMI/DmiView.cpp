// RtView.cpp : implementation of the RtView class
//

#include "pch.h"
#include "DmiApp.h"

#include "DmiDoc.h"
#include "DmiView.h"

#include "DLL/DmiJsonCmd.h"

#include "HPSExchangeProgressDialog.h"

#include <chrono>
using namespace std::chrono;

#ifdef _DEBUG
	#define new DEBUG_NEW
#endif

// RtView

IMPLEMENT_DYNCREATE(DmiView, CView)

BEGIN_MESSAGE_MAP(DmiView, CView)
	ON_WM_CONTEXTMENU()
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, OnFilePrintPreview)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, CView::OnFilePrint)
	ON_MESSAGE(WM_PRINTCLIENT, OnPrintClient)
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_RBUTTONDOWN()
	ON_WM_RBUTTONUP()
	ON_WM_MBUTTONDOWN()
	ON_WM_MBUTTONUP()
	ON_WM_MOUSEWHEEL()
	ON_WM_TIMER()
END_MESSAGE_MAP()

DmiView::DmiView()
{
}

DmiView::~DmiView()
{
	DWORD_PTR nId = GetDocument()->GetId();
	DmiJsonCmd::ExecuteHpsCommand(L"DeleteHpsView", nId);
}

// 1.
BOOL DmiView::PreCreateWindow(CREATESTRUCT& cs)
{
	// Setup Window class to work with HPS rendering.  The REDRAW flags prevent
	// flickering when resizing, and OWNDC allocates a single device context to for this window.
	cs.lpszClass = AfxRegisterWndClass(CS_OWNDC | CS_DBLCLKS | CS_HREDRAW | CS_VREDRAW);

	return CView::PreCreateWindow(cs);
}

// 2.
void DmiView::OnInitialUpdate()
{
	CView::OnInitialUpdate();

	DWORD_PTR nId = GetDocument()->GetId();

	if(false == DmiJsonCmd::ExecuteHpsCommand(L"CreateHpsView", nId)) {
		ASSERT(false);
	}

	if (false == DmiJsonCmd::ExecuteHpsCommand(L"InitialUpdateHpsView", nId, (DWORD_PTR)m_hWnd)) {
		ASSERT(false);
	}

	DmiJsonCmd::ExecuteHpsCommand(L"UpdateHpsView", nId);

	return;

	if(false == m_strFilePathName.IsEmpty())
	{
		// ===== Modal Dialog =====
/*		
		DWORD_PTR nId = GetDocument()->GetId();
		HPSExchangeProgressDialog cDlg(nDocId, nId, m_strFilePathName);
		RtJsonCmd::ExcuteHpsCommand(L"FileOpen", nDocId, m_strFilePathName);
		m_strFilePathName.Empty();
		cDlg.DoModal();
*/

		// ===== Modeless Dialog =====
// 		m_pcExchangeProgressDialog = new HPSExchangeProgressDialog(nDocId, nId, m_strFilePathName);
// 		m_pcExchangeProgressDialog->Create(IDD_EXCHANGE_IMPORT_DIALOG);
// 		m_pcExchangeProgressDialog->CenterWindow();

		// #temp
		//RtJsonCmd::ExcuteHpsCommand(L"FileOpen", nDocId, m_strFilePathName);
		//RtJsonCmd::ExcuteHpsCommand(L"ZoomFit", nId);

		COleDateTime cFileImportEndTime = COleDateTime::GetCurrentTime();
		COleDateTimeSpan cTimeSpan = cFileImportEndTime - theApp.m_cFileImportStartTime;
		
		CString strTimeResult;

		if(0 < cTimeSpan.GetMinutes()) {
			strTimeResult = cTimeSpan.Format(L"%Mm %Ss");
		}
		else {
			strTimeResult = cTimeSpan.Format(L"%Ss");
		}

		GetDocument()->SetTimeTick(3, system_clock::now().time_since_epoch().count());
		GetDocument()->UpdatePropertyBarInformation();

		// #temp
		//m_strFilePathName.Empty();

//		m_pcExchangeProgressDialog->ShowWindow(SW_SHOW);
	}

	DmiJsonCmd::ExecuteHpsCommand(L"UpdateHpsView", nId);
}

// 3. 최초 Paint가 실행된 이후 File을 Import 하도록 한다.
void DmiView::OnPaint()
{
	// File을 Open하는 Flag
	if(true == m_bFileOpenFlag) {
		m_nUpdateCount++;
		TRACE(L"m_nUpdateCount: %d\n", m_nUpdateCount);

		int nDocCount = theApp.m_pDocManager->GetOpenDocumentCount();

		bool bOpenFlag = false;
		// Open Document의 갯수가 1인 경우 (처음으로 Tab이 붙는 경우)는 Update 2회를 한 후에 Open 하도록 한다.
		if(1 == nDocCount) {
			if(2 == m_nUpdateCount) {
				bOpenFlag = true;
			}
		}
		else {
			bOpenFlag = true;
		}

		if(true == bOpenFlag) {
			m_bFileOpenFlag = false;
			DWORD_PTR nId = GetDocument()->GetId();
			HPSExchangeProgressDialog cDLG(nId);
			cDLG.SetFilePathName(m_strFilePathName);

			cDLG.DoModal();

			GetDocument()->UpdatePropertyBarInformation();

			GetDocument()->CreateModelTreeInformation();
		}
	}

/*
	// C3D Import할 때 사용하는 함수
	if(true == m_bFirstPaintFlag) {
		m_bFirstPaintFlag = false;
		RtJsonCmd::Execute3dxCommand(L"FileImport", m_nId, m_strFilePathName);
		m_strFilePathName.Empty();
		return;
	}
*/

	// Update our HPS Canvas.  A refresh is needed as the window size may have changed.
	DWORD_PTR nId = GetDocument()->GetId();
	DmiJsonCmd::ExecuteHpsCommand(L"UpdateHpsView", nId);

	// Invoke BeginPaint/EndPaint.  This must be called when providing OnPaint handler.
	CPaintDC dc(this);
}

void DmiView::OnDraw(CDC * /*pDC*/)
{
	DmiDoc * pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	// TODO: add draw code for native data here
}

void DmiView::OnFilePrintPreview()
{
	BCGPPrintPreview (this);
}

BOOL DmiView::OnPreparePrinting(CPrintInfo* pInfo)
{
	return BCGPPreparePrinting(this, pInfo);
}

void DmiView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add extra initialization before printing
}

void DmiView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
}



// RtView message handlers

LRESULT DmiView::OnPrintClient(WPARAM wp, LPARAM lp)
{
	if ((lp & PRF_CLIENT) == PRF_CLIENT)
	{
		CDC* pDC = CDC::FromHandle((HDC)wp);
		ASSERT_VALID(pDC);

		OnDraw(pDC);
	}

	return 0;
}


void DmiView::OnContextMenu(CWnd*, CPoint point)
{
/*
	if (CBCGPPopupMenu::GetSafeActivePopupMenu() != NULL)
	{
		return;
	}

	theApp.ShowPopupMenu(IDR_CONTEXT_MENU, point, this);
*/
}

// == Mouse 관련 함수 ================================================================================
void DmiView::OnLButtonDown(UINT nFlags, CPoint point)
{
	SetCapture();

	DWORD_PTR nId = GetDocument()->GetId();
	DmiJsonCmd::ExecuteHpsCommand(L"LButtonDown", nId, nFlags, point.x, point.y);

	CView::OnLButtonDown(nFlags, point);
}


void DmiView::OnLButtonUp(UINT nFlags, CPoint point)
{
	DWORD_PTR nId = GetDocument()->GetId();
	DmiJsonCmd::ExecuteHpsCommand(L"LButtonUp", nId, nFlags, point.x, point.y);

	ReleaseCapture();

	SetCursor(theApp.LoadStandardCursor(IDC_ARROW));

	CView::OnLButtonUp(nFlags, point);
}

void DmiView::OnMButtonDown(UINT nFlags, CPoint point)
{
	SetCapture();

	DWORD_PTR nId = GetDocument()->GetId();
	DmiJsonCmd::ExecuteHpsCommand(L"MButtonDown", nId, nFlags, point.x, point.y);


	CView::OnRButtonDown(nFlags, point);
}


void DmiView::OnMButtonUp(UINT nFlags, CPoint point)
{
	DWORD_PTR nId = GetDocument()->GetId();
	DmiJsonCmd::ExecuteHpsCommand(L"MButtonUp", nId, nFlags, point.x, point.y);

	ReleaseCapture();

	SetCursor(theApp.LoadStandardCursor(IDC_ARROW));

	CView::OnRButtonUp(nFlags, point);
}


void DmiView::OnRButtonDown(UINT nFlags, CPoint point)
{
	SetCapture();

	DWORD_PTR nId = GetDocument()->GetId();
	DmiJsonCmd::ExecuteHpsCommand(L"RButtonDown", nId, nFlags, point.x, point.y);

	CView::OnRButtonDown(nFlags, point);
}


void DmiView::OnRButtonUp(UINT nFlags, CPoint point)
{
	DWORD_PTR nId = GetDocument()->GetId();
	DmiJsonCmd::ExecuteHpsCommand(L"RButtonUp", nId, nFlags, point.x, point.y);

	ReleaseCapture();

	SetCursor(theApp.LoadStandardCursor(IDC_ARROW));

	CView::OnRButtonUp(nFlags, point);
}

void DmiView::OnMouseMove(UINT nFlags, CPoint point)
{
	CRect cWndRect;
	GetWindowRect(&cWndRect);
	ScreenToClient(&cWndRect);

	if(cWndRect.PtInRect(point) || (nFlags & MK_LBUTTON) || (nFlags & MK_RBUTTON))
	{
		DWORD_PTR nId = GetDocument()->GetId();
		DmiJsonCmd::ExecuteHpsCommand(L"MouseMove", nId, nFlags, point.x, point.y);
	}
	else
	{
		if(!(nFlags & MK_LBUTTON) && GetCapture() != NULL)
			OnLButtonUp(nFlags, point);
		if(!(nFlags & MK_RBUTTON) && GetCapture() != NULL)
			OnRButtonUp(nFlags, point);
	}

	CView::OnMouseMove(nFlags, point);
}

BOOL DmiView::OnMouseWheel(UINT nFlags, short zDelta, CPoint point)
{
	::ScreenToClient(m_hWnd, &point);

	DWORD_PTR nId = GetDocument()->GetId();
	DmiJsonCmd::ExecuteHpsCommand(L"MouseWheel", nId, nFlags, zDelta, point.x, point.y);

	return CView::OnMouseWheel(nFlags, zDelta, point);
}


void DmiView::OnUpdate(CView * /*pSender*/, LPARAM /*lHint*/, CObject * /*pHint*/)
{
	int nInt = 0;
}


void DmiView::OnTimer(UINT_PTR nIDEvent)
{
	if(100 == nIDEvent) {
		KillTimer(nIDEvent);

		COleDateTime cFileImportEndTime = COleDateTime::GetCurrentTime();
		COleDateTimeSpan cTimeSpan = cFileImportEndTime - theApp.m_cFileImportStartTime;
		CString strTimeResult = cTimeSpan.Format(L"%M:%S");

		AfxMessageBox(strTimeResult);
	}
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

	CView::OnTimer(nIDEvent);
}

void DmiView::OnActivateView(BOOL bActivate, CView * pActivateView, CView * pDeactiveView)
{
	if(TRUE == bActivate) {
		GetDocument()->UpdatePropertyBarInformation();
	}

	CView::OnActivateView(bActivate, pActivateView, pDeactiveView);
}

// RtView diagnostics
#ifdef _DEBUG
void DmiView::AssertValid() const
{
	CView::AssertValid();
}

void DmiView::Dump(CDumpContext & dc) const
{
	CView::Dump(dc);
}

DmiDoc * DmiView::GetDocument() const // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(DmiDoc)));
	return (DmiDoc *) m_pDocument;
}
#endif //_DEBUG
