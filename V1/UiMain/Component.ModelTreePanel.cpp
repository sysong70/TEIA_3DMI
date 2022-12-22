#include "stdafx.h"
#include "resource.h"
#include "Component.ModelTreePanel.h"
#include "Facility.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif



#define PRESET PresetModelTreePanel

namespace PresetModelTreePanel
{
	const UINT Id = WM_USER;

	int RowHeight()
	{
		return globalUtils.ScaleByDPI(24);
	}
}



using namespace Component;

BEGIN_MESSAGE_MAP(ModelTreePanel, Panel)
	ON_REGISTERED_MESSAGE(BCGM_GRID_ROW_CHECKBOX_CLICK, OnCheckClickTree)

	ON_NOTIFY(TVN_BEGINDRAG, PRESET::Id, OnBeginDragTree)
	ON_NOTIFY(TVN_BEGINLABELEDIT, PRESET::Id, OnBeginLabelEditTree)
	ON_NOTIFY(NM_CLICK, PRESET::Id, OnClickTree)
	ON_NOTIFY(NM_DBLCLK, PRESET::Id, OnDblClickTree)
	ON_NOTIFY(TVN_DELETEITEM, PRESET::Id, OnDeleteItemTree)
	ON_NOTIFY(TVN_ENDLABELEDIT, PRESET::Id, OnEndLabelEditTree)
	ON_NOTIFY(TVN_ITEMEXPANDED, PRESET::Id, OnItemExpandedTree)
	ON_NOTIFY(TVN_ITEMEXPANDING, PRESET::Id, OnItemExpandingTree)
	ON_NOTIFY(NM_RCLICK, PRESET::Id, OnRClickTree)
	ON_NOTIFY(NM_RDBLCLK, PRESET::Id, OnRDbClickTree)
	ON_NOTIFY(TVN_SELCHANGED, PRESET::Id, OnSelChangedTree)
	ON_NOTIFY(TVN_SELCHANGING, PRESET::Id, OnSelChangingTree)
	ON_NOTIFY(NM_SETFOCUS, PRESET::Id, OnSetFocusTree)

	//ON_MESSAGE(WM_DPICHANGED_AFTERPARENT, OnDPIChangedAfterParent)
	ON_COMMAND_RANGE(TOOLBAR_3D_CMD_Sort_ByOriginal, TOOLBAR_3D_CMD_Option_AlternateRows, OnCommand)
END_MESSAGE_MAP()



Component::ModelTreePanel::ModelTreePanel()
{
}



Component::ModelTreePanel::~ModelTreePanel()
{
}



void Component::ModelTreePanel::ReceiveSignal(Json::Object* pData)
{
	Json::Object& data = *pData;

	DEBUG_STOP;

	REMOVE_POINTER(pData);
}



void Component::ModelTreePanel::AdjustLayout(int cx, int cy)
{
	__super::AdjustLayout(cx, cy);

	CRect rect = GetBodyRect();
	m_wndControl.SetWindowPos(nullptr, rect.left, rect.top, rect.Width(), rect.Height(), SWP_NOACTIVATE);
}



int Component::ModelTreePanel::ConstructHeader(int cx)
{
	m_toolBar.SetPivot(EPivot::TopLeft);
	m_toolBar.Initialize(this);

	m_toolBar.AddButton(TOOLBAR_3D_CMD_Sort_ByOriginal);
	m_toolBar.AddButton(TOOLBAR_3D_CMD_Sort_ByAscending);
	m_toolBar.AddButton(TOOLBAR_3D_CMD_Sort_ByDescending);
	m_toolBar.AddSeperator();
	m_toolBar.AddButton(TOOLBAR_3D_CMD_Option_GridLines);
	m_toolBar.AddButton(TOOLBAR_3D_CMD_Option_AlternateRows);

	return m_nHeaderHeight = m_toolBar.AdjustLayout().cy;
}



