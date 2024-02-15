#include "stdafx.h"
#include "Facility.CommandIndexer.h"
#include "Command.VisualEffects3d.h"
//:TEST
#include "Command.Test.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//--------------------------------------------------------------------------------------------------
// Global single instance

Facility::CommandIndexer TheCommandIndexer;
Facility::CommandIndexer::CommandInfo theDummy;
Facility::ActiveCommand TheActiveCommand;

Command::VisualEffects3d theVisualEffects3d;
//:TEST
Command::Test9 theTestCommand9;

//--------------------------------------------------------------------------------------------------

Facility::CommandIndexer::CommandIndexer()
{
	Initialize();
}



Facility::CommandIndexer::~CommandIndexer()
{
}



Facility::CommandIndexer::CommandInfo& Facility::CommandIndexer::Get(int id)
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



Facility::CommandIndexer::CommandInfo& Facility::CommandIndexer::GetDummyData()
{
	return theDummy;
}



void Facility::CommandIndexer::Initialize()
{
#define ITEM_DEF(type,id,stringId) { id, { type, id, -1, stringId } },

	m_commandMap = {
#include "Command.Common.h"
	};

#undef ITEM_DEF

	Get(HOME_3D_LST_VisualEffects).Function = &theVisualEffects3d;
	//:TEST
	Get(CUSTOM_3D_CMD_KEN_Test9).Function = &theTestCommand9;
}



Facility::CommandIndexer::CommandMap& Facility::CommandIndexer::GetInstance()
{
	if (m_commandMap.size() == 0) {
		Initialize();
	}

	return m_commandMap;
}
