// BackStagePageInfo.cpp : implementation file
//

#include "pch.h"
#include "DmiApp.h"
#include "MainFrm.h"
#include "BackStagePageInfo.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CBackStagePageInfo dialog

IMPLEMENT_DYNCREATE(CBackStagePageInfo, CBCGPDialog)

CBackStagePageInfo::CBackStagePageInfo(CWnd* pParent /*=NULL*/)
	: CBCGPDialog(IDD_FORM_INFO, pParent)
{
	m_strDocName = _T("");
	m_strPath = _T("");
	EnableLayout();
}

void CBackStagePageInfo::DoDataExchange(CDataExchange* pDX)
{
	CBCGPDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PATH_LABEL, m_wndPath);
	DDX_Control(pDX, IDC_INFO_LABEL, m_wndInfo);
	DDX_Control(pDX, IDC_DOC_NAME, m_wndDocName);
	DDX_Control(pDX, IDC_PREVIEW, m_btnPreview);
	DDX_Text(pDX, IDC_DOC_NAME, m_strDocName);
	DDX_Text(pDX, IDC_PATH_LABEL, m_strPath);
}


BEGIN_MESSAGE_MAP(CBackStagePageInfo, CBCGPDialog)
	ON_BN_CLICKED(IDC_PREVIEW, OnPreview)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CBackStagePageInfo message handlers

BOOL CBackStagePageInfo::OnInitDialog()
{
	CBCGPDialog::OnInitDialog();

	LOGFONT lf;
	globalData.fontCaption.GetLogFont(&lf);

	m_fontCaption.CreateFontIndirect(&lf);

	const COLORREF clrCaption = CBCGPVisualManager::GetInstance()->GetRibbonBackstageInfoTextColor();

	m_wndInfo.SetFont(&m_fontCaption);
	m_wndInfo.m_clrText = clrCaption;

	m_wndDocName.SetFont(&m_fontCaption);
	m_wndDocName.m_clrText = clrCaption;

	m_wndPath.ModifyStyle(0, SS_PATHELLIPSIS);

	CBCGPStaticLayout* pLayout = (CBCGPStaticLayout*)GetLayout();
	ASSERT_VALID(pLayout);

	pLayout->AddAnchor(IDC_DOC_NAME, CBCGPStaticLayout::e_MoveTypeNone, CBCGPStaticLayout::e_SizeTypeHorz);
	pLayout->AddAnchor(IDC_PATH_LABEL, CBCGPStaticLayout::e_MoveTypeNone, CBCGPStaticLayout::e_SizeTypeHorz);

	pLayout->AddAnchor(IDC_SEPARATOR_1, CBCGPStaticLayout::e_MoveTypeNone, CBCGPStaticLayout::e_SizeTypeHorz);
	pLayout->AddAnchor(IDC_TEXT1, CBCGPStaticLayout::e_MoveTypeNone, CBCGPStaticLayout::e_SizeTypeHorz);

	pLayout->AddAnchor(IDC_SEPARATOR_2, CBCGPStaticLayout::e_MoveTypeNone, CBCGPStaticLayout::e_SizeTypeHorz);
	pLayout->AddAnchor(IDC_TEXT2, CBCGPStaticLayout::e_MoveTypeNone, CBCGPStaticLayout::e_SizeTypeHorz);

	pLayout->AddAnchor(IDC_SEPARATOR_3, CBCGPStaticLayout::e_MoveTypeNone, CBCGPStaticLayout::e_SizeTypeHorz);
	pLayout->AddAnchor(IDC_TEXT3, CBCGPStaticLayout::e_MoveTypeNone, CBCGPStaticLayout::e_SizeTypeHorz);

	pLayout->AddAnchor(IDC_SEPARATOR_4, CBCGPStaticLayout::e_MoveTypeHorz, CBCGPStaticLayout::e_SizeTypeVert);

	pLayout->AddAnchor(IDC_INFO_LABEL, CBCGPStaticLayout::e_MoveTypeHorz, CBCGPStaticLayout::e_SizeTypeNone);
	pLayout->AddAnchor(IDC_SEPARATOR_5, CBCGPStaticLayout::e_MoveTypeHorz, CBCGPStaticLayout::e_SizeTypeNone);
	pLayout->AddAnchor(IDC_PREVIEW, CBCGPStaticLayout::e_MoveTypeHorz, CBCGPStaticLayout::e_SizeTypeNone);
	pLayout->AddAnchor(IDC_TEXT4, CBCGPStaticLayout::e_MoveTypeHorz, CBCGPStaticLayout::e_SizeTypeNone);

	CFrameWnd* pMainFrame = (CFrameWnd*)AfxGetMainWnd();

	CFrameWnd* pFrame = pMainFrame->GetActiveFrame();
	if (pFrame != NULL)
	{
		CDocument* pDoc = pFrame->GetActiveDocument();
		if (pDoc != NULL)
		{
			m_strDocName = pDoc->GetTitle();
			m_strPath = pDoc->GetPathName();

			UpdateData(FALSE);
		}
	}

	PreparePreviewBitmap();

	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}

