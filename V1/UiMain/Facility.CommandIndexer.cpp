#include "stdafx.h"
#include "Facility.CommandIndexer.h"
#include "Command.Resource.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif



Facility::CommandIndexer TheCommandIndexer;
Facility::CommandIndexer::Command theDummy;



Facility::CommandIndexer::CommandIndexer()
{
	Initialize();
}



Facility::CommandIndexer::Command& Facility::CommandIndexer::Get(int id)
{
	CommandMap& map = GetInstance();

	auto& it = map.find(id);
	if (it != map.end()) {
		return it->second;
	}
	else {
		ASSERT(FALSE);
		return theDummy;
	}
}



Facility::CommandIndexer::Command& Facility::CommandIndexer::GetDummyData()
{
	return theDummy;
}



void Facility::CommandIndexer::Initialize()
{
#define ITEM_DEF(type,id,stringId) { id, { type, false, id, stringId } },

	m_commandMap = {
#include "Command.Common.h"
	};

#undef ITEM_DEF

	Get(HOME_3D_POP_ObjectSnap).Local = true;
}



Facility::CommandIndexer::CommandMap& Facility::CommandIndexer::GetInstance()
{
	if (m_commandMap.size() == 0) {
		Initialize();
	}

	return m_commandMap;
}
