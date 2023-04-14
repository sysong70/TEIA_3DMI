#pragma once

#include "HTools.h"
#include <vector>
#include <atlstr.h>

#include "3DF.Math.h"
#include "3DF.Math.Matrix.h"

class HBaseView;
class HEventInfo;

namespace HDraw
{
	void Test(HBaseView* view, TDF::Math::Matrix & cMatrix, HPoint p1, HPoint p2, TDF::Point op1, TDF::Point op2);

	class Format
	{
	public:

		Format(const char* format, ...)
		{
			va_list ap;

			va_start(ap, format);
			buffer.FormatV(format, ap);
			va_end(ap);
		}

		operator char const* () const
		{
			return buffer;
		}

	private:

		CStringA buffer;
	};



	using HPoints = std::vector<HPoint>;

	void SetView(HBaseView* view);



	namespace Arc
	{
		// 3 Points
		void Create(HPoint first, HPoint second, HPoint third);
		// angle: degree
		void GetPoints(HPoint center, double radius, double startAngle, double endAngle, HPoints& points);

		void GetPoints(float x, float y, double radius, double startAngle, double endAngle, HPoints& points);
	};



	namespace Circle
	{
		// Center, Radius
		void Create(HPoint center, double radius, bool polygon = true);
		// 3 Points
		void Create(HPoint first, HPoint second, HPoint third, bool polygon = true);
	};



	namespace Compute
	{
		double Distance(HPoint p1, HPoint p2);

		double PixelToWorld(double value);

		HVector Normal();
	}



	namespace Figure
	{
		// Left & Right Half Circle
		void CreateObround(HPoint topLeft, HPoint bottomRight);
	};

	//:REF - https://docs.techsoft3d.com/3df/latest/api_ref/3dgs/HC_Set_Text_Font.html

	namespace Font
	{
		enum class ERenderer
		{
			Default,
			Driver,
			TrueType,
			Defined,
		};

		// bold = [on | off]
		void SetBold(bool value = true);
		// name = arial
		void SetName(const char* value);
		// size = xx [oru | sru | wru | wsu | points | pt | pixels | px]
		void SetSize(double size, const char* unit);
		// renderer = [default | driver | truetype | defined]
		void SetRenderer(const char* value);
		// transforms = [on | off]
		void SetTransform(bool value = true);
	}



	namespace Line
	{
		void Create(HPoint first, HPoint second, bool firstEnd = false, bool secondEnd = false);
	};



	namespace Polyline
	{
		void Create(HPoints& points);
	};



	namespace Polygon
	{
		void Create(HPoints& points);
	};



	namespace Text
	{
		void Create(HPoint center, const char* value);

		void GetExtent(const char* value, float& width, float& height);
	};
};
