#pragma once

#include <Json.h>

#ifndef INITIALIZE_A3D_API
#include <A3DSDKIncludes.h>
#endif

namespace H3DX
{
	// root of Json using class

	class JsonWrapper
	{
	public:

		JsonWrapper();

		virtual ~JsonWrapper();

		virtual bool Set(Json::Object& value);

	protected:

		Json::Object m_root;
	};



	class ImportOptions : public JsonWrapper
	{
	public:

		ImportOptions();

		virtual ~ImportOptions();

	public:

		bool Set(Json::Object& value) override;

		bool SetReference(Json::Object& value);

		bool Get(CStringA fileTypeName, A3DRWParamsLoadData& param);

	protected:

		bool GetGeneral(Json::Object& source, A3DRWParamsLoadData& target);

		bool GetTessellation(Json::Object& source, A3DRWParamsLoadData& target);

		bool GetSearch(Json::Object& source, A3DRWParamsLoadData& target);

		bool GetPMI(Json::Object& source, A3DRWParamsLoadData& target);

		bool GetSpecial(Json::Object& source, A3DRWParamsLoadData& target);

	private:

		CStringA m_activeType;
		Json::Object m_reference;

		bool IsValidFileTypeName(CStringA name);

		CStringA GetFileTypeName(CString filePath);

		A3DGraphRgbColorData ToHoopsColor(CString value);
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
		bool Get(CStringA fileTypeName, A3DRWParamsExportPrcData& param) {
			return true;
		}

	private:

		CStringA m_activeType;
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

extern H3DX::FileOptions TheFileOptions;
