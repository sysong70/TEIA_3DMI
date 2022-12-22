// RtDoc.cpp : implementation of the RtDoc class
//

#include "pch.h"
#include "DmiApp.h"

#include "DmiDoc.h"
#include "DmiView.h"

#include "MainFrm.h"

#include "HPSExchangeProgressDialog.h"

#include "DLL/DmiJsonCmd.h"
#include "../Common/DmiCommandDefine.h"

#ifdef _DEBUG
	#define new DEBUG_NEW
#endif

using namespace std::chrono;

// RtDoc

IMPLEMENT_DYNCREATE(DmiDoc, CDocument)

BEGIN_MESSAGE_MAP(DmiDoc, CDocument)
	ON_COMMAND(ID_FILE_SAVE, &DmiDoc::OnFileSaveAs)
	ON_COMMAND(ID_FILE_SAVE_AS, &DmiDoc::OnFileSaveAs)
END_MESSAGE_MAP()

int g_nRtDocId = 0;

// View 식별자
DmiDoc::DmiDoc()
{
	m_nId = g_nRtDocId;
	g_nRtDocId++;
}

DmiDoc::~DmiDoc()
{
	DmiJsonCmd::ExecuteHpsCommand(L"ResetModelHandler", m_nId);
	DmiJsonCmd::ExecuteC3dCommand(L"DeleteModel", m_nId);

// 	if(nullptr != m_pcExchangeProgressDialog) {
// 		delete m_pcExchangeProgressDialog;
// 	}
}

BOOL DmiDoc::OnNewDocument()
{
	if(!CDocument::OnNewDocument()) {
		return FALSE;
	}

	DmiJsonCmd::ExecuteHpsCommand(L"CreateModelHandler", m_nId);

	return TRUE;
}

BOOL DmiDoc::OnOpenDocument(LPCTSTR lpszPathName)
{
	if(FALSE == CDocument::OnOpenDocument(lpszPathName)) {
		return FALSE;
	}

	theApp.m_cFileImportStartTime = COleDateTime::GetCurrentTime();
	GetView()->SetFilePathName(lpszPathName);

	m_strFilePathName = lpszPathName;

	DmiJsonCmd::ExecuteHpsCommand(L"CreateModelHandler", m_nId);

/*
	if(nullptr == m_pcExchangeProgressDialog) {
		m_pcExchangeProgressDialog = new HPSExchangeProgressDialog(m_nId);
		m_pcExchangeProgressDialog->Create(HPSExchangeProgressDialog::IDD);
	}

	// Modaless Dialong 처리
	m_pcExchangeProgressDialog->SetFilePathName(lpszPathName);
	m_pcExchangeProgressDialog->ShowWindow(SW_SHOW);
*/

	//RtJsonCmd::ExecuteHpsCommand(RtHps::CommandId::ImportExchangeFile, m_nId, lpszPathName);

	// #temp_incremental_loading
	//RtJsonCmd::Excute3dxCommand(L"FileImport", m_nId, lpszPathName);

	return TRUE;
}

// RtDoc serialization

void DmiDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: add storing code here
	}
	else
	{
		// TODO: add loading code here
	}
}


// RtDoc diagnostics

#ifdef _DEBUG
void DmiDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void DmiDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG


DmiView * DmiDoc::GetView()
{
	POSITION pos = GetFirstViewPosition();
	if(pos != nullptr) {
		CView* pView = GetNextView(pos);
		if(pView != nullptr) {
			return (DmiView*) pView;
		}
	}

	ASSERT(false);
	return nullptr;
}

void DmiDoc::OnChangedViewList()
{
	CDocument::OnChangedViewList();
}

void DmiDoc::OnFileSaveAs()
{
	CString strFilePathName = GetPathName() + L".hsf";

	CString strFilter = _T("hsf Files (*.hsf)|*.hsf|All Files (*.*)|*.*||");
	CFileDialog cDLG(false, L"hsf", strFilePathName, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, strFilter);

	if(IDOK != cDLG.DoModal()) {
		return;
	}

	strFilePathName = cDLG.GetPathName();

	DmiJsonCmd::ExecuteHpsCommand(L"WriteHsfFile", m_nId, strFilePathName);
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
}

