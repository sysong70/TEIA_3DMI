#pragma once

#include "Facility.h"



namespace Facility
{
	class AppOptions
	{
	public:

		AppOptions();

		~AppOptions();

		void SetFolderPath(CString c);

		Json::Object& GetPreferences();

		Json::Object& GetFileOptions();

		bool Load();

		bool Save();

	public: // Preference Values

		bool BooleanValue(const char* path);

	private:

 		CString m_sFolderPath;
		Json::Object m_preferences;
		Json::Object m_fileOptions;
	};
}

extern Facility::AppOptions TheAppOptions;
