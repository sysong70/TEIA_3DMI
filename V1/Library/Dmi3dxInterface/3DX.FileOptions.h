#pragma once

#include <Json.h>

#include "3DX.h"

#ifndef INITIALIZE_A3D_API
#include <A3DSDKIncludes.h>
#endif

namespace H3DX
{
	// root of Json using class

	class API_3DX JsonWrapper
	{
	public:

		JsonWrapper();

		virtual ~JsonWrapper();

		virtual bool Set(Json::Object& value);

	protected:

		Json::Object m_root;
	};

	class API_3DX FileOptions : public JsonWrapper
	{
	public:

		ImportOptions();

		virtual ~ImportOptions();

	public:

		bool Set(Json::Object& value) override;

		bool SetReference(Json::Object& value);

		bool Get(CString& filePath, A3DRWParamsLoadData& param);

	protected:

		bool GetGeneral(Json::Object& source, A3DRWParamsLoadData& target);

		bool GetTessellation(Json::Object& source, A3DRWParamsLoadData& target);

		bool GetSearch(Json::Object& source, A3DRWParamsLoadData& target);

		bool GetPMI(Json::Object& source, A3DRWParamsLoadData& target);

		bool GetSpecial(CStringA fileType, Json::Object& source, A3DRWParamsLoadData& target);

	private:

		Json::Object m_reference;

		CString GetFileTypeName(CString filePath);

		A3DGraphRgbColorData ToHoopsColor(CString value);

		A3DUTF8Char* ToHoopsString(CString value);
		// split string by ';'
		A3DUTF8Char** ToHoopsStrings(CString value);
	};

	//:TODO

	class ExportOptions : public JsonWrapper
	{
	public:

		ExportOptions();

		virtual ~ExportOptions();

	public:

		bool Set(Json::Object& value) override;

		bool SetReference(Json::Object& value);
		// A3DRWParamsExportPrcData??
		bool Get(CString& filePath, A3DRWParamsExportPrcData& param) {
			RETURN_TRUE;
		}

	private:

		Json::Object m_reference;
	};



	class FileOptions
	{
	public:
		
		FileOptions();

		~FileOptions();

		bool Set(Json::Object& value);

		bool SetReference(Json::Object& value);

		ImportOptions Import;
		ExportOptions Export;
	};
}

extern API_3DX H3DX::FileOptions TheFileOptions;
