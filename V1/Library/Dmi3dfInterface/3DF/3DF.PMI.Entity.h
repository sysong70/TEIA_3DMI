#pragma once

#include "3DF.h"
#include "3DF.Key.h"
#include "3DF.Math.h"

#include "3DF.Color.h"

#include <atlstr.h>

#define OPEN_3DF_PMI_NAMESPACE namespace _3DF { namespace PMI {
#define CLOSE_3DF_PMI_NAMESPACE } }

#define USING_3DF_PMI_NAMESPACE using namespace _3DF::PMI;

OPEN_3DF_PMI_NAMESPACE

enum class Type
{
	UnknownType,
	DatumType,
	DimensionType,
	FeatureControlFrameType,
	GenericType,
	NoteType,
	RoughnessType
};

namespace Datum
{
	enum class Type
	{
		Unknown,
		Identifier,
		Target
	};
};

namespace Dimension
{
	enum class Type
	{
		UnknownType,
		BasicType,
		ReferenceType,
		ToleranceType
	};

	enum class SubType
	{
		UnknownSubType,
		AngleSubType,
		ChamferSubType,
		DiameterSubType,
		DistanceSubType,
		LengthSubType,
		RadiusSubType,
		SlopeSubType
	};

};

namespace Roughness
{
	namespace Obtention
	{
		enum class Type
		{
			Unknown,
			NotDefined,
			MachiningMandatory,
			MachiningForbidden
		};
	};

	namespace Applicability
	{
		enum class Type
		{
			Unknown,
			SpecifiedSurface,
			SurroundingSurfaces,
			BasicSurface,
			AllSurfaces
		};
	};

	namespace Mode
	{
		enum class Type
		{
			Unknown,
			None,
			MultiDirectional,
			Circular,
			Radial,
			Particular,
			WithAngles,
			Parallel,
			Perpendicular
		};
	};
};

namespace Font
{
	namespace Size
	{
		enum class Units
		{
			UnknownUnits,
			WorldSpaceUnits,
			PixelUnits
		};
	};
};

class Frame;
class Drawing;

class API_3DF Entity : public Key
{
public:
	Entity(HC_KEY nInKey = INVALID_KEY);
	Entity(Key const & cInThat);
	Entity(Entity const & cInThat);

	void Set(Entity const & cInThat);
	Entity const & operator=(Entity const & cInThat);

	// return the PMI::Type for this entity. Overridden by subclasses.
	virtual PMI::Type GetType() const { return PMI::Type::UnknownType; };

	void SetFrame(Frame const & cInFrame);

	void GetDrawing(Drawing & cOutDrawing) const;
	void SetDrawing(Drawing const & cInDrawing);

	unsigned int GetLeaderLineCount() const;
	void SetLeaderLines(unsigned int in_count, Polyline const * in_leader_lines);

	void SetLeaderSymbols(unsigned int nInCount, _3DF::Polygon const * pcInLeaderSymbols);

	void SetDisplayParallelToScreen(bool const bInParallel);
	bool IsDisplayParallelToScreen() const;

protected:
	void SetPolygons(char const * pchInSegmentName, unsigned int nInCount, _3DF::Polygon const * pcInPolygons);

	void SetColor(const _3DF::Polyline & cPolyline);
	void SetColor(const _3DF::Polygon & cPolygon);

	unsigned int GetStringsAndTextAttributesCount(CString strInSegmentName) const;
	void GetStringsAndTextAttributes(CString strInSegmentName, CString * pstrOutStrings, TextAttributes * pcOutTextAttributes) const;
	void SetStringsAndTextAttributes(CString strInSegmentName, unsigned int in_count, CString const * in_strings,
		TextAttributes const * in_text_attributes, bool is_parallel_to_screen = false);
};

class API_3DF DatumEntity : public Entity
{
public:
	DatumEntity(HC_KEY nInKey = INVALID_KEY);
	DatumEntity(Key const & cInThat);
	DatumEntity(DatumEntity const & cInThat);

	void Set(DatumEntity const & cInThat);
	DatumEntity const & operator=(DatumEntity const & cInThat);

	PMI::Type GetType() const override { return PMI::Type::DatumType; };

	Datum::Type GetDatumType() const;
	void SetDatumType(Datum::Type const eInType);
	
	unsigned int GetLabelCount() const;
	void GetLabels(unsigned int & nOutCount, CString * pstrOutLabels, TextAttributes * pcOutTextAttributes) const;
	void SetLabels(unsigned int in_count, CString const * pstrInlabels, TextAttributes const * in_text_attributes);
};

class API_3DF DimensionEntity : public Entity
{
public:
	DimensionEntity(HC_KEY nInKey = INVALID_KEY);
	DimensionEntity(Key const & cInThat);
	DimensionEntity(DimensionEntity const & cInThat);

	void Set(DimensionEntity const & cInThat);
	DimensionEntity const & operator=(DimensionEntity const & cInThat);

	PMI::Type GetType() const override { return Type::DimensionType; };

	Dimension::Type GetDimensionType() const;
	void SetDimensionType(Dimension::Type const cInType);

	Dimension::SubType GetDimensionSubType() const;
	void SetDimensionSubType(Dimension::SubType const cInSubType);

	unsigned int GetStringCount() const;
	void GetStrings(unsigned int & nOutCount, CString * pstrOutStrings, TextAttributes * pcOutTextAttributes) const;
	void SetStrings(unsigned int nInCount, CString const * pstrInStrings, TextAttributes const * pcInTextAttributes);
};

class API_3DF GenericEntity : public Entity
{
public:
	GenericEntity(HC_KEY nInKey = INVALID_KEY);
	GenericEntity(Key const & cInThat);
	GenericEntity(GenericEntity const & cInThat);

