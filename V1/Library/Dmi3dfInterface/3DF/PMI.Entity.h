#pragma once

#include "3DF.h"
#include "Segment.h"
#include "Math.h"
#include "Math.Matrix.h"

#include "Color.h"

#include <atlstr.h>

namespace H3DF {
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
	class Draw;
	class ViewEntity;

	class API_3DF PmiKey : public SegmentKey
	{
	public:
		PmiKey(HC_KEY nInKey = INVALID_KEY);
		PmiKey(Key const & cInThat);
		PmiKey(PmiKey const & cInThat);

		static const H3DF::Type staticType = H3DF::Type::PMI;
		H3DF::Type ObjectType() const { return staticType; };

		void Set(PmiKey const & cInThat);
		PmiKey const & operator=(PmiKey const & cInThat);

		void GetFrame(Frame & cOutFrame);
		void SetFrame(Frame const & cInFrame);

		void GetDrawing(Draw & cOutDrawing) const;
		void SetDrawing(Draw const & cInDrawing);

		unsigned int GetLeaderLineCount() const;
		void SetLeaderLines(unsigned int in_count, Polyline const * in_leader_lines);

		void SetLeaderSymbols(unsigned int nInCount, H3DF::Polygon const * pcInLeaderSymbols);

		void SetDisplayParallelToScreen(bool const bInParallel);
		bool IsDisplayParallelToScreen() const;

	protected:
		void SetPolygons(char const * pchInSegmentName, unsigned int nInCount, H3DF::Polygon const * pcInPolygons);

		void SetColor(const H3DF::Polyline & cPolyline);
		void SetColor(const H3DF::Polygon & cPolygon);

		unsigned int GetStringsAndTextAttributesCount(CString strInSegmentName) const;
		void GetStringsAndTextAttributes(CString strInSegmentName, CString * pstrOutStrings, TextAttributes * pcOutTextAttributes) const;
		void SetStringsAndTextAttributes(CString strInSegmentName, unsigned int in_count, CString const * in_strings,
			TextAttributes const * in_text_attributes, bool is_parallel_to_screen = false);
	};

	class API_3DF DatumEntity : public PmiKey
	{
	public:
		DatumEntity(HC_KEY nInKey = INVALID_KEY);
		DatumEntity(SegmentKey const & cInThat);
		DatumEntity(DatumEntity const & cInThat);

		static const H3DF::Type staticType = H3DF::Type::DatumEntity;
		H3DF::Type ObjectType() const { return staticType; };

		DatumEntity const & operator=(DatumEntity const & cInThat);

		Datum::Type GetDatumType() const;
		void SetDatumType(Datum::Type const eInType);

		unsigned int GetLabelCount() const;
		void GetLabels(unsigned int & nOutCount, CString * pstrOutLabels, TextAttributes * pcOutTextAttributes) const;
		void SetLabels(unsigned int in_count, CString const * pstrInlabels, TextAttributes const * in_text_attributes);
	};

	class API_3DF DimensionEntity : public PmiKey
	{
	public:
		DimensionEntity(HC_KEY nInKey = INVALID_KEY);
		DimensionEntity(SegmentKey const & cInThat);
		DimensionEntity(DimensionEntity const & cInThat);

		static const H3DF::Type staticType = H3DF::Type::DimensionEntity;
		H3DF::Type ObjectType() const { return staticType; };

		void Set(DimensionEntity const & cInThat);
		DimensionEntity const & operator=(DimensionEntity const & cInThat);

		Dimension::Type GetDimensionType() const;
		void SetDimensionType(Dimension::Type const eInType);

		Dimension::SubType GetDimensionSubType() const;
		void SetDimensionSubType(Dimension::SubType const eInSubType);

		unsigned int GetStringCount() const;
		void GetStrings(unsigned int & nOutCount, CString * pstrOutStrings, TextAttributes * pcOutTextAttributes) const;
		void SetStrings(unsigned int nInCount, CString const * pstrInStrings, TextAttributes const * pcInTextAttributes);
	};

	class API_3DF FeatureControlFrameEntity : public PmiKey {
	public:
		FeatureControlFrameEntity(HC_KEY nInKey = INVALID_KEY);
		FeatureControlFrameEntity(SegmentKey const & cInThat);
		FeatureControlFrameEntity(FeatureControlFrameEntity const & cInThat);

