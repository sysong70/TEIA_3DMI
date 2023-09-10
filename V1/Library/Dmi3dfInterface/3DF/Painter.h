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

		void SetView(HBaseView* view);

		void SetColor(const char* type, const char* space, double abc[3]);

		H3DF::Point TransColor(COLORREF color);



		namespace Arc
		{
			// 3 Points
			HC_KEY Create(H3DF::Point first, H3DF::Point second, H3DF::Point third);
			// angle: degree
			void GetPoints(H3DF::Point center, double radius, double startAngle, double endAngle, Points& points);

			void GetPoints(float x, float y, double radius, double startAngle, double endAngle, Points& points);
		};



		namespace Circle
		{
			// Center, Radius
			HC_KEY Create(H3DF::Point center, double radius, bool polygon = true);
			// 3 Points
			HC_KEY Create(H3DF::Point first, H3DF::Point second, H3DF::Point third, bool polygon = true);

			void GetPoints(H3DF::Point center, double radius, bool reverse, Points& points);
		};



		namespace Compute
		{
			double Distance(H3DF::Point p1, H3DF::Point p2);

			double PixelToWorld(double value);

			H3DF::Vector Normal();
		}



		namespace Figure
		{
			HC_KEY CreateDonut(H3DF::Point center, double inner, double outer);
			// Left & Right Half Circle
			HC_KEY CreateObround(H3DF::Point topLeft, H3DF::Point bottomRight);

			HC_KEY CreateRectangle(H3DF::Point topLeft, H3DF::Point bottomRight);
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
			HC_KEY Create(H3DF::Point first, H3DF::Point second, bool firstEnd = false, bool secondEnd = false);
		};



		namespace Polyline
		{
			HC_KEY Create(Points& points);
		};



		namespace Polygon
		{
			HC_KEY Create(Points& points);
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
			HC_KEY Create(H3DF::Point center, const char* value);

			HC_KEY Create(H3DF::Point center, const wchar_t* value);

			void GetExtent(const char* value, float& width, float& height);

			void GetExtent(const wchar_t* value, float& width, float& height);

			void Update(HC_KEY textKey, const wchar_t* value);
		};



		namespace Cursor
		{
			HC_KEY Create(HC_KEY textKey, int row, int column);

			void Hide(HC_KEY key);

			void Move(HC_KEY key, int row, int column);
		}
	};
};
