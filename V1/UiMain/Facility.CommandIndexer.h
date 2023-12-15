#pragma once

#include "Facility.h"
#include "Command.Resource.h"
#include <unordered_map>



namespace Facility
{
	class CommandIndexer
	{
	public:

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
			int Id = -1;				// File_3D_CMD_New
			CString StringId;			// L"File_3D_CMD_New"
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
