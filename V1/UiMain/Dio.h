#pragma once

//--------------------------------------------------------------------------------------------------

namespace Dio
{
	enum class EControlId
	{
		Unkown = WM_USER,

		Command,
		Length,
		Angle,
		Prompt,
		CoordX,
		CoordY,
		CoordZ,
	};

	enum class EState
	{
		Unchanged,
		Editing,
		Modified,
		Error,
	};

	enum class EValue
	{
		Unknown,

		Point,
		Length,
		Angle,
		Real,
		String,
	};

	enum ECharMask
	{
		eAllowNone = 0,
		eAllowLessThen = 1,				// <
		eAllowAt = eAllowLessThen * 2,	// @
		eAllowHash = eAllowAt * 2,		// #
		eAllowDot = eAllowHash * 2,		// .
		eAllowComma = eAllowDot * 2,	// ,
		eAllowUnder = eAllowComma * 2,	// _
		eAllowMinus = eAllowUnder * 2,	// -
		eAllowNumber = eAllowMinus * 2,
		eAllowAlpha = eAllowNumber * 2,
		eAllowSpace = eAllowAlpha * 2,

		eAllowCommand = eAllowNone
		| eAllowNumber
		| eAllowAlpha
		,
		eAllowLength = eAllowNone
		| eAllowNumber
		| eAllowDot
		,
		eAllowAngle = eAllowNone
		| eAllowMinus
		| eAllowNumber
		| eAllowDot
		,
		eAllowAll = eAllowNone
		| eAllowLessThen
		| eAllowAt
		| eAllowHash
		| eAllowDot
		| eAllowNumber
		| eAllowComma
		| eAllowNumber
		| eAllowAlpha
		| eAllowSpace
		,
		eAllowReal = eAllowNone
		| eAllowLessThen
		| eAllowAt
		| eAllowHash
		| eAllowDot
		| eAllowNumber
		| eAllowComma
		,
	};

	enum ESkipMask
	{
		eSkipNone = 0,
		eSkipTab = 1,		// \t
		eSkipLessThen = 2,	// <
		eSkipComma = 4,		// ,

		eSkipCoord = eSkipLessThen | eSkipComma,
		eSkipAll = eSkipTab | eSkipLessThen | eSkipComma,
	};

	enum EEnterMask
	{
		eEnterNone = 0,
		eEnterReturn = 1,
		eEnterSpace = 2,

		eEnterAll = eEnterReturn | eEnterSpace,
	};
}



struct DynamicIoParams
{
	Dio::EControlId Id = Dio::EControlId::Unkown;
	Dio::EState State = Dio::EState::Unchanged;
	Dio::EValue Value = Dio::EValue::Unknown;

	bool Activated = false;
	bool Editable = false;
	bool Nullable = false;
	bool VisibleWait = false;
	CString Text;
};
