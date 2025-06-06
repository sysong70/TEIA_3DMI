#include <StdAfx.h>

#include "Texture.h"
#include "Impl/TextureImpl.h"

using namespace H3DF;

//== TextureOptionsKit class =======================================================================
H3DF::TextureOptionsKit::TextureOptionsKit()
{
	if (staticType != Type()) {
		return;
	}

	m_pcImpl = std::make_unique<TextureOptionsKitImpl>();
	DEBUG_VALID(m_pcImpl);
}

H3DF::TextureOptionsKit::TextureOptionsKit(TextureOptionsKit const & cInKit)
{
	if (staticType != Type()) {
		return;
	}

	m_pcImpl = (nullptr == cInKit.m_pcImpl) ? cInKit.m_pcImpl->Clone() : nullptr;
}

H3DF::TextureOptionsKit::TextureOptionsKit(TextureOptionsKit && cInThat) noexcept : 
	Kit(std::move(cInThat))
{
}

TextureOptionsKit & H3DF::TextureOptionsKit::operator = (TextureOptionsKit && cInThat) noexcept
{
	this->Kit::operator = (std::move(cInThat));
	return *this;
}

TextureOptionsKit const & H3DF::TextureOptionsKit::operator = (TextureOptionsKit const & cInKit)
{
	if (nullptr != cInKit.m_pcImpl) {
		m_pcImpl = cInKit.m_pcImpl->Clone();
	}
	else {
		m_pcImpl.reset();
	}

	return *this;
}

void H3DF::TextureOptionsKit::Show(TextureOptionsKit & cOutKit) const
{
	cOutKit = *this;
}

bool H3DF::TextureOptionsKit::Empty() const
{
	auto pcImpl = static_cast<TextureOptionsKitImpl *>(m_pcImpl.get());
	DEBUG_VALID(pcImpl);

	pcImpl->m_bDecalFlag = false;
	pcImpl->m_bDownSamplingFlag = false;
	pcImpl->m_bModulationFlag = false;
	pcImpl->m_bParameterOffsetFlag = false;
	pcImpl->m_bParameterizationFlag = false;
	pcImpl->m_bTilingFlag = false;
	pcImpl->m_bInterpolationFlag = false;

	return true;
}

bool H3DF::TextureOptionsKit::Equals(TextureOptionsKit const & cInKit) const
{
	auto pcImpl = static_cast<TextureOptionsKitImpl *>(m_pcImpl.get());
	DEBUG_VALID(pcImpl);

	auto pcInKitImpl = static_cast<TextureOptionsKitImpl *>(cInKit.m_pcImpl.get());
	DEBUG_VALID(pcInKitImpl);

	if (pcImpl->m_bDecalFlag != pcInKitImpl->m_bDecalFlag) {
		return false;
	}
	if (pcImpl->m_bDecal != pcInKitImpl->m_bDecal) {
		return false;
	}

	if (pcImpl->m_bDownSamplingFlag != pcInKitImpl->m_bDownSamplingFlag) {
		return false;
	}
	if (pcImpl->m_bDownSampling != pcInKitImpl->m_bDownSampling) {
		return false;
	}

	if (pcImpl->m_bModulationFlag != pcInKitImpl->m_bModulationFlag) {
		return false;
	}
	if (pcImpl->m_bModulation != pcInKitImpl->m_bModulation) {
		return false;
	}

	if (pcImpl->m_bParameterOffsetFlag != pcInKitImpl->m_bParameterOffsetFlag) {
		return false;
	}
	if (pcImpl->m_nParameterOffset != pcInKitImpl->m_nParameterOffset) {
		return false;
	}

	if (pcImpl->m_bParameterizationFlag != pcInKitImpl->m_bParameterizationFlag) {
		return false;
	}
	if (pcImpl->m_eParameterization != pcInKitImpl->m_eParameterization) {
		return false;
	}

	if (pcImpl->m_bTilingFlag != pcInKitImpl->m_bTilingFlag) {
		return false;
	}
	if (pcImpl->m_eTiling != pcInKitImpl->m_eTiling) {
		return false;
	}

	if (pcImpl->m_bInterpolationFlag != pcInKitImpl->m_bInterpolationFlag) {
		return false;
	}
	if (pcImpl->m_eInterpolation != pcInKitImpl->m_eInterpolation) {
		return false;
	}

	return true;
}

bool H3DF::TextureOptionsKit::operator == (TextureOptionsKit const & cInKit) const
{
	return Equals(cInKit);
}

bool H3DF::TextureOptionsKit::operator != (TextureOptionsKit const & cInKit) const
{
	return !Equals(cInKit);
}

TextureOptionsKit & H3DF::TextureOptionsKit::SetDecal(bool bInState)
{
	auto pcImpl = static_cast<TextureOptionsKitImpl *>(m_pcImpl.get());
	DEBUG_VALID(pcImpl);

	pcImpl->m_bDecal = bInState;
	pcImpl->m_bDecalFlag = true;

	return *this;
}

TextureOptionsKit & H3DF::TextureOptionsKit::SetDownSampling(bool bInState)
{
	auto pcImpl = static_cast<TextureOptionsKitImpl *>(m_pcImpl.get());
	DEBUG_VALID(pcImpl);

	pcImpl->m_bDownSampling = bInState;
	pcImpl->m_bDownSamplingFlag = true;

	return *this;

}

TextureOptionsKit & H3DF::TextureOptionsKit::SetModulation(bool bInState)
{
	auto pcImpl = static_cast<TextureOptionsKitImpl *>(m_pcImpl.get());
	DEBUG_VALID(pcImpl);
	
	pcImpl->m_bModulation = bInState;
	pcImpl->m_bModulationFlag = true;

	return *this;

}

