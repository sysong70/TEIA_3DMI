// Copyright (c) Tech Soft 3D
//
// The information contained herein is confidential and proprietary to Tech Soft 3D, Inc.,
// and considered a trade secret as defined under civil and criminal statutes.
// Tech Soft 3D, Inc. shall pursue its civil and criminal remedies in the event of
// unauthorized use or misappropriation of its trade secrets.  Use of this information
// by anyone other than authorized employees of Tech Soft 3D, Inc. is granted only under
// a written non-disclosure agreement, expressly prescribing the scope and manner of such use.

#pragma once


namespace Query
{

	template <typename Type, typename TypeData>
	class ExchangePointerObject
	{
	public:
		typedef A3DStatus(*PGetFunction)(Type const *, TypeData *);

		ExchangePointerObject(
			Type * entity,
			PGetFunction getter)
			: entity(entity)
			, getter(getter)
			, valid(false)
		{}

		ExchangePointerObject(
			ExchangePointerObject && that)
		{
			*this = that;

			that.valid = false;
			that.entity = nullptr;
			that.memset(&data, 0, sizeof(data));
			that.getter = nullptr;
		}

		virtual ~ExchangePointerObject()
		{
			if(valid)
				getter(nullptr, &data);
		}

		ExchangePointerObject & operator=(
			ExchangePointerObject && that)
		{
			*this = that;

			that.valid = false;
			that.entity = nullptr;
			that.memset(&data, 0, sizeof(data));
			that.getter = nullptr;

			return *this;
		}

		operator Type * () const { return entity; }

		TypeData const * operator->() const { return &data; }
		TypeData * operator->() { return &data; }

		bool IsValid() const { return valid; }

		TypeData data;

	private:
		bool valid;
		Type * entity;
		PGetFunction getter;

	protected:
		void getData()
		{
			A3DStatus status = getter(entity, &data);
			valid = (status == A3D_SUCCESS);
		}

	private:
		ExchangePointerObject();
		ExchangePointerObject(ExchangePointerObject const &);
		ExchangePointerObject & operator=(ExchangePointerObject const &) = default;
	};

	template <typename Type, typename TypeData>
	class ExchangeIndexObject
	{
	public:
		typedef A3DStatus(*PGetFunction)(Type, TypeData *);
		ExchangeIndexObject(
			Type index,
			PGetFunction getter)
			: index(index)
			, getter(getter)
			, valid(false)
		{}

		ExchangeIndexObject(
			ExchangeIndexObject && that)
		{
			*this = that;

			that.valid = false;
			that.index = (Type) -1;
			that.memset(&data, 0, sizeof(data));
			that.getter = nullptr;
		}

		virtual ~ExchangeIndexObject()
		{
			if(valid)
				getter((Type) -1, &data);
		}

		ExchangeIndexObject & operator=(
			ExchangeIndexObject && that)
		{
			*this = that;

			that.valid = false;
			that.index = (Type) -1;
			that.memset(&data, 0, sizeof(data));
			that.getter = nullptr;

			return *this;
		}

		operator Type () const { return index; }

		TypeData const * operator->() const { return &data; }
		TypeData * operator->() { return &data; }

		bool IsValid() const { return valid; }

		TypeData data;

	private:
		bool valid;
		Type index;
		PGetFunction getter;

	protected:
		void getData()
		{
			A3DStatus status = getter(index, &data);
			valid = (status == A3D_SUCCESS);
		}

	private:
		ExchangeIndexObject();
		ExchangeIndexObject(ExchangeIndexObject const &);
		ExchangeIndexObject & operator=(ExchangeIndexObject const &) = default;
	};

	typedef ExchangePointerObject<A3DDrawingModel, A3DDrawingModelData> BaseDrawingModel;
	class DrawingModel : public BaseDrawingModel
	{
	public:
		DrawingModel(
			A3DDrawingModel * entity)
			: BaseDrawingModel(entity, A3DDrawingModelGet)
		{
			A3D_INITIALIZE_DATA(A3DDrawingModelData, data);
			getData();
		}
	};

