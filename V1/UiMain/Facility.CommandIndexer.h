#pragma once

#include "Facility.h"
#include <unordered_map>



namespace Facility
{
	class CommandIndexer
	{
	public:

		enum ItemType
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

		struct Command
		{
			ItemType Type = Unknown;
			bool Local = false;      // not send command to renderer
			int Id = -1;             // File_3D_CMD_New
			CString StringId;        // L"File_3D_CMD_New"
		};

		CommandIndexer();

		Command& Get(int id);

		Command& GetDummyData();

	private:

		void Initialize();

		typedef std::unordered_map<int, Command> CommandMap;

		CommandMap& GetInstance();

		CommandMap m_commandMap;
	};
}

extern Facility::CommandIndexer TheCommandIndexer;
