#pragma once

//--------------------------------------------------------------------------------------------------

namespace Uio
{
	//  do not use enum class
	enum EInputOptions
	{
		eDefault = 0,
		eAllowInteger = 1 << 0,
		eAllowReal = 1 << 1,
		eNoZero = 1 << 2,
		eNoNegative = 1 << 3,
		eUseLastPoint = 1 << 4,

		eRubberBand = 1 << 5,
		eRubberRect = 1 << 6,
		eBandLine = 1 << 7,
		eLengthGuide = 1 << 8,
		eAngleGuide = 1 << 9,

		eUseBandGuide = eRubberBand | eBandLine | eLengthGuide | eAngleGuide,
		eUseGuideOnly = eRubberBand | eLengthGuide | eAngleGuide,
		eUseLengthGuideOnly = eRubberBand | eBandLine | eLengthGuide,
		eUseAngleGuideOnly = eRubberBand | eBandLine | eAngleGuide,

		eUseLengthFilter = 1 << 10,
		eUseAngleFilter = 1 << 11,
		eUseXFilter = 1 << 12,
		eUseYFilter = 1 << 13,
		eUseOSnap = 1 << 14,

		eFirstPoint = eUseXFilter | eUseYFilter | eUseOSnap,
		eOtherPoint = eUseLengthFilter | eUseAngleFilter | eFirstPoint,
	};

	enum EFilter
	{
		None = 0,
		Angle = 1 << 0,
		Length = 1 << 1,
		X = 1 << 3,
		Y = 1 << 4,

		WaitAngle = 1 << 5,
		WaitLength = 1 << 6,
		WaitX = 1 << 7,
		WaitY = 1 << 8,
	};

	enum class EOSnap
	{
		None,
		Point,
		End,
		Mid,
		Intersection,
		Perpendicular,
		Center,
		Quadrant,
		Near,
	};

	enum class EWait
	{
		None,
		OSnap,

		Point,
		PointOrAngle,
		PointOrLength,

		Integer,
		Real,
		String,
	};

	enum class EReturn
	{
		None,
		Angle,
		Length,
		Point,
		Integer,
		Real,
		String,

		Cancel,
		Keyword,
	};



	double ToRadian(double degree);

	double ToDegree(double radian);
}
