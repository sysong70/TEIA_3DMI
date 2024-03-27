#pragma once

//--------------------------------------------------------------------------------------------------

namespace Geometry
{
	class UserDataFiler;



	enum EType
	{
		Unknown = -1,
		Figure,
		Symbol,

		Point,

		Line,
		Polyline,
		Polygon,
		Rectangle,

		Arc,
		Circle,
		Ellipse,
		Spline,

		Text,
		Leader,
		Note,

		Dimension,
	};



	enum EFilerCommon
	{
		Type,
		Id,
		Name,
		LineWidth,
		LineStyle,
		Color,
	};
};
