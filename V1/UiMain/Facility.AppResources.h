#pragma once

#include "Facility.h"
#include <Json.h>



namespace Facility
{
	class AppResources
	{
	public:

		~AppResources();

		bool Load();

		Json::Object& GetDialog(CStringA name);

		Json::Object& GetPreferences();

		Json::Object& GetFileOptions();

		Json::Object& GetStyles();

		HBITMAP GetBackground();

	private:

		Json::Object m_ui; // Root value
		Json::Object m_preferences; // default value
		Json::Object m_fileOptions; // default value
		HBITMAP m_background = nullptr;

		bool Initialize();

		bool InitPreferences();

		bool InitFileOptions();

		bool InitImages();
	};
}

extern Facility::AppResources TheAppResources;
