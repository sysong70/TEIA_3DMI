#include "StdAfx.h"

#include <vhash.h>
#include <vlist.h>

#include "3DF.Selection.h"

USING_3DF_NAMESPACE

//== Select 관련 Class ==============================================================================

void QualifiedKey::Select(HBaseView * view)
{
	HSelectionSet * sel = view->GetSelection();
	sel->Select(m_nKey, m_nIncludeListCount, m_pnIncludeList);
}

MultiSelectItem::MultiSelectItem(HC_KEY key)
{
	m_Key.SetKey(key);
	m_pcAssociatedKeyList = new_vlist(malloc, free);
	m_pcAssociatedSelectItemList = new_vlist(malloc, free);
}

MultiSelectItem::~MultiSelectItem()
{
	START_LIST_ITERATION(QualifiedKey, m_pcAssociatedKeyList); {
		delete temp;
	}END_LIST_ITERATION(m_pcAssociatedKeyList);
	delete_vlist(m_pcAssociatedKeyList);

	START_LIST_ITERATION(QualifiedKey, m_pcAssociatedSelectItemList); {
		delete temp;
	}END_LIST_ITERATION(m_pcAssociatedSelectItemList);
	delete_vlist(m_pcAssociatedSelectItemList);
}

void MultiSelectItem::AddAssociatedKey(HC_KEY key, int length, HC_KEY * include_list)
{
	QualifiedKey * qk = new QualifiedKey(key, length, include_list);
	vlist_add_last(m_pcAssociatedKeyList, qk);
}
void MultiSelectItem::AddAssociatedSelectItem(HC_KEY key, int length, HC_KEY * include_list)
{
	QualifiedKey * qk = new QualifiedKey(key, length, include_list);
	vlist_add_last(m_pcAssociatedSelectItemList, qk);
}
void MultiSelectItem::MultiSelect(MultiSelectManager * mm, HBaseView * view)
{
	START_LIST_ITERATION(QualifiedKey, m_pcAssociatedKeyList); {
		temp->Select(view);
	}END_LIST_ITERATION(m_pcAssociatedKeyList);
	START_LIST_ITERATION(QualifiedKey, m_pcAssociatedSelectItemList); {
		mm->MultiSelect(temp->GetKey(), view);
	}END_LIST_ITERATION(m_pcAssociatedSelectItemList);
}

MultiSelectManager::MultiSelectManager()
{
	m_pcSelectItemHash = new_vhash(10, malloc, free);
	m_pcSelectItemList = new_vlist(malloc, free);
	m_bHasItems = false;
}

MultiSelectManager::~MultiSelectManager()
{
	START_LIST_ITERATION(MultiSelectItem, m_pcSelectItemList); {
		delete temp;
	}END_LIST_ITERATION(m_pcSelectItemList);
	delete_vlist(m_pcSelectItemList);
	delete_vhash(m_pcSelectItemHash);

}

MultiSelectItem * MultiSelectManager::CreateMultiSelectItem(HC_KEY key)
{
	MultiSelectItem * item = new MultiSelectItem(key);
	vhash_insert_item(m_pcSelectItemHash, (void *) key, (void *) item);
	vlist_add_last(m_pcSelectItemList, item);
	m_bHasItems = true;
	return item;
}
MultiSelectItem * MultiSelectManager::GetMultiSelectItem(HC_KEY key)
{

	MultiSelectItem * item;
	int res = vhash_lookup_item(m_pcSelectItemHash, (void *) key, (void **) &item);
	if(!(res == VHASH_STATUS_SUCCESS))
		return 0;
	return item;
}

HC_KEY  MultiSelectManager::MultiSelect(HC_KEY key, HBaseView * view)
{
	int i;
	if(!m_bHasItems)
		return key;
	HC_KEY oldkey = key;
	for(i = 0; i < 5; i++)
	{
		MultiSelectItem * item = GetMultiSelectItem(key);
		if(item)
		{
			item->MultiSelect(this, view);
			break;
		}
		key = HC_KShow_Owner_By_Key(key);
	}
	if(i == 5)
		return oldkey;
	else
		return key;

}

SelectionControl::SelectionControl(HBaseView * pcView, bool bReferenceSelection) :
	HSelectionSet(pcView, bReferenceSelection)
{
	SetAllowEntitySelection(false);
}

SelectionControl::~SelectionControl()
{
	if(nullptr != m_pcSelection)
	{
		delete_vlist(m_pcSelection);
		m_pcSelection = nullptr;
	}
}

// create a new list  object
void SelectionControl::Init()
{
	m_pcSelection = new_vlist(malloc, free);

	HSelectionSet::Init();
}
