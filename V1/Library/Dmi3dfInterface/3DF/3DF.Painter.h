#pragma once

#include <vector>
#include <atlstr.h>

#include "3DF.Math.h"
#include "3DF.Math.Matrix.h"
#include "3DF.Point.h"

class HBaseView;

namespace TDF
{
	namespace Painter
	{
		using Points = std::vector<TDF::Point>;

		void SetView(HBaseView* view);

		void SetColor(const char* type, const char* space, double abc[3]);



		namespace Arc
		{
			// 3 Points
			void Create(TDF::Point first, TDF::Point second, TDF::Point third);
			// angle: degree
			void GetPoints(TDF::Point center, double radius, double startAngle, double endAngle, Points& points);

			void GetPoints(float x, float y, double radius, double startAngle, double endAngle, Points& points);
		};



		namespace Circle
		{
			// Center, Radius
			void Create(TDF::Point center, double radius, bool polygon = true);
			// 3 Points
			void Create(TDF::Point first, TDF::Point second, TDF::Point third, bool polygon = true);

			void GetPoints(TDF::Point center, double radius, bool reverse, Points& points);
		};



		namespace Compute
		{
			double Distance(TDF::Point p1, TDF::Point p2);

			double PixelToWorld(double value);

			TDF::Vector Normal();
		}



		namespace Figure
		{
			void CreateDonut(TDF::Point center, double inner, double outer);
			// Left & Right Half Circle
			void CreateObround(TDF::Point topLeft, TDF::Point bottomRight);

			void CreateRectangle(TDF::Point topLeft, TDF::Point bottomRight);
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

			enum class EPivot
			{
				TopLeft,
				TopCenter,
				TopRight,
				MiddleLeft,
				MiddleCenter,
				MiddleRight,
				BottomLeft,
				BottomCenter,
				BottomRight,
			};

			void SetAlignment(const char* value);
			void SetAlignment(EPivot value);
			// bold = [on | off]
			void SetBold(bool value = true);
			// name = arial
			void SetName(const char* value);
			// size = xx [oru | sru | wru | wsu | points | pt | pixels | px]
			void SetSize(double size, const char* unit);
			// renderer = [default | driver | truetype | defined]
			void SetRenderer(const char* value);

			void SetRotation(double degree);
			// transforms = [on | off | character position only | character position adjusted]
			void SetTransform(bool value = true);
			void SetTransform(const char* value);
		}



		namespace Line
		{
			void Create(TDF::Point first, TDF::Point second, bool firstEnd = false, bool secondEnd = false);
		};



		namespace Polyline
		{
			void Create(Points& points);
		};



		namespace Polygon
		{
			void Create(Points& points);
		};



		namespace Segment
		{
			void SetColor(const char* type, COLORREF color);
			void SetColor(const char* type, COLORREF color, double alpha);

			void SetEdgePattern(const char* value);

			void SetEdgeWeight(double value);

			void SetEdgeType(double weight, const char* pattern, COLORREF color);

			void SetLinePattern(const char* value, const char* prefix = nullptr, const char* suffix = nullptr);

			void SetLineWeight(double value);

			void SetLineType(double weight, const char* pattern, COLORREF color);

			void SetVisibility(const char* type, bool value = true);
			void SetVisibility(const char* option, const char* sub = nullptr, bool value = true);
		};



		namespace Text
		{
			void Create(TDF::Point center, const char* value);

			void GetExtent(const char* value, float& width, float& height);
		};
	};
};
