#pragma once

#include <vector>
#include <atlstr.h>

#include "Math.h"
#include "Math.Matrix.h"
#include "Point.h"

class HBaseView;

namespace H3DF
{
	namespace Painter
	{
		using Points = std::vector<H3DF::Point>;

		void API_3DF SetView(HBaseView* view);

		void API_3DF SetColor(const char* type, const char* space, double abc[3]);

		H3DF::Point TransColor(COLORREF color);



		namespace Arc
		{
			// 3 Points
			HC_KEY API_3DF Create(H3DF::Point first, H3DF::Point second, H3DF::Point third);
			// angle: degree
			void API_3DF GetPoints(H3DF::Point center, double radius, double startAngle, double endAngle, Points& points);

			void API_3DF GetPoints(float x, float y, double radius, double startAngle, double endAngle, Points& points);
		};



		namespace Circle
		{
			// Center, Radius
			HC_KEY API_3DF Create(H3DF::Point center, double radius, bool polygon = true);
			// 3 Points
			HC_KEY API_3DF Create(H3DF::Point first, H3DF::Point second, H3DF::Point third, bool polygon = true);

			void API_3DF GetPoints(H3DF::Point center, double radius, bool reverse, Points& points);
		};



		namespace Compute
		{
			double API_3DF Distance(H3DF::Point p1, H3DF::Point p2);

			double API_3DF PixelToWorld(double value);

			H3DF::Vector Normal();
		}



		namespace Figure
		{
			HC_KEY API_3DF CreateDonut(H3DF::Point center, double inner, double outer);
			// Left & Right Half Circle
			HC_KEY API_3DF CreateObround(H3DF::Point topLeft, H3DF::Point bottomRight);

			HC_KEY API_3DF CreateRectangle(H3DF::Point topLeft, H3DF::Point bottomRight);
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

			void API_3DF SetAlignment(const char* value);
			void API_3DF SetAlignment(EPivot value);
			// bold = [on | off]
			void API_3DF SetBold(bool value = true);
			// name = arial
			void API_3DF SetName(const char* value);
			// size = xx [oru | sru | wru | wsu | points | pt | pixels | px]
			void API_3DF SetSize(double size, const char* unit);
			// renderer = [default | driver | truetype | defined]
			void API_3DF SetRenderer(const char* value);

			void API_3DF SetRotation(double degree);
			// transforms = [on | off | character position only | character position adjusted]
			void API_3DF SetTransform(bool value = true);
			void API_3DF SetTransform(const char* value);
		}



		namespace Line
		{
			HC_KEY API_3DF Create(H3DF::Point first, H3DF::Point second, bool firstEnd = false, bool secondEnd = false);
		};



		namespace Polyline
		{
			HC_KEY API_3DF Create(Points& points);
		};



		namespace Polygon
		{
			HC_KEY API_3DF Create(Points& points);
		};



		namespace Segment
		{
			void API_3DF SetColor(const char* type, COLORREF color);
			void API_3DF SetColor(const char* type, COLORREF color, double alpha);

			void API_3DF SetEdgePattern(const char* value);

			void API_3DF SetEdgeWeight(double value);

			void API_3DF SetEdgeType(double weight, const char* pattern, COLORREF color);

			void API_3DF SetLinePattern(const char* value, const char* prefix = nullptr, const char* suffix = nullptr);

			void API_3DF SetLineWeight(double value);

			void API_3DF SetLineType(double weight, const char* pattern, COLORREF color);

			void API_3DF SetVisibility(const char* type, bool value = true);
			void API_3DF SetVisibility(const char* option, const char* sub = nullptr, bool value = true);
		};



		namespace Text
		{
			HC_KEY API_3DF Create(H3DF::Point center, const char* value);

			HC_KEY API_3DF Create(H3DF::Point center, const wchar_t* value);

			void API_3DF GetExtent(const char* value, float& width, float& height);

			void API_3DF GetExtent(const wchar_t* value, float& width, float& height);

			void API_3DF Update(HC_KEY textKey, const wchar_t* value);
		};



		namespace Cursor
		{
			HC_KEY API_3DF Create(HC_KEY textKey, int row, int column);

			void API_3DF Hide(HC_KEY key);

			void API_3DF Move(HC_KEY key, int row, int column);
		}
	};
};
