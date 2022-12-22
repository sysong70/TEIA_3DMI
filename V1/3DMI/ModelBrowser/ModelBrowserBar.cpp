// workspace.cpp : implementation of the CWorkSpaceBar class
//

#include "pch.h"
#include "../DmiApp.h"
#include "ModelBrowserBar.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CWorkSpaceBar

BEGIN_MESSAGE_MAP(ModelBrowserBar, CBCGPDockingControlBar)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_PAINT()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWorkSpaceBar construction/destruction

ModelBrowserBar::ModelBrowserBar()
{
	// TODO: add one-time construction code here

}

ModelBrowserBar::~ModelBrowserBar()
{
}

/////////////////////////////////////////////////////////////////////////////
// CWorkSpaceBar message handlers

int ModelBrowserBar::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CBCGPDockingControlBar::OnCreate(lpCreateStruct) == -1)
		return -1;

	CRect cRectDummy;
	cRectDummy.SetRectEmpty();

	m_cImageList.Create(16, 16, ILC_COLOR32, 0, 1);

	for(int nIndex = IDB_MODEL_BROWSER_BEGIN; nIndex <= IDB_MODEL_BROWSER_END; ++nIndex)
	{
		m_anImageBaseIndicesArray.push_back(m_cImageList.GetImageCount());
		CBitmap bitmap;
		bitmap.LoadBitmapW(nIndex);
		m_cImageList.Add(&bitmap, RGB(0, 255, 0));
	}

	// Create tree control:
	const DWORD dwViewStyle =	WS_CHILD | WS_VISIBLE | TVS_HASLINES | TVS_LINESATROOT | TVS_HASBUTTONS;

	m_cEmptyTree.m_bVisualManagerStyle = TRUE;

	if (!m_cEmptyTree.Create(dwViewStyle, cRectDummy, this, m_nTreeId++))
	{
		TRACE0("Failed to create workspace view\n");
		return -1;      // fail to create
	}

	HTREEITEM hRoot = m_cEmptyTree.InsertItem(_T("No Model"));

	m_pwndCurrentTreeCtrl = &m_cEmptyTree;

	return 0;
}

//== Model Tree Function ===========================================================================

// 1. Model Tree를 생성 (주어진 Id에 대응하도록 생성한다.)
ModelTreeCtrl * ModelBrowserBar::CreateModelTreeCtrl(DWORD_PTR nId)
{
	ModelTreeCtrl * pcTreeCtrl = new ModelTreeCtrl(nId);

	CRect cRectDummy;
	cRectDummy.SetRectEmpty();

	// Create tree control:
	const DWORD dwViewStyle = WS_CHILD | WS_VISIBLE | TVS_HASLINES | TVS_LINESATROOT | TVS_HASBUTTONS;

	pcTreeCtrl->m_bVisualManagerStyle = TRUE;

	if(FALSE == pcTreeCtrl->Create(dwViewStyle, cRectDummy, this, m_nTreeId++)) {
		return nullptr;
	}

	pcTreeCtrl->SetImageList(&m_cImageList, TVSIL_NORMAL);

	return pcTreeCtrl;
}

void ModelBrowserBar::OnSize(UINT nType, int cx, int cy)
{
	CBCGPDockingControlBar::OnSize(nType, cx, cy);

	// Tree control should cover a whole client area:
	//m_cEmptyTree.SetWindowPos(NULL, 0, 0, cx, cy, SWP_NOACTIVATE | SWP_NOZORDER);
	m_pwndCurrentTreeCtrl->SetWindowPos(NULL, 0, 0, cx, cy, SWP_NOACTIVATE | SWP_NOZORDER);
}

void ModelBrowserBar::Init(DWORD_PTR nDocId, ModelTreeCtrl * pcModelTree)
{
	m_nDocId = nDocId;

	BOOL bStatus = FALSE;

	if(pcModelTree != m_pwndCurrentTreeCtrl) {
		if(nullptr != m_pwndCurrentTreeCtrl) {
			bStatus = m_pwndCurrentTreeCtrl->ShowWindow(SW_HIDE);
		}
	}
	else {
		return;
	}
	
	m_pwndCurrentTreeCtrl = pcModelTree;
	bStatus = m_pwndCurrentTreeCtrl->ShowWindow(SW_SHOWNORMAL);

	CRect rectClient;
	GetClientRect(rectClient);
	m_pwndCurrentTreeCtrl->SetWindowPos(NULL, 0, 0, rectClient.Width(), rectClient.Height(), SWP_NOACTIVATE | SWP_NOZORDER);

	//Invalidate();
}


void ModelBrowserBar::OnPaint()
{
	CPaintDC dc(this); // device context for painting

	CRect rectTree;
	m_pwndCurrentTreeCtrl->GetWindowRect(rectTree);
	ScreenToClient(rectTree);

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	// 그리기 메시지에 대해서는 CBCGPDockingControlBar::OnPaint()을(를) 호출하지 마십시오.
}
