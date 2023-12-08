#pragma once

#include "Facility.h"
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
}

extern Facility::CommandIndexer TheCommandIndexer;
