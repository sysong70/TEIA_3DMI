#include "StdAfx.h"

#include <vhash.h>
#include <vlist.h>

#include "3DF.Selection.h"

#define		SEGMENT_TYPE		1
#define		ENTITY_TYPE			2
#define		SUBENTITY_TYPE		3
#define		REGION_TYPE			4

USING_3DF_NAMESPACE

SelectionControl::SelectionControl(HBaseView * pcView, bool bReferenceSelection) :
	HSelectionSet(pcView, bReferenceSelection)
{
	m_nSelectLevel = SEGMENT_TYPE;
	m_pcSelection = nullptr;
	m_bShowFacesAsLines = false;
	//SetAllowEntitySelection(false);
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

void SelectionControl::Select(HC_KEY key, int num_include_keys, HC_KEY * include_keys, bool emit_message)
{
	char	keyType[MVO_BUFFER_SIZE];

	if (!m_pView->GetEmitMessageFunction() || emit_message)	//disregard segment level setting if messsage
		//from other client			
	{
		if (!GetAllowEntitySelection()) // nonzero if NOT a segment
		{
			// the key is to a geometric entity.  If we are in segment selection mode,
			// then we need to get the key to its parent segment.

			HC_Show_Key_Type(key, keyType);

			if (!streq("segment", keyType))
			{
				char segname[MVO_BUFFER_SIZE];
				HC_KEY segkey;

				segkey = HC_KShow_Owner_Original_Key(key);
				HC_Show_Owner_By_Key(key, segname);

				// climb up one more level if this is the temporary highlight key
				if (IsHighlightSegment(segkey))
				{
					segkey = HC_KShow_Owner_Original_Key(segkey);
					HC_Show_Owner_By_Key(segkey, segname);
				}
				key = segkey;
			}
		}
	}

	HSelectionSet::Select(key, num_include_keys, include_keys, emit_message);
}