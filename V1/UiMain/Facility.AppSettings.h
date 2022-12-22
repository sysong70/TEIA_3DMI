#pragma once

#include "Facility.Preference.h"
#include "Facility.FileOption.h"



namespace Facility
{
	class AppSettings
	{
	public:

		~AppSettings();

		void SetFolderPath(CString c);

		bool Load();

		bool Save();

		Facility::Preference Preference;

		Facility::ImportOption* GetImportOption(CString name);

	private:

		CString GetFilePath();

		Json::Object* GetImportOptions();

		bool SetImportOptions(Json::Object* pData);

	private:

		CString m_sFolderPath;

		static Facility::ImportOption* m_pImportOptionDefault;

		std::vector<Facility::ImportOption*> m_importOptions;
	};
}

extern Facility::AppSettings TheAppSettings;
