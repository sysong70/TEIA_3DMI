#pragma once

#include "Ast.h"

//--------------------------------------------------------------------------------------------------

class AstAppResources
{
public:

	Json::Object UI; // Root UI value
	Json::Object Dialogs;
	/*
	* key, gorup(1), code(Ast::CodeLength), eng, kor
	*
	* spectial key(first char) - Common, Error, Keyword, Message
	* group - common(1), error(2), message(3), title(4), description(5), parameter(6), command keyword(7)
	* id - key.group|code
	* ex) C.0001, FileOptions.501
	*/
	Json::Object Dictionary;
	Json::Object Tasks;
	// default value (not UI)
	Json::Object Preferences;
	// default value (not UI)
	Json::Object FileOptions;
	HBITMAP BkHandle = nullptr;

public:

	~AstAppResources();

	bool Load();

public:

	Json::Object& GetDialog(CStringA name);

	Json::Object& GetStyles();
	// Get from the dictionary
	CString GetStringFrom(Json::Value* pValue, CString dictionary = L"");

	Json::Object& GetTask(CStringA name);

public:

	bool Initialize();

	bool InitDialogs();

	bool InitDictionary();

	bool InitTasks();

	bool InitImages();

	bool InitFileOptions();

	bool InitPreferences();
};



extern AstAppResources TheAppResources;
