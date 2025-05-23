#include "stdafx.h"

#include "Cmd.Circle2d.h"
#include "Cmd.Indexer.h"
#include "Cmd.Line2d.h"
#include "Cmd.VisualEffects3d.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//**************************************************************************************************
// Global single instance

CmdIndexer TheCmdIndexer;
CmdActive TheCmdActive;

CmdIndexer::CommandInfo theDummy;

//**************************************************************************************************

CmdIndexer::CmdIndexer()
{
	Initialize();
}



CmdIndexer::~CmdIndexer()
{
	CommandMap& map = GetInstance();
	CommandMap::iterator it;

	for (it = map.begin(); it != map.end(); it++) {
		// ASSERT(it->second.Function);
		if(nullptr != it->second.Function) {
			REMOVE_POINTER(it->second.Function);
		}
	}
}



CmdIndexer::CommandInfo& CmdIndexer::Get(int id)
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



CmdIndexer::CommandInfo& CmdIndexer::GetDummyData()
{
	return theDummy;
}



bool CmdIndexer::Initialize()
{
#define ITEM_DEF(type,id,stringId) { id, { type, id, -1, stringId } },

	Instance = {
#include "Cmd.Common.h"
	};

#undef ITEM_DEF

	Get(HOME_3D_LST_VisualEffects).Function = new CmdVisualEffects3d();

	//Get(DRAW_2D_CMD_Line).Function = new CmdLine2dTwoPoints();
	//Get(DRAW_2D_CMD_Polyline).Function = new CmdLine2dPolyline();
	//Get(DRAW_2D_CMD_Rectangle).Function = new CmdLine2dRectangle();
	//Get(DRAW_2D_CMD_Polygon).Function = new CmdLine2dPolygon();
	//Get(DRAW_2D_CMD_Circle_2Points).Function = new CmdCircle2dTwoPoints();
	//Get(DRAW_2D_CMD_Circle_3Points).Function = new CmdCircle2dThreePoints();
	//Get(DRAW_2D_CMD_Circle_CenterRadius).Function = new CmdCircle2dCenterRadius();
	//Get(DRAW_2D_CMD_Circle_2TangentsRadius).Function = new CmdCircle2dTwoTangentsRadius();

	return true;
}



CmdIndexer::CommandMap& CmdIndexer::GetInstance()
{
	if (Instance.size() == 0) {
		Initialize();
	}

	return Instance;
}
