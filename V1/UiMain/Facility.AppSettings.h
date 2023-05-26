#pragma once

#include "Facility.h"



namespace Facility
{
	class AppSettings
	{
	public:

		AppSettings();

		~AppSettings();

		void SetFolderPath(CString c);

		Json::Object& GetPreferences();

		Json::Object& GetFileOptions();

		bool Load();

		bool Save();

	private:

 		CString m_sFolderPath;

		Json::Object m_preferences;
		Json::Object m_fileOptions;
	};
}

extern Facility::AppSettings TheAppSettings;