void CBackStagePageInfo::PreparePreviewBitmap()
{
	CMainFrame* pMainFrame = DYNAMIC_DOWNCAST(CMainFrame, AfxGetMainWnd());
	if (pMainFrame == NULL)
	{
		return;
	}

	CBitmap& bmpAppPreview = pMainFrame->GetAppPreview();

	BITMAP bmp;
	bmpAppPreview.GetBitmap(&bmp);

	CRect rectPreview;
	m_btnPreview.GetClientRect(rectPreview);
	double dblScaleRatio = min((double)rectPreview.Width() / bmp.bmWidth, (double)rectPreview.Height() / bmp.bmHeight);

	CSize szDst((int)(bmp.bmWidth * dblScaleRatio), (int)(bmp.bmHeight * dblScaleRatio));

	HBITMAP hBitmap = CBCGPDrawManager::CreateBitmap_24(szDst, NULL);

	{
		CDC dcDst;
		dcDst.CreateCompatibleDC(NULL);

		HBITMAP hBitmapOld = (HBITMAP)dcDst.SelectObject (hBitmap);

		CBCGPDrawManager dm(dcDst);
		int nShadowSize = 5;

		szDst.cx -= 2 * nShadowSize;
		szDst.cy -= 2 * nShadowSize;

		CBCGPVisualManager::GetInstance()->OnFillRibbonBackstageForm(&dcDst, this, rectPreview);
		dm.DrawShadow(CRect(CPoint(nShadowSize, nShadowSize), szDst), nShadowSize);

		CDC dcSrc;
		dcSrc.CreateCompatibleDC(&dcDst);

		HBITMAP hBitmapViewOld = (HBITMAP)dcSrc.SelectObject(bmpAppPreview.GetSafeHandle());

		dcDst.SetStretchBltMode(HALFTONE);

		dcDst.StretchBlt(nShadowSize, nShadowSize, szDst.cx, szDst.cy, &dcSrc, 0, 0, bmp.bmWidth, bmp.bmHeight, SRCCOPY);

		dcSrc.SelectObject (hBitmapViewOld);
		dcDst.SelectObject (hBitmapOld);
	}

	m_btnPreview.SetMouseCursorHand();
	m_btnPreview.SetDrawText(FALSE);
	m_btnPreview.SetImage(hBitmap);
	m_btnPreview.m_bTransparent = TRUE;
	m_btnPreview.m_nFlatStyle = CBCGPButton::BUTTONSTYLE_NOBORDERS;
	m_btnPreview.SizeToContent();

	bmpAppPreview.DeleteObject();
}

void CBackStagePageInfo::OnPreview()
{
	GetParent()->SendMessage(WM_CLOSE);
}

