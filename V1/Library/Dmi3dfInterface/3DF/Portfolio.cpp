#include "StdAfx.h"

#include "Portfolio.h"

#include "Impl/Portfolio.Impl.h"

#include "Segment.h"
#include "./Impl/SegmentImpl.h"

#include "./Impl/ControlImpl.h"
#include "./Impl/ImageImpl.h"
#include "./Impl/TextureImpl.h"
#include "./Impl/DefinitionImpl.h"
#include "./Impl/Shape.Impl.h"

#include "Style.h"
#include "Visibility.h"

#include <HTools.h>

using namespace H3DF;

H3DF::PortfolioKey::PortfolioKey()
{
}

H3DF::PortfolioKey::PortfolioKey(HC_KEY nInKey) : Key(nInKey)
{
}

H3DF::PortfolioKey::PortfolioKey(Key const & cInThat) : Key(cInThat) 
{
}

H3DF::PortfolioKey::PortfolioKey(PortfolioKey const & cInThat) : Key(cInThat)
{
}

void H3DF::PortfolioKey::Set(PortfolioKey const & cInThat)
{
	Key::Set(cInThat);
}

PortfolioKey & H3DF::PortfolioKey::operator = (PortfolioKey const & cInThat)
{
	Key::Set(cInThat);
	return *this;
}

TextureDefinition H3DF::PortfolioKey::DefineTexture(CStringA strName, ImageDefinition const & cInSource)
{
	TextureOptionsKit cTextureOptions;
	return DefineTexture(strName, cInSource, cTextureOptions);
}

TextureDefinition H3DF::PortfolioKey::DefineTexture(CStringA strName, ImageDefinition const & cInSource, TextureOptionsKit const & cInOptions)
{
	StyleKey cStyle(KeyValue());
	HC_KEY nImageKey = INVALID_KEY;

	// PortfolioKey는 StyleKey가 저장되어 있는 것임.
	SegmentKey cPortfolio;
	cStyle.ShowSource(cPortfolio);

	SegmentKey cPortfolioTextures = cPortfolio.Subsegment("textures");
	cPortfolio.GetStyleControl().PushSegment(cPortfolioTextures);

	const ImageDefinitionImpl * pcImageDefinitionImpl = static_cast<const ImageDefinitionImpl *> (cInSource.GetImpl());
	DEBUG_VALID(pcImageDefinitionImpl);

	CStringA strDefinition, strText;

	// Source 정의
	strText.Format("source = %s", pcImageDefinitionImpl->m_strSource);
	strDefinition += strText;

	// Texture Options 정의
	const TextureOptionsKitImpl * pcTextureOptionsKitImpl = static_cast<const TextureOptionsKitImpl *> (cInOptions.GetImpl());
	DEBUG_VALID(pcTextureOptionsKitImpl);

	CStringA strTextureOptionsDefinition;
	pcTextureOptionsKitImpl->GetDefinitionString(strTextureOptionsDefinition);

	MatrixKit cTransform;
	bool bTransformFlag = cInOptions.ShowTransformMatrix(cTransform);
	
	strDefinition += strDefinition.IsEmpty() ? "" : ", ";
	strDefinition += strTextureOptionsDefinition;

	cPortfolioTextures.Open(); {
		if (true == bTransformFlag) {
			HC_Set_Texture_Matrix(cTransform.m_fData);
		}

		HC_Define_Local_Texture(strName, strDefinition);
	} cPortfolioTextures.Close();

	// TextureDefinition 생성 
	TextureDefinition cDefinition;
	TextureDefinitionImpl * pcDefinitionImpl = static_cast<TextureDefinitionImpl *> (cDefinition.GetImpl());
	DEBUG_VALID(pcDefinitionImpl);

	pcDefinitionImpl->m_strName = strName;

	return cDefinition;
}

