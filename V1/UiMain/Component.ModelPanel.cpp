#include "stdafx.h"
#include "resource.h"
#include "Component.ModelPanel.h"
#include "Facility.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif



#define PRESET PresetModelPanel

namespace PresetModelPanel
{
	const UINT Id = WM_USER;
}



using namespace Component;

BEGIN_MESSAGE_MAP(ModelPanel, Panel)
	ON_REGISTERED_MESSAGE(BCGM_GRID_ROW_CHECKBOX_CLICK, OnTreeCheckClick)

	ON_NOTIFY(TVN_BEGINDRAG, PRESET::Id, OnTreeBeginDrag)
	ON_NOTIFY(TVN_BEGINLABELEDIT, PRESET::Id, OnTreeBeginLabelEdit)
	ON_NOTIFY(NM_CLICK, PRESET::Id, OnTreeClick)
	ON_NOTIFY(NM_DBLCLK, PRESET::Id, OnTreeDblClick)
	ON_NOTIFY(TVN_DELETEITEM, PRESET::Id, OnTreeDeleteItem)
	ON_NOTIFY(TVN_ENDLABELEDIT, PRESET::Id, OnTreeEndLabelEdit)
	ON_NOTIFY(TVN_ITEMEXPANDED, PRESET::Id, OnTreeItemExpanded)
	ON_NOTIFY(TVN_ITEMEXPANDING, PRESET::Id, OnTreeItemExpanding)
	ON_NOTIFY(NM_RCLICK, PRESET::Id, OnTreeRClick)
	ON_NOTIFY(NM_RDBLCLK, PRESET::Id, OnTreeRDbClick)
	ON_NOTIFY(TVN_SELCHANGED, PRESET::Id, OnTreeSelChanged)
	ON_NOTIFY(TVN_SELCHANGING, PRESET::Id, OnTreeSelChanging)
	ON_NOTIFY(NM_SETFOCUS, PRESET::Id, OnTreeSetFocus)

	//ON_MESSAGE(WM_DPICHANGED_AFTERPARENT, OnDPIChangedAfterParent)
	ON_COMMAND_RANGE(TOOLBAR_3D_CMD_Sort_ByOriginal, TOOLBAR_3D_CMD_Option_AlternateRows, OnCommand)
END_MESSAGE_MAP()



Component::ModelPanel::ModelPanel()
{
}



Component::ModelPanel::~ModelPanel()
{
}



void Component::ModelPanel::ReceiveSignal(Json::Object* pData)
{
	Json::Object& data = *pData;

	DEBUG_STOP;

	REMOVE_POINTER(pData);
}



void Component::ModelPanel::AdjustLayout(int cx, int cy)
{
	__super::AdjustLayout(cx, cy);

	CRect rect = GetBodyRect();
	m_wndControl.SetWindowPos(nullptr, rect.left, rect.top, rect.Width(), rect.Height(), SWP_NOACTIVATE);
}



int Component::ModelPanel::ConstructHeader(int cx)
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



