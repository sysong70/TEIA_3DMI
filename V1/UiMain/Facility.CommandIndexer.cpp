#include "stdafx.h"
#include "Facility.CommandIndexer.h"
#include "Command.VisualEffects3d.h"
#include "Command.Circle2d.h"
#include "Command.Line2d.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//**************************************************************************************************
// Global single instance

Facility::CommandIndexer TheCommandIndexer;
Facility::ActiveCommand TheActiveCommand;

Facility::CommandIndexer::CommandInfo theDummy;

//**************************************************************************************************

Facility::CommandIndexer::CommandIndexer()
{
	Initialize();
}



Facility::CommandIndexer::~CommandIndexer()
{
	CommandMap& map = GetInstance();
	CommandMap::iterator it;

	for (it = map.begin(); it != map.end(); it++) {
		REMOVE_POINTER(it->second.Function);
	}
}



Facility::CommandIndexer::CommandInfo& Facility::CommandIndexer::Get(int id)
{
	CommandMap& map = GetInstance();

	auto& it = map.find(id);
	if (it != map.end()) {
		return it->second;
	}
	else {
		DEBUG_STOP;
		return theDummy;
	}
}



Facility::CommandIndexer::CommandInfo& Facility::CommandIndexer::GetDummyData()
{
	return theDummy;
}



bool Facility::CommandIndexer::Initialize()
{
#define ITEM_DEF(type,id,stringId) { id, { type, id, -1, stringId } },

	m_commandMap = {
#include "Command.Common.h"
	};

#undef ITEM_DEF

	Get(HOME_3D_LST_VisualEffects).Function = new Command::VisualEffects3d();

	//Get(DRAW_2D_CMD_Line).Function = new Command::Line2d::TwoPoints();
	//Get(DRAW_2D_CMD_Polyline).Function = new Command::Line2d::Polyline();
	//Get(DRAW_2D_CMD_Rectangle).Function = new Command::Line2d::Rectangle();
	//Get(DRAW_2D_CMD_Polygon).Function = new Command::Line2d::Polygon();
	//Get(DRAW_2D_CMD_Circle_2Points).Function = new Command::Circle2d::TwoPoints();
	//Get(DRAW_2D_CMD_Circle_3Points).Function = new Command::Circle2d::ThreePoints();
	//Get(DRAW_2D_CMD_Circle_CenterRadius).Function = new Command::Circle2d::CenterRadius();
	//Get(DRAW_2D_CMD_Circle_2TangentsRadius).Function = new Command::Circle2d::TwoTangentsRadius();

	return true;
}



Facility::CommandIndexer::CommandMap& Facility::CommandIndexer::GetInstance()
{
	if (m_commandMap.size() == 0) {
		Initialize();
	}

	return m_commandMap;
}