ImageDefinition H3DF::PortfolioKey::DefineImage(CStringA strInName, ImageKit const & cInSource)
{
	if (INVALID_KEY == KeyValue()) {
		DEBUG_STOP;
	}

	StyleKey cStyle(KeyValue());
	HC_KEY nImageKey = INVALID_KEY;

	// PortfolioKey는 StyleKey가 저장되어 있는 것임.
	SegmentKey cPortfolio;
	cStyle.ShowSource(cPortfolio);

	SegmentKey cPortfolioImages = cPortfolio.Subsegment("images");
	cPortfolioImages.GetVisibilityControl().SetGeometry(false);

	Image::Format eFormat;
	if (false == cInSource.ShowFormat(eFormat)) {
		DEBUG_STOP;
	}

	CStringA strFormat = H3DF::PortfolioKeyImpl::FormatString(eFormat);

	double x = 0, y = 0, z = 0;
	
	UINT nHeight, nWidth;
	if (false == cInSource.ShowSize(nHeight, nWidth)) {
		DEBUG_STOP;
	}

	ByteArray arData;
	if (false == cInSource.ShowData(arData)) {
		DEBUG_STOP;
	}

	CStringA strImageSpace;
	strImageSpace = strFormat;

	if (false == strInName.IsEmpty()) {
		strImageSpace += ", name = " + strInName;
	}

	//strImageSpace += ", local";

	cPortfolioImages.Open(); {
		if (Image::Format::RGB <= eFormat && eFormat <= Image::Format::Bmp) {
			nImageKey = HC_Insert_Image(x, y, z, strImageSpace, (int) nWidth, (int) nHeight, arData.data());
		}
		else {
			nImageKey = HC_Insert_Compressed_Image(x, y, z, strImageSpace, (int) nWidth, (int) nHeight, (int) arData.size(), arData.data());
		}
	} cPortfolioImages.Close();

	ImageDefinition cDefinition;
	cDefinition.Set(cInSource);

	ImageDefinitionImpl * pcDefinitionImpl = static_cast<ImageDefinitionImpl *>(cDefinition.GetImpl());
	DEBUG_VALID(pcDefinitionImpl);

	pcDefinitionImpl->SetKeyValue(nImageKey);
	pcDefinitionImpl->m_strSource = strInName;

	return cDefinition;
}

NamedStyleDefinition H3DF::PortfolioKey::DefineNamedStyle(CStringA strInName, SegmentKey const & cInStyleSource)
{
	if (INVALID_KEY == KeyValue()) {
		DEBUG_STOP;
	}

	SegmentKey cPortfolio(KeyValue());

	SegmentKey cStyleSegment = cPortfolio.Subsegment(strInName);

	cStyleSegment.Open();
	HC_KEY nStyleKey = HC_Style_Segment_By_Key(cInStyleSource.KeyValue());
	cStyleSegment.Close();

	NamedStyleDefinition cStyle(nStyleKey);
	return cStyle;
}

ShapeDefinition H3DF::PortfolioKey::DefineShape(CStringA strInName, ShapeKit const & cInSource)
{
	if (INVALID_KEY == KeyValue()) {
		DEBUG_STOP;
	}

	SegmentKey cPortfolio(KeyValue());

	CStringA strName = cPortfolio.Name();

	// Portpolio
	SegmentKey cShapesSegment = cPortfolio.Subsegment("shapes");

	SegmentKey cStyleSegment = cShapesSegment.Subsegment(strInName);

	ShapeElementArray arShapeElements;
	if (false == cInSource.ShowElements(arShapeElements)) {
		DEBUG_STOP;
	}

	std::vector<float> vfData;

	PortfolioKeyImpl::CreateShapeData(arShapeElements, vfData);

	cStyleSegment.Open(); {
		HC_Define_Shape(strInName, (int)vfData.size(), vfData.data());
	} cStyleSegment.Close();

	ShapeDefinition cDefinition;
	ShapeDefinitionImpl * pcShapeImpl = static_cast<ShapeDefinitionImpl *>(cDefinition.GetImpl());
	DEBUG_VALID(pcShapeImpl);

	pcShapeImpl->m_nKey = cStyleSegment.KeyValue();
	pcShapeImpl->m_cOwnerPortfolio = *this;
	pcShapeImpl->m_strName = strInName;

/*
	float const clipped[] = {
		3,  
		8, 1,  0, 1,  0, 1,  0, 1, 0, 0,  0,  1, 1, -1, 0, 0,  0,  1, 1, -1, 0, -1, 0, 1, 0,
							 -1, 0, -1, 0, -1, 0, -1, 0, 0, 0, -1, -1, 1, 0, 0,  0, -1, -1, 1, 0, 1,  0, -1, 0, 0 };
	HC_Define_Shape("clipped", countof(clipped), clipped);

	HC_Open_Segment("clipped");
	HC_Set_Text_Font("background=shape=clipped");
	HC_Insert_Text(-0.8, 0.8, 0, "corner trimmed box");
	HC_Close_Segment();

	HC_Define_Shape(strInName, )*/
  

	return cDefinition;
}