void Component::ModelPanel::ConstructBody()
{
	DWORD dwStyle = WS_CHILD | WS_VISIBLE |
		TVS_CHECKBOXES | TVS_FULLROWSELECT | TVS_HASBUTTONS | TVS_HASLINES | TVS_LINESATROOT | TVS_SHOWSELALWAYS;
	if (m_wndControl.Create(dwStyle, GetBodyRect(), this, PRESET::Id) == FALSE) {
		DEBUG_RETURN;
	}

	m_wndControl.SetVisualManagerColorTheme();
	m_wndControl.EnableColumnAutoSize();
	m_wndControl.EnableAlternateRows(FALSE);
	m_wndControl.EnableGridLines(FALSE);
	m_wndControl.ModifyStyle(0, TVS_CHECKBOXES); // EnableCheckBoxes() not working
	//m_wndControl.SetCustomRowHeight(TreeRowHeight());
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



void Component::ModelPanel::OnCommand(UINT id)
{
	if (m_wndControl.GetSafeHwnd() == nullptr) {
		return;
	}

	switch (id) {
	case TOOLBAR_3D_CMD_Sort_ByOriginal:      m_wndControl.RemoveSortColumn(0);     break;
	case TOOLBAR_3D_CMD_Sort_ByAscending:     m_wndControl.SetSortColumn(0, TRUE);  break;
	case TOOLBAR_3D_CMD_Sort_ByDescending:    m_wndControl.SetSortColumn(0, FALSE); break;
	case TOOLBAR_3D_CMD_Option_GridLines:     EnableGridLines();                    break;
	case TOOLBAR_3D_CMD_Option_AlternateRows: EnableAlternateRows();                break;

	default:
		DEBUG_RETURN;
	}

	m_wndControl.AdjustLayout();
	m_wndControl.RedrawWindow();
}



LRESULT Component::ModelPanel::OnTreeCheckClick(WPARAM wp, LPARAM lp)
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



void Component::ModelPanel::OnTreeBeginDrag(NMHDR* pNMHDR, LRESULT* pResult)
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;

	DEBUG_TRACE(L"TVN_BEGINDRAG: item: %s; x: %d, y: %d\r\n",
		(LPCTSTR)m_wndControl.GetItemText(pNMTreeView->itemNew.hItem),
		pNMTreeView->ptDrag.x, pNMTreeView->ptDrag.y);

	*pResult = S_OK;
}



void Component::ModelPanel::OnTreeBeginLabelEdit(NMHDR* pNMHDR, LRESULT* pResult)
{
	TV_DISPINFO* pTVDispInfo = (TV_DISPINFO*)pNMHDR;

	CEdit* pEdit = (CEdit*)CWnd::FromHandle((HWND)m_wndControl.SendMessage(TVM_GETEDITCONTROL));
	if (pEdit->GetSafeHwnd() != NULL) {
		pEdit->PostMessage(EM_SETSEL, 0, (LPARAM)-1);
	}

	DEBUG_TRACE(L"TVN_BEGINLABELEDIT: item: %s\r\n", pTVDispInfo->item.pszText);

	*pResult = S_OK;
}



void Component::ModelPanel::OnTreeClick(NMHDR* pNMHDR, LRESULT* pResult)
{
	*pResult = S_OK;

	UINT flag = 0;
	CPoint point;
	GetCursorPos(&point);
	m_wndControl.ScreenToClient(&point);

	HTREEITEM hItem = m_wndControl.HitTest(point, &flag);
	if (hItem == nullptr) {
		return;
	}

	if (flag & TVHT_ONITEMBUTTON) {
		UINT state = m_wndControl.GetItemState(hItem, TVIS_EXPANDED);
		m_wndControl.Expand(hItem, (state & TVIS_EXPANDED ? TVE_COLLAPSE : TVE_EXPAND));
		*pResult = S_FALSE;
	}
}



void Component::ModelPanel::OnTreeDblClick(NMHDR* pNMHDR, LRESULT* pResult)
{
	UNREFERENCED_PARAMETER(pNMHDR);
	DEBUG_TRACE(L"NM_DBLCLICK\r\n");

	*pResult = S_OK;
}



void Component::ModelPanel::OnTreeDeleteItem(NMHDR* pNMHDR, LRESULT* pResult)
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;

	DEBUG_TRACE(L"TVN_DELETEITEM: item: %s\r\n",
		(LPCTSTR)m_wndControl.GetItemText(pNMTreeView->itemOld.hItem));

	*pResult = S_OK;
}



void Component::ModelPanel::OnTreeEndLabelEdit(NMHDR* pNMHDR, LRESULT* pResult)
{
	TV_DISPINFO* pTVDispInfo = (TV_DISPINFO*)pNMHDR;

	DEBUG_TRACE(L"TVN_ENDLABELEDIT: item: %s\r\n",
		pTVDispInfo->item.pszText == NULL ? L"CANCELED" : pTVDispInfo->item.pszText);

	*pResult = S_OK;
}



void Component::ModelPanel::OnTreeItemExpanded(NMHDR* pNMHDR, LRESULT* pResult)
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;

	const CString action = pNMTreeView->action == TVE_EXPAND ? L"expand" :
		pNMTreeView->action == TVE_COLLAPSE ? L"collapse" : L"unknown";

	DEBUG_TRACE(L"TVN_ITEMEXPANDED: item: %s; action: %s\r\n",
		(LPCTSTR)m_wndControl.GetItemText(pNMTreeView->itemNew.hItem), (LPCTSTR)action);

	*pResult = S_OK;
}



