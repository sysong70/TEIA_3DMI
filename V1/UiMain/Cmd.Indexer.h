#pragma once

#include "Cmd.h"

class CmdBase;

//--------------------------------------------------------------------------------------------------

class CmdIndexer
{
public:

	// WARNING - do not use enum class
	enum EItemType
	{
		Unknown,
		Button,
		Category,
		Check,
		List,
		ListItem,
		Main,
		MainButton,
		Panel,
		Popup,
	};

	struct CommandInfo
	{
		EItemType Type = Unknown;

		int Id = -1;					// HOME_3D_LST_Zoom
		int ChildId = -1;				// HOME_3D_CMD_Zoom_Fit
		CString StringId;				// L"HOME_3D_LST_Zoom"
		CmdBase* Function = nullptr;	// Target function
	};

	CmdIndexer();

	~CmdIndexer();

	CommandInfo& Get(int id);

	CommandInfo& GetDummyData();

public:

	bool Initialize();

	using CommandMap = std::unordered_map<int, CommandInfo>;

	CommandMap& GetInstance();

	CommandMap Instance;
};

//--------------------------------------------------------------------------------------------------

class CmdActive
{
public:

	struct
	{
		UINT VisualEffects3d = HOME_3D_CMD_ViewStyle_ShadeWithEdges;
	}
	Home;

	struct
	{
		UINT Angle3d = 0;
		UINT Coordinate3d = 0;
		UINT Distance3d = 0;
		UINT Length = 0;
		UINT Radius = 0;
	}
	Measure;

	struct
	{
		UINT Point2d = DRAW_2D_CMD_Point;
		UINT Line2d = DRAW_2D_CMD_Line;
		UINT Circle2d = DRAW_2D_CMD_Circle_CenterRadius;
		UINT Arc2d = DRAW_2D_CMD_Arc_3Points;
		UINT Ellipse2d = DRAW_2D_CMD_Ellipse_2Axes;
		UINT EllipticalArc2d = DRAW_2D_CMD_EllipticalArc_2Axes;
	}
	Draw;
};



extern CmdIndexer TheCmdIndexer;
extern CmdActive TheCmdActive;