PortfolioKey & H3DF::PortfolioKey::UndefineShape(CStringA strInName)
{
	if (INVALID_KEY == KeyValue()) {
		DEBUG_STOP;
	}

	return *this;
}


//== PortfolioControlImpl 관련 함수 ==================================================================

namespace H3DF
{
	class PortfolioControlImpl : public ControlImpl
	{
	public:
		PortfolioControlImpl();
		virtual ~PortfolioControlImpl();

		void Copy(PortfolioControlImpl * pcInThat) {
			ControlImpl::Copy(pcInThat);
		}
	};
}

H3DF::PortfolioControlImpl::PortfolioControlImpl()
{
	m_eType = H3DF::Type::PortfolioControl;
}

H3DF::PortfolioControlImpl::~PortfolioControlImpl()
{
}

//== PortfolioControl 관련 함수 ======================================================================

H3DF::PortfolioControl::PortfolioControl(SegmentKey & cInSegmentKey)
{
	PortfolioControlImpl * pcImpl = new PortfolioControlImpl();
	pcImpl->m_cOverrideKey = cInSegmentKey;

	m_pcImpl = pcImpl;
}

H3DF::PortfolioControl::PortfolioControl(PortfolioControl const & cInThat)
{
	m_pcImpl = new PortfolioControlImpl();
	Set(cInThat);
}

void H3DF::PortfolioControl::Set(PortfolioControl const & cInThat)
{
	PortfolioControlImpl * pcImpl = (PortfolioControlImpl *)m_pcImpl;
	PortfolioControlImpl * pcInThatImpl = (PortfolioControlImpl *)cInThat.m_pcImpl;
	pcImpl->Copy(pcInThatImpl);
}

PortfolioControl & H3DF::PortfolioControl::operator = (PortfolioControl const & cInThat)
{
	Set(cInThat);
	return *this;
}

size_t H3DF::PortfolioControl::GetCount() const
{
	PortfolioControlImpl * pcImpl = (PortfolioControlImpl *)m_pcImpl;
	DEBUG_VALID(pcImpl);

	PortfolioKeyArray arPortfolios;
	Show(arPortfolios);

	return arPortfolios.size();
}

PortfolioControl & H3DF::PortfolioControl::Push(PortfolioKey const & cInPortfolio)
{
	PortfolioControlImpl * pcImpl = (PortfolioControlImpl *)m_pcImpl;
	DEBUG_VALID(pcImpl);

	SegmentKeyImpl::LocalOpen(pcImpl->m_cOverrideKey); {
		HC_KEY nKey = HC_Style_Segment_By_Key(cInPortfolio.KeyValue());
	} SegmentKeyImpl::LocalClose(pcImpl->m_cOverrideKey);

	return *this;
}

bool H3DF::PortfolioControl::Pop()
{
	PortfolioControlImpl * pcImpl = (PortfolioControlImpl *)m_pcImpl;
	DEBUG_VALID(pcImpl);

	PortfolioKeyArray arPortfolios;
	Show(arPortfolios);

	if (true == arPortfolios.empty()) {
		return false;
	}

	HC_KEY nKey = arPortfolios.front().KeyValue();

	SegmentKeyImpl::LocalOpen(pcImpl->m_cOverrideKey); {
		HC_Delete_By_Key(nKey);
	} SegmentKeyImpl::LocalClose(pcImpl->m_cOverrideKey);
	
	return true;
}
bool H3DF::PortfolioControl::Pop(PortfolioKey & cOutPortfolio)
{
	PortfolioControlImpl * pcImpl = (PortfolioControlImpl *) m_pcImpl;
	DEBUG_VALID(pcImpl);

	PortfolioKeyArray arPortfolios;
	Show(arPortfolios);

	if (true == arPortfolios.empty()) {
		return false;
	}

	cOutPortfolio = arPortfolios.front();
	HC_KEY nKey = arPortfolios.front().KeyValue();

	SegmentKeyImpl::LocalOpen(pcImpl->m_cOverrideKey); {
		HC_Delete_By_Key(nKey);
	} SegmentKeyImpl::LocalClose(pcImpl->m_cOverrideKey);

	return true;
}

