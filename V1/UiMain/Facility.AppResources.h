#pragma once

#include "Facility.h"
#include "Json.h"



namespace Facility
{
	class AppResources
	{
	public:

		~AppResources();

		bool Load();

		Json::Object& GetDialog(CStringA name);

		Json::Object& GetFileOptions();

		Json::Object& GetPreferences();

	private:

		Json::Object m_fileOptions; // default value
		Json::Object m_preferences; // default value
		Json::Object m_ui;

		bool InitDialog();

		bool InitFileOptions();

		bool InitPreferences();
	};
}

extern Facility::AppResources TheAppResources;