	void Set(GenericEntity const & cInThat);
	GenericEntity const & operator=(GenericEntity const & cInThat);

	PMI::Type GetType() const override { return PMI::Type::GenericType; };

	unsigned int GetStringCount() const;
	void GetStrings(unsigned int & nOutCount, CString * pstrOutStrings, TextAttributes * pcOutTextAttributes) const;
	void SetStrings(unsigned int nInCount, CString const * pstrInStrings, TextAttributes const * pcInTextAttributes);

	void SetDisplayParallelToScreen(bool const in_parallel = true);
	bool IsDisplayParallelToScreen() const;
};

class API_3DF NoteEntity : public Entity
{
public:
	NoteEntity(HC_KEY nInKey = INVALID_KEY);
	NoteEntity(Key const & cInThat);
	NoteEntity(NoteEntity const & cInThat);

	void Set(NoteEntity const & cInThat);
	NoteEntity const & operator=(NoteEntity const & cInThat);

	PMI::Type GetType() const override { return PMI::Type::NoteType; };

	unsigned int GetStringCount() const;
	void GetStrings(unsigned int & nOutCount, CString pstrOutStrings[], TextAttributes pcOutTextAttributes[]) const;
	void SetStrings(unsigned int nInCount, CString const pstrInStrings[], TextAttributes const pcInTextAttributes[]);
};

class API_3DF RoughnessEntity : public Entity
{
public:
	RoughnessEntity(HC_KEY nInKey = INVALID_KEY);
	RoughnessEntity(Key const & cInThat);
	RoughnessEntity(RoughnessEntity const & cInThat);

	void Set(RoughnessEntity const & cInThat);
	RoughnessEntity const & operator=(RoughnessEntity const & cInThat);

	virtual PMI::Type GetType() const { return PMI::Type::RoughnessType; };

	Roughness::Obtention::Type GetObtentionType() const;
	void SetObtentionType(Roughness::Obtention::Type const in_obtention_type);

	Roughness::Applicability::Type GetApplicabilityType() const;
	void SetApplicabilityType(Roughness::Applicability::Type const in_applicability_type);

	Roughness::Mode::Type GetModeType() const;
	void SetModeType(Roughness::Mode::Type const in_mode_type);

	unsigned int GetFieldCount() const;
	void GetFields(unsigned int & nOutCount, CString * pstrOutStrings, TextAttributes * pcOutTextAttributes) const;
	void SetFields(unsigned int nInCount, CString const * pstrInStrings, TextAttributes const * pcInTextAttributes);
};

class API_3DF Orientation : public Entity
{
public:
	Orientation();
	Orientation(Orientation const & cInThat);

	void Set(Orientation const & cInThat);
	Orientation const & operator=(Orientation const & cInThat);

	void GetMatrix(float out_matrix[16]) const;
	void SetMatrix(_3DF::Matrix const & cInMatrix);
};

class API_3DF TextAttributes : public Entity
{
public:
	TextAttributes();
	TextAttributes(TextAttributes const & cInThat);

	void Set(TextAttributes const & cInThat);
	TextAttributes const & operator=(TextAttributes const & cInThat);

	void GetInsertionPoint(_3DF::Point & out_point) const;
	void SetInsertionPoint(_3DF::Point const & in_point);

	void GetFontSize(float & fOutSize) const;
	void SetFontSize(float const nInSize);

	void GetFontSizeUnits(Font::Size::Units & eOutUnits) const;
	void SetFontSizeUnits(Font::Size::Units const eInUnits);

	void GetFontName(char * pchOutFontName) const;
	void SetFontName(char const * pchInFontName);

	void GetOrientation(Orientation & cOutOrientation) const;
	void SetOrientation(Orientation const & cInOrientation);

	void GetRGBColor(_3DF::RGBColor & cOutColor) const;
	void SetRGBColor(_3DF::RGBColor const & cInColor);

	bool IsBold() const;
	bool IsItalic() const;
	bool IsUnderlined() const;
	bool IsStrikedThrough() const;
	bool IsOverlined() const;
	bool IsStreched() const;
	bool IsWired() const;
	bool IsFixedWidth() const;

	void SetFormat(char const & chInFormat);
	
	double WidthScale() const;
	void SetWidthScale(const double dWidthScale);
};

class API_3DF Options : public Entity
{
public:
	Options();
	Options(Options const & cInThat);

	void Set(Options const & cInThat);
	Options const & operator=(Options const & cInThat);

	// Set the option parallel to screen. The pmi is display parallel to screen and is not impacted by any rotation
	void SetDisplayParallelToScreen(bool const in_parallel = true);
	// Return the option parallel to screen value.
	bool IsDisplayParallelToScreen() const;
};

class API_3DF Frame : public Entity
{
public:
	Frame();
	Frame(Frame const & cInThat);

	void Set(Frame const & cInThat);
	Frame const & operator=(Frame const & cInThat);

	unsigned int GetPolylineCount() const;
	void GetPolylines(unsigned int & nOutCount, _3DF::Polyline * pcOutPolylines) const;
	void SetPolylines(unsigned int nInCount, _3DF::Polyline const * pcInPolylines);
};

class API_3DF Drawing : public Entity
{
public:
	Drawing();
	Drawing(Drawing const & cInThat);

	void Set(Drawing const & cInThat);
	Drawing const & operator=(Drawing const & cInThat);

	unsigned int GetPolygonCount() const;

	void GetPolygons(unsigned int & nOutCount, _3DF::Polygon pcOutPolygons[]) const;
	void SetPolygons(unsigned int nInCount, _3DF::Polygon const pcInPolygons[]);
};


CLOSE_3DF_PMI_NAMESPACE