		static const H3DF::Type staticType = H3DF::Type::FeatureControlFrameEntity;
		H3DF::Type ObjectType() const { return staticType; };

		void Set(FeatureControlFrameEntity const & cInThat);
		FeatureControlFrameEntity const & operator=(FeatureControlFrameEntity const & cInThat);
	};

	class API_3DF GenericEntity : public PmiKey
	{
	public:
		GenericEntity(HC_KEY nInKey = INVALID_KEY);
		GenericEntity(SegmentKey const & cInThat);
		GenericEntity(GenericEntity const & cInThat);

		static const H3DF::Type staticType = H3DF::Type::GenericEntity;
		H3DF::Type ObjectType() const { return staticType; };

		void Set(GenericEntity const & cInThat);
		GenericEntity const & operator=(GenericEntity const & cInThat);

		unsigned int GetStringCount() const;
		void GetStrings(unsigned int & nOutCount, CString * pstrOutStrings, TextAttributes * pcOutTextAttributes) const;
		void SetStrings(unsigned int nInCount, CString const * pstrInStrings, TextAttributes const * pcInTextAttributes);

		void SetDisplayParallelToScreen(bool const in_parallel = true);
		bool IsDisplayParallelToScreen() const;
	};

	class API_3DF NoteEntity : public PmiKey
	{
	public:
		NoteEntity(HC_KEY nInKey = INVALID_KEY);
		NoteEntity(SegmentKey const & cInThat);
		NoteEntity(NoteEntity const & cInThat);

		static const H3DF::Type staticType = H3DF::Type::NoteEntity;
		H3DF::Type ObjectType() const { return staticType; };

		void Set(NoteEntity const & cInThat);
		NoteEntity const & operator=(NoteEntity const & cInThat);

		unsigned int GetStringCount() const;
		void GetStrings(unsigned int & nOutCount, CString pstrOutStrings[], TextAttributes pcOutTextAttributes[]) const;
		void SetStrings(unsigned int nInCount, CString const pstrInStrings[], TextAttributes const pcInTextAttributes[]);
	};

	class API_3DF RoughnessEntity : public PmiKey
	{
	public:
		RoughnessEntity(HC_KEY nInKey = INVALID_KEY);
		RoughnessEntity(SegmentKey const & cInThat);
		RoughnessEntity(RoughnessEntity const & cInThat);

		static const H3DF::Type staticType = H3DF::Type::RoughnessEntity;
		H3DF::Type ObjectType() const { return staticType; };

		void Set(RoughnessEntity const & cInThat);
		RoughnessEntity const & operator=(RoughnessEntity const & cInThat);

		Roughness::Obtention::Type GetObtentionType() const;
		void SetObtentionType(Roughness::Obtention::Type const in_obtention_type);

		Roughness::Applicability::Type GetApplicabilityType() const;
		void SetApplicabilityType(Roughness::Applicability::Type const eInApplicabilityType);

		Roughness::Mode::Type GetModeType() const;
		void SetModeType(Roughness::Mode::Type const in_mode_type);

		unsigned int GetFieldCount() const;
		void GetFields(unsigned int & nOutCount, CString * pstrOutStrings, TextAttributes * pcOutTextAttributes) const;
		void SetFields(unsigned int nInCount, CString const * pstrInStrings, TextAttributes const * pcInTextAttributes);
	};

	class API_3DF Orientation : public PmiKey
	{
	public:
		Orientation();
		Orientation(Orientation const & cInThat);

		static const H3DF::Type staticType = H3DF::Type::Orientation;
		H3DF::Type ObjectType() const { return staticType; };

		Orientation const & operator=(Orientation const & cInThat);

		void GetMatrix(float out_matrix[16]) const;
		void SetMatrix(H3DF::MatrixKit const & cInMatrix);
	};

	class API_3DF TextAttributes : public PmiKey
	{
	public:
		TextAttributes();
		TextAttributes(TextAttributes const & cInThat);

		static const H3DF::Type staticType = H3DF::Type::TextAttributes;
		H3DF::Type ObjectType() const { return staticType; };

		TextAttributes const & operator=(TextAttributes const & cInThat);

