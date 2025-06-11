#pragma once

#include "3DF.h"

#include "Kit.h"
#include "Control.h"

namespace H3DF
{
	class Line
	{
	public:

		// The CoordinateSpace enum is a list of all coordinate spaces used in Visualize. These correspond to H3DF::Coordinate::Space
		enum class CoordinateSpace : uint32_t
		{
			Object,					// H3DF::Coordinate::Space::Object
			World,					// H3DF::Coordinate::Space::World
			NormalizedInnerWindow,	// H3DF::Coordinate::Space::NormalizedInnerWindow
			NormalizedInnerPixel	// H3DF::Coordinate::Space::NormalizedInnerPixel
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

	class LinePattern
	{
	public:

		// Enumerates the units used when specifying length, weight and offsets for line patterns.
		enum class SizeUnits : uint32_t
		{
			ObjectSpace,			// Object space units ignoring any scaling components in modelling matrices.
			SubscreenRelative,		// Fraction of the height of the outermost window.
			WindowRelative,			// Fraction of the height of the local window.
			WorldSpace,				// Object space units including any scaling components in modelling matrices and cameras.
			Points,					// Points units typically used for text size.  1 point corresponds to 1/72 inch.
			Pixels,					// Number of pixels.
			ScaleFactor				// Fraction of the default line width.  A value of <span class='code'>1</span> corresponds to 0.1 percent of the screen size.
		};

		// Enumerates how glyphs interact with other line pattern elements within a line pattern.
		enum class InsetBehavior : uint32_t
		{
			Overlap,				// Glyphs will overlap with surrounding line pattern elements.  Line pattern elements adjacent to glyphs will not be trimmed.
			Trim,					// Glyphs will overlap with surrounding line pattern elements.  Line pattern elements adjacent to glyphs will be trimmed.
			Inline					// Glyphs will not overlap with surrounding line pattern elements, and adjacent line pattern elements will not be trimmed.
		};

		// 	Enumerates the types of join to use for a line pattern.  Joins will always be mitred at the half angle for the smaller angle defined by two line
		//	segments.  This enumeration defines what the join will look like for the larger angle defined by two line segments.
		enum class Join : uint32_t
		{
			Mitre,				// The join for the larger angle will be mitred, i.e., the segments will meet at a point at the half angle of the larger angle.
			Round,				// The join for the larger angle will be rounded, i.e., there will be a circle that subtends the larger angle.
			Bevel				// The join for the larger angle will be beveled, i.e., there will be a line that subtends the larger angle.
		};

		// Enumerates the predefined cap types that can be used at the ends of line segments within a line pattern.
		enum class Cap : uint32_t
		{
			// Line segments will end with a flat cap perpendicular to the line segment vector.  This cap will occur immediately at the end of the line segment
			// and will not extend its length.
			Butt,
			// Line segments will end with half of a filled square cap whose side is perpendicular to the line segment vector.  This cap will extend the length of a
			// line segment, but otherwise will appear the same as the ButtCap. 
			Square,
			// Line segments will end with half of a filled circle cap whose diameter is perpendicular to the line segment vector.  This cap will extend the length
			// of a line segment.
			Round,
			// Line segments will end with a filled triangle cap whose base is perpendicular to the line segment vector.  This cap will extend the length of a line segment.
			Mitre
		};

		//Enumerates the justfication modes for a line pattern.  These are used to attempt to make the line pattern appear evenly spaced between end points of a line.
		enum class Justification : uint32_t
		{
			// Attempts to ensure that the first element of the line pattern appears at both the beginning and the end of the line.  This will give the best results
			// if the first element in a line pattern makes up at least half the total length of the elements for the line pattern body.
			Center,
			// Stretches the pattern and modifies the size of the line pattern elements to ensure even spacing.
			Stretch
		};

		// Enumerates the types of joins, start caps, and end caps that can be set when a line pattern attribute is set.
		enum class Modifier : uint32_t
		{
			GlyphName,				// A glyph was set for the cap or join.  The glyph name is valid.
			Enumerated				// A predefined (enumerated) cap or join was set.  The enumeration type is valid.
		};


		// Enumerates the predefined line patterns in Visualize.  These can be accessed via the LinePatternKit::GetDefault function and their appearance can be seen
		// here. Hardware acceleration for line patterns is available when the following conditions are met:
		enum class Default : uint32_t
		{
			Solid,					// A solid unbroked line pattern.
			DashDot,				// A repeating pattern of 18 solid pixels, 5 blank pixels, 4 solid pixels, and 5 blank pixels.
			Dashed,					// A repeating pattern of 10 solid pixels and 6 blank pixels.
			Dotted,					// A repeating pattern of 5 solid pixels and 3 blank pixels.
			Dash2Dot,				// A repeating pattern of 15 solid pixels, 4 blank pixels, 3 solid pixels, 3 blank pixels, 3 solid pixels, and 4 blank pixels.
			Dash3Dot,				// A repeating pattern of 15 solid pixels, 2 blank pixels, 3 solid pixels, 2 blank pixels, 3 solid pixels, 2 blank pixels, 3 solid pixels, and 2 blank pixels.
			LongDash,				// A repeating pattern of 26 solid pixels and 6 blank pixels.
			LongDashShortDash,		// A repeating pattern of 100 solid pixels, 10 blank pixels, 20 solid pixels, and 10 blank pixels.
			LongDash2ShortDash,		// A repeating pattern of 86 solid pixels, 8 blank pixels, 15 solid pixels, 8 blank pixels, 15 solid pixels, and 8 blank pixels.
			FineDot,				// A repeating pattern of 1 solid pixel and 3 blank pixels.
			None
		};

	private:
		LinePattern() {}
	};

	class LineAttributeKit : public Kit
	{
	public:
		LineAttributeKit();
		LineAttributeKit(LineAttributeKit const & cInThat);

		static const H3DF::Type staticType = H3DF::Type::LineAttributeKit;
		H3DF::Type ObjectType() const { return staticType; };

		LineAttributeKit const & operator = (LineAttributeKit const & cInThat);

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


	class API_3DF LineAttributeControl : public Control
	{
	public:
		explicit LineAttributeControl(SegmentKey & cInSegment);
		LineAttributeControl(LineAttributeControl const & cInThat);

		static const H3DF::Type staticType = H3DF::Type::LineAttributeControl;
		H3DF::Type ObjectType() const { return staticType; };

		LineAttributeControl & operator = (LineAttributeControl const & cInThat);

		LineAttributeControl & SetPattern(CStringA strInPatternName);
		LineAttributeControl & SetWeight(float fInWeight, Line::SizeUnits eInUnits = Line::SizeUnits::ScaleFactor);

		LineAttributeControl & UnsetPattern();
		LineAttributeControl & UnsetWeight();
		LineAttributeControl & UnsetEverything();

		bool ShowPattern(CStringA & strOutPatternName) const;
		bool ShowWeight(float & fOutWeight, Line::SizeUnits & eOutUnits) const;

	private:
		// Private default constructor to prevent instantiation without a segment.
		LineAttributeControl();
	};
}