void Component::ModelTreePanel::ConstructBody()
{
	DWORD dwStyle = WS_CHILD | WS_VISIBLE |
		TVS_HASLINES | TVS_TRACKSELECT | TVS_LINESATROOT | TVS_HASBUTTONS |
		TVS_SHOWSELALWAYS | TVS_FULLROWSELECT;

	if (m_wndControl.Create(dwStyle, GetBodyRect(), this, PRESET::Id) == FALSE) {
		DEBUG_RETURN;
	}

	m_wndControl.SetVisualManagerColorTheme();
	m_wndControl.EnableColumnAutoSize();
	m_wndControl.EnableAlternateRows(FALSE);
	m_wndControl.EnableGridLines(FALSE);
	m_wndControl.ModifyStyle(0, TVS_CHECKBOXES); // EnableCheckBoxes() not working
	m_wndControl.ModifyStyle(TVS_TRACKSELECT, 0); // off hot tracking
	m_wndControl.SetCustomRowHeight(PRESET::RowHeight());
	m_wndControl.SetSingleSel(FALSE);

	//:WARNING - do not use local string
	BCGP_GRID_FILTERBAR_OPTIONS filter(m_sFilterMessage = Facility::Local(L"Search models...|모델 검색..."));
	filter.m_clrMarkBackground = (COLORREF)EColor::White;
	filter.m_clrMarkText = 0;
	filter.m_bAutoExpandGroups = TRUE;
	filter.m_bIncludeGroups = TRUE;

	m_wndControl.SetOutOfFilterLabel(Facility::Local(L"No items match your search.|일치하는 항목을 찾을 수 없습니다."));
	m_wndControl.EnableFilterBar(TRUE, filter);
	m_wndControl.OnFilterBarUpdate(0);

#ifdef _DEBUG
	m_wndControl.SetRedraw(FALSE);

	auto GetRandom = []() -> CString {
		const CString& CHARS = L"ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
		const int LENGTH = CHARS.GetLength();

		CString random;
		for (int i = 0; i < 20; ++i) {
			int index = rand() % LENGTH;
			TCHAR nextChar = CHARS[index];
			random.AppendChar(nextChar);
		}

		return random;
	};

	HTREEITEM parent[6] = { nullptr };
	for (int i = 0; i < 300; i++) {
		int index = rand() % 5;
		HTREEITEM hParent = parent[index];
		HTREEITEM hCurrent = m_wndControl.InsertItem(GetRandom(), hParent);
		m_wndControl.SetCheck(hCurrent);
		parent[index + 1] = hCurrent;
	}

	m_wndControl.SetRedraw(TRUE);
	//:WARNING - UpdateWindow or RedrawWindow not working 
	m_wndControl.AdjustLayout();
#endif
}



void Component::ModelTreePanel::OnCommand(UINT id)
{
	if (m_wndControl.GetSafeHwnd() == nullptr) {
		return;
	}

	switch (id) {
	case TOOLBAR_3D_CMD_Sort_ByOriginal: m_wndControl.RemoveSortColumn(0); break;
	case TOOLBAR_3D_CMD_Sort_ByAscending: m_wndControl.SetSortColumn(0, TRUE); break;
	case TOOLBAR_3D_CMD_Sort_ByDescending: m_wndControl.SetSortColumn(0, FALSE); break;
	case TOOLBAR_3D_CMD_Option_GridLines: EnableGridLines(); break;
	case TOOLBAR_3D_CMD_Option_AlternateRows: EnableAlternateRows(); break;

	default:
		DEBUG_RETURN;
	}

	m_wndControl.AdjustLayout();
	m_wndControl.RedrawWindow();
}



LRESULT Component::ModelTreePanel::OnCheckClickTree(WPARAM wp, LPARAM lp)
{
	CBCGPGridRow* pRow = (CBCGPGridRow*)lp;
	if (pRow == NULL) {
		return 0;
	}

	if (pRow->HasCheckBox()) {
		//:CHECK
		BOOL checked = pRow->GetCheck();
		pRow->SetCheck(!checked);
		pRow->CheckSubItems(!checked);
		pRow->UpdateParentCheckbox(TRUE);

		m_wndControl.RedrawWindow();
	}

	return TRUE; // disable the default implementation
}



void Component::ModelTreePanel::OnBeginDragTree(NMHDR* pNMHDR, LRESULT* pResult)
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;

	DEBUG_TRACE(L"TVN_BEGINDRAG: item: %s; x: %d, y: %d\r\n",
		(LPCTSTR)m_wndControl.GetItemText(pNMTreeView->itemNew.hItem),
		pNMTreeView->ptDrag.x, pNMTreeView->ptDrag.y);

	*pResult = 0;
}



void Component::ModelTreePanel::OnBeginLabelEditTree(NMHDR* pNMHDR, LRESULT* pResult)
{
	TV_DISPINFO* pTVDispInfo = (TV_DISPINFO*)pNMHDR;

	CEdit* pEdit = (CEdit*)CWnd::FromHandle((HWND)m_wndControl.SendMessage(TVM_GETEDITCONTROL));
	if (pEdit->GetSafeHwnd() != NULL) {
		pEdit->PostMessage(EM_SETSEL, 0, (LPARAM)-1);
	}

	DEBUG_TRACE(L"TVN_BEGINLABELEDIT: item: %s\r\n", pTVDispInfo->item.pszText);

	*pResult = 0;
}



void Component::ModelTreePanel::OnClickTree(NMHDR* pNMHDR, LRESULT* pResult)
{
	UNREFERENCED_PARAMETER(pNMHDR);
	DEBUG_TRACE(L"NM_CLICK\r\n");

	*pResult = 0;
}



void Component::ModelTreePanel::OnDblClickTree(NMHDR* pNMHDR, LRESULT* pResult)
{
	UNREFERENCED_PARAMETER(pNMHDR);
	DEBUG_TRACE(L"NM_DBLCLICK\r\n");

	*pResult = 0;
}



void Component::ModelTreePanel::OnDeleteItemTree(NMHDR* pNMHDR, LRESULT* pResult)
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;

	DEBUG_TRACE(L"TVN_DELETEITEM: item: %s\r\n",
		(LPCTSTR)m_wndControl.GetItemText(pNMTreeView->itemOld.hItem));

	*pResult = 0;
}



