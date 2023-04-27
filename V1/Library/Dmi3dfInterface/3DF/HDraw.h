#pragma once

#include "HTools.h"
#include <vector>
#include <atlstr.h>

#include "3DF.Math.h"
#include "3DF.Math.Matrix.h"
#include "3DF.Point.h"

class HBaseView;
class HEventInfo;

namespace HDraw
{
	void DrawSnapPoint(HBaseView * view, TDF::Matrix & cMatrix, TDF::Point2D cPo);

	void Test(HBaseView* view, TDF::Matrix & cMatrix, TDF::Point2D p1, TDF::Point2D p2);

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



	using HPoints = std::vector<TDF::Point>;

	void SetView(HBaseView* view);



	namespace Arc
	{
		// 3 Points
		void Create(TDF::Point first, TDF::Point second, TDF::Point third);
		// angle: degree
		void GetPoints(TDF::Point center, double radius, double startAngle, double endAngle, HPoints& points);

		void GetPoints(float x, float y, double radius, double startAngle, double endAngle, HPoints& points);
	};



	namespace Circle
	{
		// Center, Radius
		void Create(TDF::Point center, double radius, bool polygon = true);
		// 3 Points
		void Create(TDF::Point first, TDF::Point second, TDF::Point third, bool polygon = true);
	};



	namespace Compute
	{
		double Distance(TDF::Point p1, TDF::Point p2);

		double PixelToWorld(double value);

		TDF::Vector Normal();
	}



	namespace Figure
	{
		// Left & Right Half Circle
		void CreateObround(TDF::Point topLeft, TDF::Point bottomRight);
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
		void Create(TDF::Point first, TDF::Point second, bool firstEnd = false, bool secondEnd = false);
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
		void Create(TDF::Point center, const char* value);

		void GetExtent(const char* value, float& width, float& height);
	};
};