		void GetInsertionPoint(H3DF::Point & out_point) const;
		void SetInsertionPoint(H3DF::Point const & in_point);

		void GetFontSize(float & fOutSize) const;
		void SetFontSize(float const nInSize);

		void GetFontSizeUnits(Font::Size::Units & eOutUnits) const;
		void SetFontSizeUnits(Font::Size::Units const eInUnits);

		void GetFontName(char * pchOutFontName) const;
		void SetFontName(char const * pchInFontName);

		void GetOrientation(Orientation & cOutOrientation) const;
		void SetOrientation(Orientation const & cInOrientation);

		void GetRGBColor(H3DF::RGBColor & cOutColor) const;
		void SetRGBColor(H3DF::RGBColor const & cInColor);

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

	class API_3DF Options : public PmiKey
	{
	public:
		Options();
		Options(Options const & cInThat);

		static const H3DF::Type staticType = H3DF::Type::Options;
		H3DF::Type ObjectType() const { return staticType; };

		Options const & operator=(Options const & cInThat);

		// Set the option parallel to screen. The pmi is display parallel to screen and is not impacted by any rotation
		void SetDisplayParallelToScreen(bool const in_parallel = true);
		// Return the option parallel to screen value.
		bool IsDisplayParallelToScreen() const;
	};

	class API_3DF Frame : public PmiKey
	{
	public:
		Frame();
		Frame(Frame const & cInThat);

		static const H3DF::Type staticType = H3DF::Type::Frame;
		H3DF::Type ObjectType() const { return staticType; };

		Frame const & operator=(Frame const & cInThat);

		unsigned int GetPolylineCount() const;
		void GetPolylines(unsigned int & nOutCount, H3DF::Polyline * pcOutPolylines) const;
		void SetPolylines(unsigned int nInCount, H3DF::Polyline const * pcInPolylines);
	};

	class API_3DF Draw : public PmiKey
	{
	public:
		Draw();
		Draw(Draw const & cInThat);

		static const H3DF::Type staticType = H3DF::Type::Draw;
		H3DF::Type ObjectType() const { return staticType; };

		Draw const & operator=(Draw const & cInThat);

		unsigned int GetPolygonCount() const;

		void GetPolygons(unsigned int & nOutCount, H3DF::Polygon * pcOutPolygons) const;
		void SetPolygons(unsigned int nInCount, H3DF::Polygon const * pcInPolygons);
	};

	class API_3DF ViewEntity : public PmiKey
	{
	public:
		ViewEntity(HC_KEY nInSegmentKey = INVALID_KEY);
		ViewEntity(SegmentKey & nInSegmentKey);
		ViewEntity(ViewEntity const & cInThat);

		static const H3DF::Type staticType = H3DF::Type::ViewEntity;
		H3DF::Type ObjectType() const { return staticType; };

		ViewEntity const & operator = (ViewEntity const & cInThat);

		void GetCamera(CameraKit & cOutCamera) const;
		void SetCamera(CameraKit const & cInCamera);

		size_t GetAssociatedEntityCount() const;
		void GetAssociatedEntities(size_t & nOutCount, PmiKey *& pcOutEntities) const;
// 		void SetAssociatedEntities(unsigned int in_count, PMI const in_entities[]);
/*
		void GetCamera(Point & out_position,
			Point & out_target,
			Point & out_up_vector,
			float & out_field_width,
			float & out_field_height,
			char out_projection[128]);

		void GetCamera(float out_position[3],
			float out_target[3],
			float out_up_vector[3],
			float & out_field_width,
			float & out_field_height,
			char out_projection[128]);

		void SetCamera(Point const & in_position,
			Point const & in_target,
			Point const & in_up_vector,
			float in_field_width,
			float in_field_height,
			char const in_projection[128]);

		void SetCamera(float const in_position[3],
			float const in_target[3],
			float const in_up_vector[3],
			float in_field_width,
			float in_field_height,
			char const in_projection[128]);


		void GetFrame(Frame & out_frame) const;

		void SetFrame(Frame const & in_frame);

		unsigned int GetAssociatedEntityCount() const;
		void GetAssociatedEntities(unsigned int & out_count, PMI out_entities[]) const;
		void SetAssociatedEntities(unsigned int in_count, PMI const in_entities[]);
*/
	};
}