TextureOptionsKit & H3DF::TextureOptionsKit::SetParameterOffset(size_t nInSffset)
{
	auto pcImpl = static_cast<TextureOptionsKitImpl *>(m_pcImpl.get());
	DEBUG_VALID(pcImpl);
	
	pcImpl->m_nParameterOffset = nInSffset;
	pcImpl->m_bParameterOffsetFlag = true;

	return *this;
}

TextureOptionsKit & H3DF::TextureOptionsKit::SetParameterizationSource(Material::Texture::Parameterization cInSource)
{
	auto pcImpl = static_cast<TextureOptionsKitImpl *>(m_pcImpl.get());
	DEBUG_VALID(pcImpl);

	pcImpl->m_eParameterization = cInSource;
	pcImpl->m_bParameterizationFlag = true;

	return *this;
}

TextureOptionsKit & H3DF::TextureOptionsKit::SetTiling(Material::Texture::Tiling eInTiling)
{
	auto pcImpl = static_cast<TextureOptionsKitImpl *>(m_pcImpl.get());
	DEBUG_VALID(pcImpl);

	pcImpl->m_eTiling = eInTiling;
	pcImpl->m_bTilingFlag = true;

	return *this;
}

TextureOptionsKit & H3DF::TextureOptionsKit::SetInterpolationFilter(Material::Texture::Interpolation eInFilter)
{
	auto pcImpl = static_cast<TextureOptionsKitImpl *>(m_pcImpl.get());
	DEBUG_VALID(pcImpl);

	pcImpl->m_eInterpolation = eInFilter;
	pcImpl->m_bInterpolationFlag = true;

	return *this;
}

TextureOptionsKit & H3DF::TextureOptionsKit::SetDecimationFilter(Material::Texture::Decimation eInFilter)
{
	auto pcImpl = static_cast<TextureOptionsKitImpl *>(m_pcImpl.get());
	DEBUG_VALID(pcImpl);

	pcImpl->m_eDecimation = eInFilter;
	pcImpl->m_bDecimationFlag = true;

	return *this;
}

TextureOptionsKit & H3DF::TextureOptionsKit::SetTransformMatrix(MatrixKit const & cInTransform)
{
	auto pcImpl = static_cast<TextureOptionsKitImpl *>(m_pcImpl.get());
	DEBUG_VALID(pcImpl);

	pcImpl->m_cTransform = cInTransform;
	pcImpl->m_bTransformFlag = true;

	return *this;
}

TextureOptionsKit & H3DF::TextureOptionsKit::SetValueScale(float fInMin, float fInMax)
{
	auto pcImpl = static_cast<TextureOptionsKitImpl *>(m_pcImpl.get());
	DEBUG_VALID(pcImpl);

	pcImpl->m_fValueScaleMin = fInMin;
	pcImpl->m_fValueScaleMax = fInMax;
	pcImpl->m_bValueScaleFlag = true;

	return *this;
}

bool H3DF::TextureOptionsKit::ShowParameterizationSource(Material::Texture::Parameterization & cOutSource) const
{
	auto pcImpl = static_cast<TextureOptionsKitImpl *>(m_pcImpl.get());
	DEBUG_VALID(pcImpl);

	if (false == pcImpl->m_bParameterizationFlag) {
		return false;
	}

	cOutSource = pcImpl->m_eParameterization;

	return true;
}

bool H3DF::TextureOptionsKit::ShowTransformMatrix(MatrixKit & cOutTransform) const
{
	auto pcImpl = static_cast<TextureOptionsKitImpl *>(m_pcImpl.get());
	DEBUG_VALID(pcImpl);

	if (false == pcImpl->m_bTransformFlag) {
		return false;
	}

	cOutTransform = pcImpl->m_cTransform;
	return true;
}

//== TextureDefinition class =======================================================================
H3DF::TextureDefinition::TextureDefinition()
{
	m_pcImpl = std::make_unique<TextureDefinitionImpl>();
	DEBUG_VALID(m_pcImpl);
}

H3DF::TextureDefinition::TextureDefinition(Definition const & cInThat)
{
	if (staticType != Type()) {
		return;
	}

	// PolygonShapeElementImpl 생성
	m_pcImpl = std::make_unique<TextureDefinitionImpl>();
	auto pcImpl = static_cast<TextureDefinitionImpl *>(m_pcImpl.get());

	auto pcInThatImpl = static_cast<const DefinitionImpl *>(cInThat.GetImpl());

	if (nullptr != pcImpl && nullptr != pcInThatImpl) {
		pcImpl->DefinitionImpl::Copy(pcInThatImpl);
	}
	else {
		DEBUG_STOP;
	}

	if (H3DF::Type::TextureDefinition != cInThat.Type())
	{
		DEBUG_STOP;
		return;
	}
}

H3DF::TextureDefinition::TextureDefinition(TextureDefinition const & cInThat)
{
	if (staticType != Type()) {
		return;
	}

	m_pcImpl = (nullptr == cInThat.m_pcImpl) ? cInThat.m_pcImpl->Clone() : nullptr;
	DEBUG_VALID(m_pcImpl);
}

H3DF::TextureDefinition::TextureDefinition(TextureDefinition && cInThat) noexcept :
	Definition(cInThat)
{

}

TextureDefinition & H3DF::TextureDefinition::operator = (TextureDefinition && cInThat) noexcept
{
	this->Definition::operator = (cInThat);
	return *this;
}

TextureDefinition const & H3DF::TextureDefinition::operator = (TextureDefinition const & cInKit)
{
	if (nullptr != cInKit.m_pcImpl) {
		m_pcImpl = cInKit.m_pcImpl->Clone();
	}
	else {
		m_pcImpl.reset();
	}

	return *this;
}
