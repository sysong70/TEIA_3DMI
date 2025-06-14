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
	m_pcImpl = std::make_unique<PortfolioKeyImpl>();
	DEBUG_VALID(m_pcImpl);
}

H3DF::PortfolioKey::PortfolioKey(HC_KEY nInKey)
{
	if (INVALID_KEY == nInKey) {
		return;
	}

	m_pcImpl = std::make_unique<PortfolioKeyImpl>();
	DEBUG_VALID(m_pcImpl);

	static_cast<PortfolioKeyImpl *>(m_pcImpl.get())->SetKeyValue(nInKey);
}

H3DF::PortfolioKey::PortfolioKey(Key const & cInThat)
{
	// cInThat이 올바른 Impl(PortfolioKeyImpl)을 가지고 있으면 복제
	if (cInThat.GetImpl()) {
		// 만약 PortfolioKeyImpl이 KeyImpl에서 파생된 구조라면 dynamic_cast에 의해서 nullptr이 아닌 정상적인 값이 넘어옴
		auto pcSrcImpl = dynamic_cast<const PortfolioKeyImpl *>(cInThat.GetImpl());
		if (nullptr != pcSrcImpl) {
			m_pcImpl = pcSrcImpl->Clone();
		}
		else {
			// 타입이 다를 경우 예외 처리 또는 방어적 초기화
			m_pcImpl = std::make_unique<PortfolioKeyImpl>();
			static_cast<PortfolioKeyImpl *>(m_pcImpl.get())->Copy((PortfolioKeyImpl *) (cInThat.GetImpl()));
		}
	}
	else {
		m_pcImpl = std::make_unique<PortfolioKeyImpl>();
		static_cast<PortfolioKeyImpl *>(m_pcImpl.get())->Copy((PortfolioKeyImpl *) (cInThat.GetImpl()));
	}
}

H3DF::PortfolioKey::PortfolioKey(PortfolioKey const & cInThat)
{
	m_pcImpl = (nullptr != cInThat.m_pcImpl) ? cInThat.m_pcImpl->Clone() : nullptr;
}

PortfolioKey & H3DF::PortfolioKey::operator = (PortfolioKey const & cInThat)
{
	if (nullptr != cInThat.m_pcImpl) {
		m_pcImpl = cInThat.m_pcImpl->Clone();
	}
	else {
		m_pcImpl.reset();
	}

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
		std::unique_ptr<Impl> Clone() const override {
			auto pcClone = std::make_unique<PortfolioControlImpl>();
			pcClone->Copy(this);
			return pcClone;
		}

		void Copy(const PortfolioControlImpl * pcInThat) {
			ControlImpl::Copy(pcInThat);
		}
	};
}

//== PortfolioControl 관련 함수 ======================================================================

H3DF::PortfolioControl::PortfolioControl(SegmentKey & cInSegment)
{
	m_pcImpl = std::make_unique<PortfolioControlImpl>();
	DEBUG_VALID(m_pcImpl);

	auto pcImpl = static_cast<PortfolioControlImpl *>(m_pcImpl.get());
	DEBUG_VALID(pcImpl);
	
	pcImpl->m_cOverrideKey = cInSegment;
}

H3DF::PortfolioControl::PortfolioControl(PortfolioControl const & cInThat)
{
	m_pcImpl = (nullptr != cInThat.m_pcImpl) ? cInThat.m_pcImpl->Clone() : nullptr;
}

PortfolioControl & H3DF::PortfolioControl::operator = (PortfolioControl const & cInThat)
{
	if (nullptr != cInThat.m_pcImpl) {
		m_pcImpl = cInThat.m_pcImpl->Clone();
	}
	else {
		m_pcImpl.reset();
	}

	return *this;
}

size_t H3DF::PortfolioControl::GetCount() const
{
	auto pcImpl = static_cast<PortfolioControlImpl *>(m_pcImpl.get());
	DEBUG_VALID(pcImpl);

	PortfolioKeyArray arPortfolios;
	Show(arPortfolios);

	return arPortfolios.size();
}

PortfolioControl & H3DF::PortfolioControl::Push(PortfolioKey const & cInPortfolio)
{
	auto pcImpl = static_cast<PortfolioControlImpl *>(m_pcImpl.get());
	DEBUG_VALID(pcImpl);

	SegmentKeyImpl::LocalOpen(pcImpl->m_cOverrideKey); {
		HC_KEY nKey = HC_Style_Segment_By_Key(cInPortfolio.KeyValue());
	} SegmentKeyImpl::LocalClose(pcImpl->m_cOverrideKey);

	return *this;
}

bool H3DF::PortfolioControl::Pop()
{
	auto pcImpl = static_cast<PortfolioControlImpl *>(m_pcImpl.get());
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
	auto pcImpl = static_cast<PortfolioControlImpl *>(m_pcImpl.get());
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
	auto pcImpl = static_cast<PortfolioControlImpl *>(m_pcImpl.get());
	DEBUG_VALID(pcImpl);

	UnsetEverything();

	SegmentKeyImpl::LocalOpen(pcImpl->m_cOverrideKey); {
		HC_KEY nKey = HC_Style_Segment_By_Key(cInPortfolio.KeyValue());
	} SegmentKeyImpl::LocalClose(pcImpl->m_cOverrideKey);

	return *this;
}

PortfolioControl & H3DF::PortfolioControl::Set(PortfolioKeyArray const & cInPortfolios)
{
	auto pcImpl = static_cast<PortfolioControlImpl *>(m_pcImpl.get());
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
	auto pcImpl = static_cast<PortfolioControlImpl *>(m_pcImpl.get());
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
	auto pcImpl = static_cast<PortfolioControlImpl *>(m_pcImpl.get());
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