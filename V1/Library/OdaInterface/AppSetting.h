#pragma once

#include "Ge/GePoint3d.h"
#include <unordered_map>
#include <variant>

//--------------------------------------------------------------------------------------------------

#define AstBoolean(x)	TheAppSetting.GetBoolean(AppSetting::EValue::##x)
#define AstDouble(x)	TheAppSetting.GetDouble(AppSetting::EValue::##x)
#define AstInteger(x)	TheAppSetting.GetInteger(AppSetting::EValue::##x)
#define AstPoint(x)		TheAppSetting.GetPoint(AppSetting::EValue::##x)
#define AstString(x)	TheAppSetting.GetString(AppSetting::EValue::##x)

//--------------------------------------------------------------------------------------------------

class AppSetting
{
public:

	using VariantValue = std::variant<bool, int, double, CString, OdGePoint3d>;

	enum class EValue
	{
		Unknown = 0,

		UseThreadIo,
		UseRubberBand,
		UseDynamicInput,
	};

	std::unordered_map<EValue, VariantValue> Buffer;

	AppSetting();

	~AppSetting();

public:

	bool GetBoolean(EValue key) { return std::get<bool>(Buffer[key]); }

	double GetDouble(EValue key) { return std::get<double>(Buffer[key]); }

	int GetInteger(EValue key) { return std::get<int>(Buffer[key]); }

	OdGePoint3d GetPoint(EValue key) { return std::get<OdGePoint3d>(Buffer[key]); }

	CString GetString(EValue key) { return std::get<CString>(Buffer[key]); }
};



extern AppSetting TheAppSetting;
