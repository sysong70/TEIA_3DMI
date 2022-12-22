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

		bool Load(CString stream);

		Json::Object& GetDialog(CStringA name);
	#ifdef _DEBUG
		Json::Object& GetDebug();
	#endif

	private:

		Json::Object m_data;

		Json::Object* m_pDialog;
	#ifdef _DEBUG
		Json::Object* m_pDebug;
	#endif
	};
}

extern Facility::AppResources TheAppResources;
