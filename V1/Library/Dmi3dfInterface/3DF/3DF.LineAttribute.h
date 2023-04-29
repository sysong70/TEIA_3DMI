#pragma once

#include "3DF.h"

#include "3DF.Kit.h"
#include "3DF.Control.h"

OPEN_3DF_NAMESPACE

class Line
{
public:

	// The CoordinateSpace enum is a list of all coordinate spaces used in Visualize. These correspond to TDF::Coordinate::Space
	enum class CoordinateSpace : uint32_t
	{
		Object,					// TDF::Coordinate::Space::Object
		World,					// TDF::Coordinate::Space::World
		NormalizedInnerWindow,	// TDF::Coordinate::Space::NormalizedInnerWindow
		NormalizedInnerPixel	// TDF::Coordinate::Space::NormalizedInnerPixel
	};

	// Line Size Units
	enum class SizeUnits : uint32_t
	{
		ScaleFactor,			// A unitless linear scaling factor.  A value of 2.0 will cause edges to be rendered twice as thick.  
								// A value of 1.0 will result in a visually pleasing device-dependent Line weight.  
								// A value of 0 will result in the minimal weight Line supported by the device.
		ObjectSpace,			// Object space units ignoring any scaling components in modelling matrices. (oru)
		SubscreenRelative,		// Fraction of the height of the outermost window. (sru)
		WindowRelative,			// Fraction of the height of the local window. (wru)
		WorldSpace,				// Object space units including any scaling components in modelling matrices and cameras. (wsu)
		Points,					// Points units typically used for text size. 1 point corresponds to 1/72 inch. (points)
		Pixels,					// Number of pixels. (pixels)
	};

private:
	Line() {}
};

class LineAttributeKit : public Kit
{
public:
	LineAttributeKit();
	LineAttributeKit(LineAttributeKit const & cInThat);

	void Set(LineAttributeKit const & cInThat);
	LineAttributeKit const & operator = (LineAttributeKit const & cInThat);

	TDF::Type ObjectType() const { return TDF::Type::LineAttributeKit; };

	void Show(LineAttributeKit & cOutKit) const;

	bool Empty() const;

	bool operator == (LineAttributeKit const & cInThat) const;
	bool operator != (LineAttributeKit const & cInThat) const;

	LineAttributeKit & SetPattern(CString strInPatternName);
	LineAttributeKit & SetWeight(float fInWeight, Line::SizeUnits eInUnits = Line::SizeUnits::ScaleFactor);

	LineAttributeKit & UnsetPattern();
	LineAttributeKit & UnsetWeight();
	LineAttributeKit & UnsetEverything();

	bool ShowPattern(CString & strOutPatternName) const;
	bool ShowWeight(float & fOutWeight, Line::SizeUnits & eOutUnits) const;
};


class LineAttributeControl : public Control
{
public:
	LineAttributeControl(SegmentKey & cInSegmentKey);
	LineAttributeControl(LineAttributeControl const & cInThat);

	void Set(LineAttributeControl const & cInThat);
	LineAttributeControl & operator = (LineAttributeControl const & cInThat);

	TDF::Type ObjectType() const { return TDF::Type::LineAttributeControl; };

	LineAttributeControl & SetPattern(CString strInPatternName);
	LineAttributeControl & SetWeight(float fInWeight, Line::SizeUnits eInUnits = Line::SizeUnits::ScaleFactor);

	LineAttributeControl & UnsetPattern();
	LineAttributeControl & UnsetWeight();
	LineAttributeControl & UnsetEverything();

	bool ShowPattern(CString & strOutPatternName) const;
	bool ShowWeight(float & fOutWeight, Line::SizeUnits & eOutUnits) const;

private:
	// Private default constructor to prevent instantiation without a segment.
	LineAttributeControl();
};


CLOSE_3DF_NAMESPACE