void Component::ModelPanel::OnTreeItemExpanding(NMHDR* pNMHDR, LRESULT* pResult)
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;

	const CString action = pNMTreeView->action == TVE_EXPAND ? L"expand" :
		pNMTreeView->action == TVE_COLLAPSE ? L"collapse" : L"unknown";

	DEBUG_TRACE(L"TVN_ITEMEXPANDING: item: %s; action: %s\r\n",
		(LPCTSTR)m_wndControl.GetItemText(pNMTreeView->itemNew.hItem), (LPCTSTR)action);

	*pResult = S_OK;
}



void Component::ModelPanel::OnTreeRClick(NMHDR* pNMHDR, LRESULT* pResult)
{
	UNREFERENCED_PARAMETER(pNMHDR);
	DEBUG_TRACE(L"NM_RCLICK\r\n");

	*pResult = S_OK;
}



void Component::ModelPanel::OnTreeRDbClick(NMHDR* pNMHDR, LRESULT* pResult)
{
	UNREFERENCED_PARAMETER(pNMHDR);
	DEBUG_TRACE(L"NM_RDBLCLICK\r\n");

	*pResult = S_OK;
}



void Component::ModelPanel::OnTreeSelChanged(NMHDR* pNMHDR, LRESULT* pResult)
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;

	const CString oldItem = pNMTreeView->itemOld.hItem == NULL ? L"(none)" : m_wndControl.GetItemText(pNMTreeView->itemOld.hItem);
	const CString newItem = pNMTreeView->itemNew.hItem == NULL ? L"(none)" : m_wndControl.GetItemText(pNMTreeView->itemNew.hItem);
	const CString action = pNMTreeView->action == TVC_BYMOUSE ? L"by mouse" :
		pNMTreeView->action == TVC_BYKEYBOARD ? L"by keyboard" : L"unknown";

	DEBUG_TRACE(L"TVN_SELCHANGED: Old item: %s New item: %s; action: %s\r\n",
		(LPCTSTR)oldItem, (LPCTSTR)newItem, (LPCTSTR)action);

	*pResult = S_OK;
}



void Component::ModelPanel::OnTreeSelChanging(NMHDR* pNMHDR, LRESULT* pResult)
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;

	const CString oldItem = pNMTreeView->itemOld.hItem == NULL ? L"(none)" : m_wndControl.GetItemText(pNMTreeView->itemOld.hItem);
	const CString newItem = pNMTreeView->itemNew.hItem == NULL ? L"(none)" : m_wndControl.GetItemText(pNMTreeView->itemNew.hItem);
	const CString action = pNMTreeView->action == TVC_BYMOUSE ? L"by mouse" :
		pNMTreeView->action == TVC_BYKEYBOARD ? L"by keyboard" : L"unknown";

	DEBUG_TRACE(L"TVN_SELCHANGING: Old item: %s New item: %s; action: %s\r\n",
		(LPCTSTR)oldItem, (LPCTSTR)newItem, (LPCTSTR)action);

	*pResult = S_OK;
}



void Component::ModelPanel::OnTreeSetFocus(NMHDR* pNMHDR, LRESULT* pResult)
{
	UNREFERENCED_PARAMETER(pNMHDR);
	DEBUG_TRACE(L"NM_SETFOCUS\r\n");

	*pResult = S_OK;
}



void Component::ModelPanel::GetParent(HTREEITEM sel, std::vector<HTREEITEM>& parent)
{
	DEBUG_STOP;
}



void Component::ModelPanel::EnableAlternateRows()
{
	m_bAlternateRows = !m_bAlternateRows;
	m_wndControl.EnableAlternateRows(m_bAlternateRows);
}



void Component::ModelPanel::EnableGridLines()
{
	m_bGridLines = !m_bGridLines;
	m_wndControl.EnableGridLines(m_bGridLines);
}

#undef PRESET
