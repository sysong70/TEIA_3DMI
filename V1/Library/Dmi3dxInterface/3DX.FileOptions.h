#pragma once

#include <Json.h>

#include "3DX.h"

#ifndef INITIALIZE_A3D_API
#	include <A3DSDKIncludes.h>
#endif

namespace H3DX
{
	// root of Json using class

	class API_3DX JsonWrapper
	{
	public:

		JsonWrapper();

		virtual ~JsonWrapper();
		// from Signal
		bool SetRootObject(const wchar_t* pSource);

	protected:

		Json::Object m_root;
	};

	class API_3DX FileOptions : public JsonWrapper
	{
	public:

		FileOptions();

		virtual ~FileOptions();

	public: // Import options

		//:WARNING - use A3D_INITIALIZE_DATA before call function
		// call GetImport("3MF")
		bool GetImport(CStringA fileTypeName, A3DRWParamsLoadData& param);

	protected:

		bool GetGeneral(Json::Object& source, A3DRWParamsLoadData& target);

		bool GetTessellation(Json::Object& source, A3DRWParamsLoadData& target);

		bool GetSearch(Json::Object& source, A3DRWParamsLoadData& target);

		bool GetPMI(Json::Object& source, A3DRWParamsLoadData& target);

		bool GetSpecial(Json::Object& source, A3DRWParamsLoadData& target);

	private:

		bool IsValidFileTypeName(CStringA name);

		CStringA m_activeType;
	};
}

extern API_3DX H3DX::FileOptions TheFileOptions;