void Component::ModelTreePanel::OnEndLabelEditTree(NMHDR* pNMHDR, LRESULT* pResult)
{
	TV_DISPINFO* pTVDispInfo = (TV_DISPINFO*)pNMHDR;

	DEBUG_TRACE(L"TVN_ENDLABELEDIT: item: %s\r\n",
		pTVDispInfo->item.pszText == NULL ? L"CANCELED" : pTVDispInfo->item.pszText);

	*pResult = 0;
}



void Component::ModelTreePanel::OnItemExpandedTree(NMHDR* pNMHDR, LRESULT* pResult)
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;

	const CString action = pNMTreeView->action == TVE_EXPAND ? L"expand" :
		pNMTreeView->action == TVE_COLLAPSE ? L"collapse" : L"unknown";

	DEBUG_TRACE(L"TVN_ITEMEXPANDED: item: %s; action: %s\r\n",
		(LPCTSTR)m_wndControl.GetItemText(pNMTreeView->itemNew.hItem), (LPCTSTR)action);

	*pResult = 0;
}



void Component::ModelTreePanel::OnItemExpandingTree(NMHDR* pNMHDR, LRESULT* pResult)
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;

	const CString action = pNMTreeView->action == TVE_EXPAND ? L"expand" :
		pNMTreeView->action == TVE_COLLAPSE ? L"collapse" : L"unknown";

	DEBUG_TRACE(L"TVN_ITEMEXPANDING: item: %s; action: %s\r\n",
		(LPCTSTR)m_wndControl.GetItemText(pNMTreeView->itemNew.hItem), (LPCTSTR)action);
	*pResult = 0;
}



void Component::ModelTreePanel::OnRClickTree(NMHDR* pNMHDR, LRESULT* pResult)
{
	UNREFERENCED_PARAMETER(pNMHDR);
	DEBUG_TRACE(L"NM_RCLICK\r\n");

	*pResult = 0;
}



void Component::ModelTreePanel::OnRDbClickTree(NMHDR* pNMHDR, LRESULT* pResult)
{
	UNREFERENCED_PARAMETER(pNMHDR);
	DEBUG_TRACE(L"NM_RDBLCLICK\r\n");

	*pResult = 0;
}



void Component::ModelTreePanel::OnSelChangedTree(NMHDR* pNMHDR, LRESULT* pResult)
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;

	const CString oldItem = pNMTreeView->itemOld.hItem == NULL ? L"(none)" : m_wndControl.GetItemText(pNMTreeView->itemOld.hItem);
	const CString newItem = pNMTreeView->itemNew.hItem == NULL ? L"(none)" : m_wndControl.GetItemText(pNMTreeView->itemNew.hItem);
	const CString action = pNMTreeView->action == TVC_BYMOUSE ? L"by mouse" :
		pNMTreeView->action == TVC_BYKEYBOARD ? L"by keyboard" : L"unknown";

	DEBUG_TRACE(L"TVN_SELCHANGED: Old item: %s New item: %s; action: %s\r\n",
		(LPCTSTR)oldItem, (LPCTSTR)newItem, (LPCTSTR)action);

	*pResult = 0;
}



void Component::ModelTreePanel::OnSelChangingTree(NMHDR* pNMHDR, LRESULT* pResult)
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;

	const CString oldItem = pNMTreeView->itemOld.hItem == NULL ? L"(none)" : m_wndControl.GetItemText(pNMTreeView->itemOld.hItem);
	const CString newItem = pNMTreeView->itemNew.hItem == NULL ? L"(none)" : m_wndControl.GetItemText(pNMTreeView->itemNew.hItem);
	const CString action = pNMTreeView->action == TVC_BYMOUSE ? L"by mouse" :
		pNMTreeView->action == TVC_BYKEYBOARD ? L"by keyboard" : L"unknown";

	DEBUG_TRACE(L"TVN_SELCHANGING: Old item: %s New item: %s; action: %s\r\n",
		(LPCTSTR)oldItem, (LPCTSTR)newItem, (LPCTSTR)action);

	*pResult = 0;
}



void Component::ModelTreePanel::OnSetFocusTree(NMHDR* pNMHDR, LRESULT* pResult)
{
	UNREFERENCED_PARAMETER(pNMHDR);
	DEBUG_TRACE(L"NM_SETFOCUS\r\n");

	*pResult = 0;
}



void Component::ModelTreePanel::GetParent(HTREEITEM sel, std::vector<HTREEITEM>& parent)
{
}



void Component::ModelTreePanel::EnableAlternateRows()
{
	m_bAlternateRows = !m_bAlternateRows;
	m_wndControl.EnableAlternateRows(m_bAlternateRows);
}



void Component::ModelTreePanel::EnableGridLines()
{
	m_bGridLines = !m_bGridLines;
	m_wndControl.EnableGridLines(m_bGridLines);
}

#undef PRESET