PortfolioControl & H3DF::PortfolioControl::Set(PortfolioKey const & cInPortfolio)
{
	PortfolioControlImpl * pcImpl = (PortfolioControlImpl *) m_pcImpl;
	DEBUG_VALID(pcImpl);

	UnsetEverything();

	SegmentKeyImpl::LocalOpen(pcImpl->m_cOverrideKey); {
		HC_KEY nKey = HC_Style_Segment_By_Key(cInPortfolio.KeyValue());
	} SegmentKeyImpl::LocalClose(pcImpl->m_cOverrideKey);

	return *this;
}

PortfolioControl & H3DF::PortfolioControl::Set(PortfolioKeyArray const & cInPortfolios)
{
	PortfolioControlImpl * pcImpl = (PortfolioControlImpl *) m_pcImpl;
	DEBUG_VALID(pcImpl);

	UnsetEverything();

	SegmentKeyImpl::LocalOpen(pcImpl->m_cOverrideKey); {
		for (auto & cPortfolio : cInPortfolios) {
			HC_KEY nKey = HC_Style_Segment_By_Key(cPortfolio.KeyValue());
		}
	} SegmentKeyImpl::LocalClose(pcImpl->m_cOverrideKey);

	return *this;
}

PortfolioControl & H3DF::PortfolioControl::UnsetTop()
{
	Pop();
	return *this;
}

PortfolioControl & H3DF::PortfolioControl::UnsetEverything()
{
	PortfolioControlImpl * pcImpl = static_cast<PortfolioControlImpl *>(m_pcImpl);
	DEBUG_VALID(pcImpl);

	PortfolioKeyArray arPortfolios;
	Show(arPortfolios);

	SegmentKeyImpl::LocalOpen(pcImpl->m_cOverrideKey); {
		for (auto & cPortfolio : arPortfolios) {
			HC_Delete_By_Key(cPortfolio.KeyValue());
		}
	} SegmentKeyImpl::LocalClose(pcImpl->m_cOverrideKey);

	return *this;
}

bool H3DF::PortfolioControl::ShowTop(PortfolioKey & cOutPortfolio) const
{
	PortfolioKeyArray arPortfolios;
	Show(arPortfolios);

	if (true == arPortfolios.empty()) {
		return false;
	}

	cOutPortfolio = arPortfolios.front();

	return true;
}

bool H3DF::PortfolioControl::Show(PortfolioKeyArray & cOutPortfolios) const
{
	PortfolioControlImpl * pcImpl = static_cast<PortfolioControlImpl *>(m_pcImpl);
	DEBUG_VALID(pcImpl);

	SegmentKey cModelSegment;

	// Model을 찾는다.
	if (H3DF::Type::Model == pcImpl->m_cOverrideKey.Type()) {
		cModelSegment = pcImpl->m_cOverrideKey;
	}
	else {
		SegmentKeyImpl::FindUp(pcImpl->m_cOverrideKey, H3DF::Type::Model, cModelSegment);
	}

	if (false == cModelSegment.IsValidate()) {
		return false;
	}

	SearchResults cResults;
	cModelSegment.Find(Search::Type::SegmentStyle, Search::Space::SegmentOnly, cResults);

	if (0 == cResults.GetCount()) {
		return false;
	}

	CStringA strModelSegmentName = cModelSegment.Name();

	CStringA strPortfoliosText = strModelSegmentName + "/portfolios";

	SearchResultsIterator cIter = cResults.GetIterator();

	while (true == cIter.IsValid()) {
		if (H3DF::Type::SegmentStyle != cIter.GetItem().Type()) {
			DEBUG_STOP;
		}

		H3DF::Style::Type eSoruceType;
		H3DF::SegmentKey cSource;
		CStringA strSourceName;

		StyleKey cStyle(cIter.GetItem());
		cStyle.ShowSource(eSoruceType, cSource, strSourceName);

		if (0 == strSourceName.Left(strPortfoliosText.GetLength()).Compare(strPortfoliosText)) {
			cOutPortfolios.emplace_back(cSource.KeyValue());
		}

		cIter.Next();
	}

	return true;
}