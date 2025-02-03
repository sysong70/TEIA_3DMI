#pragma once

#include "Facility.h"

//--------------------------------------------------------------------------------------------------

namespace Facility
{
	class AppResources
	{
		Json::Object m_ui; // Root UI value
		Json::Object m_dialogs;
		Json::Object m_dictionary;
		Json::Object m_tasks;

		Json::Object m_preferences;
		Json::Object m_fileOptions;
		HBITMAP m_background = nullptr;

	public:

		~AppResources();

		bool Load();

	public:

		HBITMAP GetBackground();

		Json::Object& GetDialog(CStringA name);
		/*
		* key, gorup(1), code(Facility::CodeLength), eng, kor
		* 
		* spectial key(first char) - Common, Error, Keyword, Message
		* group - common(1), error(2), message(3), title(4), description(5), parameter(6), command keyword(7)
		* id - key.group|code
		* ex) C.0001, FileOptions.501
		*/
		Json::Object& GetDictionary();

		Json::Object& GetStyles();
		// Get from the dictionary
		CString GetStringFrom(Json::Value* pValue, CString dictionary = L"");

		Json::Object& GetTask(CStringA name);

		// default value (not UI)
		Json::Object& GetFileOptions();
		// default value (not UI)
		Json::Object& GetPreferences();

	private:

		bool Initialize();

		bool InitDialogs();

		bool InitDictionary();

		bool InitTasks();

		bool InitImages();

		bool InitFileOptions();

		bool InitPreferences();
	};
}

extern Facility::AppResources TheAppResources;