	typedef ExchangePointerObject<A3DDrawingSheet, A3DDrawingSheetData> BaseDrawingSheet;
	class DrawingSheet : public BaseDrawingSheet
	{
	public:
		DrawingSheet(
			A3DDrawingSheet * entity)
			: BaseDrawingSheet(entity, A3DDrawingSheetGet)
		{
			A3D_INITIALIZE_DATA(A3DDrawingSheetData, data);
			getData();
		}
	};

	typedef ExchangeIndexObject<A3DUns32, A3DGraphStyleData> BaseGraphStyle;
	class GraphStyle : public BaseGraphStyle
	{
	public:
		GraphStyle(
			A3DUns32 index)
			: BaseGraphStyle(index, A3DGlobalGetGraphStyleData)
		{
			A3D_INITIALIZE_DATA(A3DGraphStyleData, data);
			if(index != A3D_DEFAULT_STYLE_INDEX)
				getData();
		}
	};

	typedef ExchangePointerObject<A3DDrawingSheetFormat, A3DDrawingSheetFormatData> BaseDrawingSheetFormat;
	class DrawingSheetFormat : public BaseDrawingSheetFormat
	{
	public:
		DrawingSheetFormat(
			A3DDrawingSheetFormat * entity)
			: BaseDrawingSheetFormat(entity, A3DDrawingSheetFormatGet)
		{
			A3D_INITIALIZE_DATA(A3DDrawingSheetFormatData, data);
			getData();
		}
	};

	typedef ExchangePointerObject<A3DDrawingBlockBasic, A3DDrawingBlockBasicData> BaseDrawingBlockBasic;
	class DrawingBlockBasic : public BaseDrawingBlockBasic
	{
	public:
		DrawingBlockBasic(
			A3DDrawingBlockBasic * entity)
			: BaseDrawingBlockBasic(entity, A3DDrawingBlockBasicGet)
		{
			A3D_INITIALIZE_DATA(A3DDrawingBlockBasicData, data);
			getData();
		}
	};

	typedef ExchangePointerObject<A3DDrawingBlockOperator, A3DDrawingBlockOperatorData> BaseDrawingBlockOperator;
	class DrawingBlockOperator : public BaseDrawingBlockOperator
	{
	public:
		DrawingBlockOperator(
			A3DDrawingBlockOperator * entity)
			: BaseDrawingBlockOperator(entity, A3DDrawingBlockOperatorGet)
		{
			A3D_INITIALIZE_DATA(A3DDrawingBlockOperatorData, data);
			getData();
		}
	};

	typedef ExchangePointerObject<A3DDrawingCurve, A3DDrawingCurveData> BaseDrawingCurve;
	class DrawingCurve : public BaseDrawingCurve
	{
	public:
		DrawingCurve(
			A3DDrawingCurve * entity)
			: BaseDrawingCurve(entity, A3DDrawingCurveGet)
		{
			A3D_INITIALIZE_DATA(A3DDrawingCurveData, data);
			getData();
		}
	};

	typedef ExchangePointerObject<A3DDrawingFilledArea, A3DDrawingFilledAreaData> BaseDrawingFilledArea;
	class DrawingFilledArea : public BaseDrawingFilledArea
	{
	public:
		DrawingFilledArea(
			A3DDrawingFilledArea * entity)
			: BaseDrawingFilledArea(entity, A3DDrawingFilledAreaGet)
		{
			A3D_INITIALIZE_DATA(A3DDrawingFilledAreaData, data);
			getData();
		}
	};

	typedef ExchangePointerObject<A3DDrawingPicture, A3DDrawingPictureData> BaseDrawingPicture;
	class DrawingPicture : public BaseDrawingPicture
	{
	public:
		DrawingPicture(
			A3DDrawingPicture * entity)
			: BaseDrawingPicture(entity, A3DDrawingPictureGet)
		{
			A3D_INITIALIZE_DATA(A3DDrawingPictureData, data);
			getData();
		}
	};

