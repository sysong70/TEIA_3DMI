#pragma once

#include "Geometry.h"
#include <Json.h>

//--------------------------------------------------------------------------------------------------

namespace Geometry
{
	class UserDataFiler : public Json::Object
	{
	public:

		UserDataFiler();

		virtual ~UserDataFiler();

	public:

		//virtual void Get(HC_KEY target) {}

		//virtual void Set(HC_KEY target) {}

		virtual void SetType(EType type) {}

		virtual void SetId(CString id) {}

		virtual void SetName(CString name) {}

	public: // get standard type

		bool GetBoolean(int id, bool defaultValue = false);

		DWORD_PTR GetDwordPtr(int id, DWORD_PTR defaultValue = NULL);

		int GetInteger(int id, int defaultValue = 0);

		double GetReal(int id, double defaultValue = 0.0);

		CString GetString(int id, CString defaultValue = L"");

	public: // set standard type

		void SetBoolean(int id, bool value);

		void SetDwordPtr(int id, DWORD_PTR value);

		void SetInteger(int id, int value);

		void SetReal(int id, double value);

		void SetString(int id, CString value);
	};



	class Base
	{
	};




	class Line : public Base
	{
	public:

		enum EFiler
		{
			X1 = 101,
			Y1 = 102,
			Z1 = 103,
			X2 = 111,
			Y2 = 112,
			Z2 = 113,
		};
	};



	class Arc : public Base
	{
	public:

		enum EFiler
		{
			CenterX	= 101,
			CenterY	= 102,
			CenterZ	= 103,
			StartX	= 111,
			StartY	= 112,
			StartZ	= 113,
			EndX	= 121,
			EndY	= 122,
			EndZ	= 123,
			Radius	= 201,
		};
	};



	class Dimension : public Base
	{
	public:

		enum EType
		{
			Coordinate,
			Horizontal,
			Vertical,
			Align,
			Length,
			Radius,
			Diameter,
			Angle,
		};
	};



	class HorizontalDimension : public Dimension
	{
	};
};