void DmiDoc::UpdatePropertyBarInformation()
{
	return UpdateImportExchangePropertyBarInformation();

	CBCGPPropList & cPropList = GetMainFrame()->GetPropBar().GetPropList();

	cPropList.RemoveAll();

	CBCGPProp * pcFileGroup = new CBCGPProp(_T("File"));
	{
		system_clock::time_point cTime1 = system_clock::time_point(system_clock::duration(m_nTimeTick[0]));
		system_clock::time_point cTime2 = system_clock::time_point(system_clock::duration(m_nTimeTick[1]));
		system_clock::time_point cTime3 = system_clock::time_point(system_clock::duration(m_nTimeTick[2]));

		auto cMilliSec1 = duration_cast<milliseconds>(cTime2 - cTime1);
		auto cMilliSec2 = duration_cast<milliseconds>(cTime3 - cTime2);
		auto cMilliSec4 = duration_cast<milliseconds>(cTime3 - cTime1);

		CString strTimeText = GetTimeSpanString(cMilliSec1);
		pcFileGroup->AddSubItem(new CBCGPProp(L"Import & Creat Solid", (_variant_t) strTimeText));

		strTimeText = GetTimeSpanString(cMilliSec2);
		pcFileGroup->AddSubItem(new CBCGPProp(L"Graphic Tessellation", (_variant_t) strTimeText));

		strTimeText = GetTimeSpanString(cMilliSec4);
		pcFileGroup->AddSubItem(new CBCGPProp(L"Total load time", (_variant_t) strTimeText));
	}
	cPropList.AddProperty(pcFileGroup);

	if(false == m_astrHpsErrMsgArray.IsEmpty()) {
		CBCGPProp * pcHpsErrGroup = new CBCGPProp(_T("HPS Error"));
		{
			for(INT_PTR nIndex = 0; nIndex < m_astrHpsErrMsgArray.GetCount(); nIndex++) {
				CString strTitle;
				strTitle.Format(L"%d", nIndex);

				pcHpsErrGroup->AddSubItem(new CBCGPProp(strTitle, (_variant_t) m_astrHpsErrMsgArray[nIndex]));
			}
		}
		cPropList.AddProperty(pcHpsErrGroup);
	}

	CBCGPProp * pcSelItemInfo = new CBCGPProp(_T("Select Item Information"));
	{
		pcSelItemInfo->AddSubItem(new CBCGPProp(L"Item", (_variant_t) m_strSelectItemInfo));
	}
	cPropList.AddProperty(pcSelItemInfo);

	cPropList.ExpandAll();
}

void DmiDoc::UpdateImportExchangePropertyBarInformation()
{
	CBCGPPropList & cPropList = GetMainFrame()->GetPropBar().GetPropList();

	cPropList.RemoveAll();

	CBCGPProp * pcFileGroup = new CBCGPProp(_T("File"));
	{
		system_clock::time_point cTime1 = system_clock::time_point(system_clock::duration(m_nTimeTick[0]));
		system_clock::time_point cTime2 = system_clock::time_point(system_clock::duration(m_nTimeTick[1]));
		system_clock::time_point cTime3 = system_clock::time_point(system_clock::duration(m_nTimeTick[2]));
		system_clock::time_point cTime4 = system_clock::time_point(system_clock::duration(m_nTimeTick[3]));

		auto cMilliSec1 = duration_cast<milliseconds>(cTime2 - cTime1);
		auto cMilliSec2 = duration_cast<milliseconds>(cTime3 - cTime2);
		auto cMilliSec3 = duration_cast<milliseconds>(cTime4 - cTime3);
		auto cMilliSec4 = duration_cast<milliseconds>(cTime4 - cTime1);

		CString strTimeText = GetTimeSpanString(cMilliSec1);
		pcFileGroup->AddSubItem(new CBCGPProp(L"Import & Tessellation", (_variant_t) strTimeText));

		strTimeText = GetTimeSpanString(cMilliSec2);
		pcFileGroup->AddSubItem(new CBCGPProp(L"Creating Graphics Database", (_variant_t) strTimeText));

		strTimeText = GetTimeSpanString(cMilliSec3);
		pcFileGroup->AddSubItem(new CBCGPProp(L"First Update Time", (_variant_t) strTimeText));

		strTimeText = GetTimeSpanString(cMilliSec4);
		pcFileGroup->AddSubItem(new CBCGPProp(L"Total load time", (_variant_t) strTimeText));
	}
	cPropList.AddProperty(pcFileGroup);

	if(false == m_astrHpsErrMsgArray.IsEmpty()) {
		CBCGPProp * pcHpsErrGroup = new CBCGPProp(_T("HPS Error"));
		{
			for(INT_PTR nIndex = 0; nIndex < m_astrHpsErrMsgArray.GetCount(); nIndex++) {
				CString strTitle;
				strTitle.Format(L"%d", nIndex);

				pcHpsErrGroup->AddSubItem(new CBCGPProp(strTitle, (_variant_t) m_astrHpsErrMsgArray[nIndex]));
			}
		}
		cPropList.AddProperty(pcHpsErrGroup);
	}

	CBCGPProp * pcSelItemInfo = new CBCGPProp(_T("Select Item Information"));
	{
		pcSelItemInfo->AddSubItem(new CBCGPProp(L"Item", (_variant_t) m_strSelectItemInfo));
	}
	cPropList.AddProperty(pcSelItemInfo);

	cPropList.ExpandAll();
}

