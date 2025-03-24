#pragma once

#include "Ast.h"

//--------------------------------------------------------------------------------------------------

class AstAppOptions
{
public:

	CString FolderPath;
	Json::Object Preferences;
	Json::Object FileOptions;

public:

	AstAppOptions() {}

	~AstAppOptions() {}

	void SetFolderPath(CString c);

	bool Load();

	bool Save();

public: // Preference Values

	bool GetBoolean(const char* path);

	COLORREF GetColor(const char* path);

	int GetInteger(const char* path);
};



extern AstAppOptions TheAppOptions;
