#include "stdafx.h"
#include "Component.TreePropList.h"
#include "Facility.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif



#define PRESET PresetTreePropList

namespace PresetTreePropList
{
	const int Id = WM_USER;
	const int TreeId = WM_USER + 1;
	const int PropListId = WM_USER + 2;

	int TreeWidth()
	{
		return globalUtils.ScaleByDPI(200);
	}

	CSize Padding()
	{
		return globalUtils.ScaleByDPI(CSize(3, 3));
	}
}



using namespace Component;

BEGIN_MESSAGE_MAP(TreePropList, CWnd)
	ON_WM_SIZE()
END_MESSAGE_MAP()



Component::TreePropList::TreePropList()
{
}



Component::TreePropList::~TreePropList()
{
}



bool Component::TreePropList::Initialize(CWnd* pParentWnd)
{
	if (__super::Create(nullptr, L"", WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN, {}, pParentWnd, PRESET::Id) == FALSE) {
		return false;
	}

	CreateTreeCtrl();
	CreatePropList();

	return true;
}



void Component::TreePropList::PostNcDestroy()
{
	__super::PostNcDestroy();
}



void Component::TreePropList::OnSize(UINT nType, int cx, int cy)
{
	__super::OnSize(nType, cx, cy);

	if (cx > 0 && cy > 0) {
		int width = PRESET::TreeWidth();
		CSize padding = PRESET::Padding();
		cx -= padding.cx * 2;
		cy -= padding.cy * 2;

		m_wndTree.SetWindowPos(nullptr, padding.cx, padding.cy, width, cy, 0);
		m_wndPropList.SetWindowPos(nullptr, padding.cx + width, padding.cy, cx - width, cy, 0);
	}
}



void Component::TreePropList::CreateTreeCtrl()
{
	DWORD dwStyle = WS_CHILD | WS_VISIBLE |
		TVS_HASLINES | TVS_TRACKSELECT | TVS_LINESATROOT | TVS_HASBUTTONS |
		TVS_SHOWSELALWAYS | TVS_FULLROWSELECT;
	if (m_wndTree.Create(dwStyle, {}, this, PRESET::TreeId) == FALSE) {
		DEBUG_RETURN;
	}

	m_wndTree.SetVisualManagerColorTheme();
	m_wndTree.EnableColumnAutoSize();
	m_wndTree.EnableAlternateRows(FALSE);
	m_wndTree.EnableGridLines(FALSE);
	m_wndTree.ModifyStyle(TVS_TRACKSELECT, 0);
	m_wndTree.SetCustomRowHeight(TreeRowHeight());
	m_wndTree.SetSingleSel(TRUE);

	//:WARNING - do not use local string
	BCGP_GRID_FILTERBAR_OPTIONS filter(m_sFilterMessage = Facility::Local(L"Search models...|모델 검색..."));
	filter.m_clrMarkBackground = (COLORREF)EColor::White;
	filter.m_clrMarkText = 0;
	filter.m_bAutoExpandGroups = TRUE;
	filter.m_bIncludeGroups = TRUE;

	m_wndTree.SetOutOfFilterLabel(Facility::Local(L"No items match your search.|일치하는 항목을 찾을 수 없습니다."));
	m_wndTree.EnableFilterBar(TRUE, filter);
	m_wndTree.OnFilterBarUpdate(0);
}



void Component::TreePropList::CreatePropList()
{
	DWORD dwStyle = WS_VISIBLE | WS_CHILD;
	if (m_wndPropList.Create(dwStyle, {}, this, PRESET::PropListId) == FALSE) {
		DEBUG_RETURN;
	}

	m_wndPropList.EnableToolBar();
	m_wndPropList.EnableSearchBox();
	m_wndPropList.EnableHeaderCtrl(FALSE);
	m_wndPropList.EnableDescriptionArea();
	m_wndPropList.EnableContextMenu();

	m_wndPropList.MarkModifiedProperties();
	m_wndPropList.SetVSDotNetLook();
	m_wndPropList.SetGroupNameFullWidth();

	m_wndPropList.SetRowPadding(PropListRowPadding());
}