	typedef ExchangePointerObject<A3DDrawingVertices, A3DDrawingVerticesData> BaseDrawingVertices;
	class DrawingVertices : public BaseDrawingVertices
	{
	public:
		DrawingVertices(
			A3DDrawingVertices * entity)
			: BaseDrawingVertices(entity, A3DDrawingVerticesGet)
		{
			A3D_INITIALIZE_DATA(A3DDrawingVerticesData, data);
			getData();
		}
	};

	typedef ExchangePointerObject<A3DDrawingView, A3DDrawingViewData> BaseDrawingView;
	class DrawingView : public BaseDrawingView
	{
	public:
		DrawingView(
			A3DDrawingView * entity)
			: BaseDrawingView(entity, A3DDrawingViewGet)
		{
			A3D_INITIALIZE_DATA(A3DDrawingViewData, data);
			getData();
		}
	};

	typedef ExchangePointerObject<A3DDrawingClipFrame, A3DDrawingClipFrameData> BaseDrawingClipFrame;
	class DrawingClipFrame : public BaseDrawingClipFrame
	{
	public:
		DrawingClipFrame(
			A3DDrawingClipFrame * entity)
			: BaseDrawingClipFrame(entity, A3DDrawingClipFrameGet)
		{
			A3D_INITIALIZE_DATA(A3DDrawingClipFrameData, data);
			getData();
		}
	};

	typedef ExchangePointerObject<A3DCrvBlend02Boundary, A3DCrvBlend02BoundaryData> BaseCrvBlend02Boundary;
	class CrvBlend02Boundary : public BaseCrvBlend02Boundary
	{
	public:
		CrvBlend02Boundary(
			A3DCrvBlend02Boundary * entity)
			: BaseCrvBlend02Boundary(entity, A3DCrvBlend02BoundaryGet)
		{
			A3D_INITIALIZE_DATA(A3DCrvBlend02BoundaryData, data);
			getData();
		}
	};

	typedef ExchangePointerObject<A3DCrvCircle, A3DCrvCircleData> BaseCrvCircle;
	class CrvCircle : public BaseCrvCircle
	{
	public:
		CrvCircle(
			A3DCrvCircle * entity)
			: BaseCrvCircle(entity, A3DCrvCircleGet)
		{
			A3D_INITIALIZE_DATA(A3DCrvCircleData, data);
			getData();
		}
	};

	typedef ExchangePointerObject<A3DCrvComposite, A3DCrvCompositeData> BaseCrvComposite;
	class CrvComposite : public BaseCrvComposite
	{
	public:
		CrvComposite(
			A3DCrvComposite * entity)
			: BaseCrvComposite(entity, A3DCrvCompositeGet)
		{
			A3D_INITIALIZE_DATA(A3DCrvCompositeData, data);
			getData();
		}
	};

	typedef ExchangePointerObject<A3DCrvEllipse, A3DCrvEllipseData> BaseCrvEllipse;
	class CrvEllipse : public BaseCrvEllipse
	{
	public:
		CrvEllipse(
			A3DCrvEllipse * entity)
			: BaseCrvEllipse(entity, A3DCrvEllipseGet)
		{
			A3D_INITIALIZE_DATA(A3DCrvEllipseData, data);
			getData();
		}
	};

	typedef ExchangePointerObject<A3DCrvHelix, A3DCrvHelixData> BaseCrvHelix;
	class CrvHelix : public BaseCrvHelix
	{
	public:
		CrvHelix(
			A3DCrvHelix * entity)
			: BaseCrvHelix(entity, A3DCrvHelixGet)
		{
			A3D_INITIALIZE_DATA(A3DCrvHelixData, data);
			getData();
		}
	};

	typedef ExchangePointerObject<A3DCrvHyperbola, A3DCrvHyperbolaData> BaseCrvHyperbola;
	class CrvHyperbola : public BaseCrvHyperbola
	{
	public:
		CrvHyperbola(
			A3DCrvHyperbola * entity)
			: BaseCrvHyperbola(entity, A3DCrvHyperbolaGet)
		{
			A3D_INITIALIZE_DATA(A3DCrvHyperbolaData, data);
			getData();
		}
	};