CString DmiDoc::GetTimeSpanString(milliseconds & ms)
{
	auto secs = duration_cast<seconds>(ms);
	ms -= duration_cast<milliseconds>(secs);
	auto mins = duration_cast<minutes>(secs);
	secs -= duration_cast<seconds>(mins);

	CString strSpanText;

	if(0 < mins.count()) {
		strSpanText.Format(L"%dm %d.%ds", mins.count(), secs.count(), (int) (ms.count() / 10));
	}
	else {
		strSpanText.Format(L"%d.%ds", secs.count(), (int) (ms.count() / 10));
	}

	return strSpanText;
}

//== Model Tree Function ===========================================================================

// 1. File Open후 최초 Model Tree 생성
void DmiDoc::CreateModelTreeInformation()
{
	Json::Object cRetObject;
	if(false == DmiJsonCmd::ExecuteHpsCommand(DmiHps::Command::CreateComponentRootTreeItem, m_nId, cRetObject)) {
		ASSERT(false);
	}

	CreateModelTree(cRetObject);

// 	CString strModelFileTitle = m_strFilePathName.Right(m_strFilePathName.GetLength() - m_strFilePathName.ReverseFind('\\') - 1);
// 	m_pcModelTreeCtrl = GetMainFrame()->GetModelBrowserBar().GetModelTreeCtrl();
// 	m_pcModelTreeCtrl->InsertItem(strModelFileTitle);
}

// 2. Json 정보를 이용해서 Model Tree 생성
void DmiDoc::CreateModelTree(Json::Object & cInObject)
{
	// 1. 새롭게 Model Tree Control을 Model Browser에서 가져옴.
	m_pcModelTreeCtrl = GetMainFrame()->GetModelBrowserBar().CreateModelTreeCtrl(m_nId);
	HTREEITEM hTreeItem = m_pcModelTreeCtrl->GetRootItem();

/*
	CString strTitle = cInObject.GetString("Title");
	int nType = cInObject.GetDwordPtr("Type");
	bool bHasChildren = cInObject.GetBoolean("HasChildren");
	DWORD_PTR nItemData = cInObject.GetDwordPtr("CompPtr");

	int nImageIndex = GetMainFrame()->GetModelBrowserBar().GetImageBaseIndex(nType);

	TVINSERTSTRUCT cInsertStruct;
	cInsertStruct.hParent = hTreeItem;
	cInsertStruct.hInsertAfter = TVI_LAST;
	cInsertStruct.item.mask = TVIF_CHILDREN | TVIF_TEXT | TVIF_PARAM | TVIF_IMAGE | TVIF_SELECTEDIMAGE;
	cInsertStruct.item.cChildren = bHasChildren;
	cInsertStruct.item.lParam = (LPARAM) nItemData;
	cInsertStruct.item.pszText = (LPTSTR) strTitle.GetBuffer();
	cInsertStruct.item.iImage = nImageIndex;
	cInsertStruct.item.iSelectedImage = nImageIndex;
*/

	m_pcModelTreeCtrl->InsertJsonItem(hTreeItem, cInObject);

	//m_pcModelTreeCtrl->InsertItem(&cInsertStruct);

	//m_pcModelTreeCtrl->Expand(hItem, TVE_EXPAND);

	//cInsertStruct.hParent = hItem;
 	//m_pcModelTreeCtrl->InsertItem(&cInsertStruct);

	GetMainFrame()->GetModelBrowserBar().Init(m_nId, m_pcModelTreeCtrl);
}



void DmiDoc::UpdateModelTreeInformation()
{
	GetMainFrame()->GetModelBrowserBar().Init(m_nId, m_pcModelTreeCtrl);
}