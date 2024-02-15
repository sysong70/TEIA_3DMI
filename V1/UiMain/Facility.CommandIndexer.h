#pragma once

#include "Facility.h"
#include "Command.Resource.h"

//--------------------------------------------------------------------------------------------------

namespace Facility
{
	class CommandIndexer
	{
	public:

		//:WARNING - do not use enum class
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

			int Id = -1;				// HOME_3D_LST_Zoom
			int ChildId = -1;			// HOME_3D_CMD_Zoom_Fit
			CString StringId;			// L"HOME_3D_LST_Zoom"
			void* Function = nullptr;	// Target function
		};

		CommandIndexer();

		~CommandIndexer();

		CommandInfo& Get(int id);

		CommandInfo& GetDummyData();

	private:

		void Initialize();

		using CommandMap = std::unordered_map<int, CommandInfo>;

		CommandMap& GetInstance();

		CommandMap m_commandMap;
	};



	class ActiveCommand
	{
	public:

		struct
		{
			UINT VisualEffects3d = HOME_3D_CMD_ViewStyle_ShadeWithEdges;
		}
		Home;

		//:TODO
		struct
		{
			UINT Angle3d = 0;
			UINT Coordinate3d = 0;
			UINT Distance3d = 0;
			UINT Length = 0;
			UINT Radius = 0;
		}
		Measure;
	};
}

extern Facility::CommandIndexer TheCommandIndexer;
extern Facility::ActiveCommand TheActiveCommand;