	typedef ExchangePointerObject<A3DCrvIntersection, A3DCrvIntersectionData> BaseCrvIntersection;
	class CrvIntersection : public BaseCrvIntersection
	{
	public:
		CrvIntersection(
			A3DCrvIntersection * entity)
			: BaseCrvIntersection(entity, A3DCrvIntersectionGet)
		{
			A3D_INITIALIZE_DATA(A3DCrvIntersectionData, data);
			getData();
		}
	};

	typedef ExchangePointerObject<A3DCrvLine, A3DCrvLineData> BaseCrvLine;
	class CrvLine : public BaseCrvLine
	{
	public:
		CrvLine(
			A3DCrvLine * entity)
			: BaseCrvLine(entity, A3DCrvLineGet)
		{
			A3D_INITIALIZE_DATA(A3DCrvLineData, data);
			getData();
		}
	};

	typedef ExchangePointerObject<A3DCrvOffset, A3DCrvOffsetData> BaseCrvOffset;
	class CrvOffset : public BaseCrvOffset
	{
	public:
		CrvOffset(
			A3DCrvOffset * entity)
			: BaseCrvOffset(entity, A3DCrvOffsetGet)
		{
			A3D_INITIALIZE_DATA(A3DCrvOffsetData, data);
			getData();
		}
	};

	typedef ExchangePointerObject<A3DCrvOnSurf, A3DCrvOnSurfData> BaseCrvOnSurf;
	class CrvOnSurf : public BaseCrvOnSurf
	{
	public:
		CrvOnSurf(
			A3DCrvOnSurf * entity)
			: BaseCrvOnSurf(entity, A3DCrvOnSurfGet)
		{
			A3D_INITIALIZE_DATA(A3DCrvOnSurfData, data);
			getData();
		}
	};

	typedef ExchangePointerObject<A3DCrvParabola, A3DCrvParabolaData> BaseCrvParabola;
	class CrvParabola : public BaseCrvParabola
	{
	public:
		CrvParabola(
			A3DCrvParabola * entity)
			: BaseCrvParabola(entity, A3DCrvParabolaGet)
		{
			A3D_INITIALIZE_DATA(A3DCrvParabolaData, data);
			getData();
		}
	};

	typedef ExchangePointerObject<A3DCrvPolyLine, A3DCrvPolyLineData> BaseCrvPolyLine;
	class CrvPolyLine : public BaseCrvPolyLine
	{
	public:
		CrvPolyLine(
			A3DCrvPolyLine * entity)
			: BaseCrvPolyLine(entity, A3DCrvPolyLineGet)
		{
			A3D_INITIALIZE_DATA(A3DCrvPolyLineData, data);
			getData();
		}
	};

	typedef ExchangePointerObject<A3DCrvTransform, A3DCrvTransformData> BaseCrvTransform;
	class CrvTransform : public BaseCrvTransform
	{
	public:
		CrvTransform(
			A3DCrvTransform * entity)
			: BaseCrvTransform(entity, A3DCrvTransformGet)
		{
			A3D_INITIALIZE_DATA(A3DCrvTransformData, data);
			getData();
		}
	};

	typedef ExchangePointerObject<A3DTessBase, A3DTessBaseData> BaseTessBase;
	class TessBase : public BaseTessBase
	{
	public:
		TessBase(
			A3DTessBase * entity)
			: BaseTessBase(entity, A3DTessBaseGet)
		{
			A3D_INITIALIZE_DATA(A3DTessBaseData, data);
			getData();
		}
	};

	typedef ExchangePointerObject<A3DTess3DWire, A3DTess3DWireData> BaseTess3DWire;
	class Tess3DWire : public BaseTess3DWire
	{
	public:
		Tess3DWire(
			A3DTess3DWire * entity)
			: BaseTess3DWire(entity, A3DTess3DWireGet)
		{
			A3D_INITIALIZE_DATA(A3DTess3DWireData, data);
			getData();
		}
	};

