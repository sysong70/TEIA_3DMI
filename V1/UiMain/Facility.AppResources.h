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

		HBITMAP GetBackground();

		Json::Object& GetDescription();

		Json::Object& GetDialog(CStringA name);
		// default value
		Json::Object& GetPreferences();
		// default value
		Json::Object& GetFileOptions();

		Json::Object& GetStyles();

		Json::Object& GetTask(CStringA name);

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