	typedef ExchangePointerObject<A3DTessMarkup, A3DTessMarkupData> BaseTessMarkup;
	class TessMarkup : public BaseTessMarkup
	{
	public:
		TessMarkup(
			A3DTessMarkup * entity)
			: BaseTessMarkup(entity, A3DTessMarkupGet)
		{
			A3D_INITIALIZE_DATA(A3DTessMarkupData, data);
			getData();
		}
	};

	typedef ExchangePointerObject<A3DRiRepresentationItem, A3DRiRepresentationItemData> BaseRiRepresentationItem;
	class RiRepresentationItem : public BaseRiRepresentationItem
	{
	public:
		RiRepresentationItem(
			A3DRiRepresentationItem * entity)
			: BaseRiRepresentationItem(entity, A3DRiRepresentationItemGet)
		{
			A3D_INITIALIZE_DATA(A3DRiRepresentationItemData, data);
			getData();
		}
	};

	typedef ExchangeIndexObject<A3DUns32, A3DGraphHatchingPatternData> BaseGraphHatchingPattern;
	class GraphHatchingPattern : public BaseGraphHatchingPattern
	{
	public:
		GraphHatchingPattern(
			A3DUns32 index)
			: BaseGraphHatchingPattern(index, A3DGlobalGetGraphHatchingPatternData)
		{
			A3D_INITIALIZE_DATA(A3DGraphHatchingPatternData, data);
			if(index != A3D_DEFAULT_PATTERN_INDEX)
				getData();
		}
	};

	typedef ExchangePointerObject<A3DAsmModelFile, A3DAsmModelFileData> BaseAsmModelFile;
	class AsmModelFile : public BaseAsmModelFile
	{
	public:
		AsmModelFile(
			A3DAsmModelFile * entity)
			: BaseAsmModelFile(entity, A3DAsmModelFileGet)
		{
			A3D_INITIALIZE_DATA(A3DAsmModelFileData, data);
			getData();
		}
	};

	typedef ExchangePointerObject<A3DAsmProductOccurrence, A3DAsmProductOccurrenceData> BaseAsmProductOccurrence;
	class AsmProductOccurrence : public BaseAsmProductOccurrence
	{
	public:
		AsmProductOccurrence(
			A3DAsmProductOccurrence * entity)
			: BaseAsmProductOccurrence(entity, A3DAsmProductOccurrenceGet)
		{
			A3D_INITIALIZE_DATA(A3DAsmProductOccurrenceData, data);
			getData();
		}
	};

	typedef ExchangePointerObject<A3DAsmPartDefinition, A3DAsmPartDefinitionData> BaseAsmPartDefinition;
	class AsmPartDefinition : public BaseAsmPartDefinition
	{
	public:
		AsmPartDefinition(
			A3DAsmPartDefinition * entity)
			: BaseAsmPartDefinition(entity, A3DAsmPartDefinitionGet)
		{
			A3D_INITIALIZE_DATA(A3DAsmPartDefinitionData, data);
			getData();
		}
	};

	typedef ExchangePointerObject<A3DRootBase, A3DRootBaseData> BaseRootBase;
	class RootBase : public BaseRootBase
	{
	public:
		RootBase(
			A3DRootBase * entity)
			: BaseRootBase(entity, A3DRootBaseGet)
		{
			A3D_INITIALIZE_DATA(A3DRootBaseData, data);
			getData();
		}
	};

	typedef ExchangePointerObject<A3DMkpMarkup, A3DMkpMarkupData> BaseMkpMarkup;
	class MkpMarkup : public BaseMkpMarkup
	{
	public:
		MkpMarkup(
			A3DMkpMarkup * entity)
			: BaseMkpMarkup(entity, A3DMkpMarkupGet)
		{
			A3D_INITIALIZE_DATA(A3DMkpMarkupData, data);
			getData();
		}
	};

	typedef ExchangePointerObject<A3DMarkupRichText, A3DMarkupRichTextData> BaseMarkupRichText;
	class MarkupRichText : public BaseMarkupRichText
	{
	public:
		MarkupRichText(
			A3DMarkupRichText * entity)
			: BaseMarkupRichText(entity, A3DMarkupRichTextGet)
		{
			A3D_INITIALIZE_DATA(A3DMarkupRichTextData, data);
			getData();
		}
	};

	typedef ExchangePointerObject<A3DMarkupDefinition, A3DMarkupDefinitionData> BaseMarkupDefinition;
	class MarkupDefinition : public BaseMarkupDefinition
	{
	public:
		MarkupDefinition(
			A3DMarkupDefinition * entity)
			: BaseMarkupDefinition(entity, A3DMarkupDefinitionGet)
		{
			A3D_INITIALIZE_DATA(A3DMarkupDefinitionData, data);
			getData();
		}
	};

	typedef ExchangePointerObject<A3DMDPosition2D, A3DMDPosition2DData> BaseMDPosition2D;
	class MDPosition2D : public BaseMDPosition2D
	{
	public:
		MDPosition2D(
			A3DMDPosition2D * entity)
			: BaseMDPosition2D(entity, A3DMDPosition2DGet)
		{
			A3D_INITIALIZE_DATA(A3DMDPosition2DData, data);
			getData();
		}
	};

	typedef ExchangeIndexObject<A3DUns32, A3DGraphLinePatternData> BaseGraphLinePattern;
	class GraphLinePattern : public BaseGraphLinePattern
	{
	public:
		GraphLinePattern(
			A3DUns32 index)
			: BaseGraphLinePattern(index, A3DGlobalGetGraphLinePatternData)
		{
			A3D_INITIALIZE_DATA(A3DGraphLinePatternData, data);
			if(index != A3D_DEFAULT_LINEPATTERN_INDEX)
				getData();
		}
	};

	typedef ExchangeIndexObject<A3DUns32, A3DGraphPictureData> BaseGraphPicture;
	class GraphPicture : public BaseGraphPicture
	{
	public:
		GraphPicture(
			A3DUns32 index)
			: BaseGraphPicture(index, A3DGlobalGetGraphPictureData)
		{
			A3D_INITIALIZE_DATA(A3DGraphPictureData, data);
			if(index != A3D_DEFAULT_PICTURE_INDEX)
				getData();
		}
	};

	typedef ExchangeIndexObject<A3DUns32, A3DGraphTextureDefinitionData> BaseGraphTextureDefinition;
	class GraphTextureDefinition : public BaseGraphTextureDefinition
	{
	public:
		GraphTextureDefinition(
			A3DUns32 index)
			: BaseGraphTextureDefinition(index, A3DGlobalGetGraphTextureDefinitionData)
		{
			A3D_INITIALIZE_DATA(A3DGraphTextureDefinitionData, data);
			if(index != A3D_DEFAULT_TEXTURE_DEFINITION_INDEX)
				getData();
		}
	};


	class MiscCascadedAttributes
	{
	public:
		MiscCascadedAttributes(
			A3DRootBaseWithGraphics const * entity,
			A3DMiscCascadedAttributes const * fatherAttr)
			: entity(entity)
			, attr(nullptr)
			, validData(false)
		{
			A3D_INITIALIZE_DATA(A3DMiscCascadedAttributesData, data);

			A3DStatus status = A3DMiscCascadedAttributesCreate(&attr);
			if(status == A3D_SUCCESS)
			{
				status = A3DMiscCascadedAttributesPush(attr, entity, fatherAttr);
				if(status == A3D_SUCCESS)
				{
					status = A3DMiscCascadedAttributesGet(attr, &data);
					validData = (status == A3D_SUCCESS);
				}
			}
		}

		~MiscCascadedAttributes()
		{
			if(validData)
				A3DMiscCascadedAttributesGet(nullptr, &data);
			if(attr)
				A3DMiscCascadedAttributesDelete(attr);
		}

		operator A3DMiscCascadedAttributes * () const { return attr; }

		A3DMiscCascadedAttributesData const * operator->() const { return &data; }
		A3DMiscCascadedAttributesData * operator->() { return &data; }

		bool IsValid() const { return attr != nullptr && validData; }

		A3DRootBaseWithGraphics const * entity;
		A3DMiscCascadedAttributes * attr;
		bool validData;
		A3DMiscCascadedAttributesData data;
	};

}
