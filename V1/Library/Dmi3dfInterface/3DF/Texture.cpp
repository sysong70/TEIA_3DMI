#include <StdAfx.h>

#include "Texture.h"
#include "Impl/TextureImpl.h"

using namespace H3DF;

//== TextureOptionsKit class =======================================================================
H3DF::TextureOptionsKit::TextureOptionsKit()
{
	m_pcImpl = new TextureOptionsKitImpl();
	DEBUG_VALID(m_pcImpl);
}

H3DF::TextureOptionsKit::TextureOptionsKit(TextureOptionsKit const & cInKit)
{
	m_pcImpl = new TextureOptionsKitImpl();
	DEBUG_VALID(m_pcImpl);

	Set(cInKit);
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

void H3DF::TextureOptionsKit::Set(TextureOptionsKit const & cInKit)
{
	TextureOptionsKitImpl * pcImpl = static_cast<TextureOptionsKitImpl *> (m_pcImpl);
	DEBUG_VALID(pcImpl);

	TextureOptionsKitImpl * pccInKitImpl = static_cast<TextureOptionsKitImpl *>(cInKit.m_pcImpl);
	DEBUG_VALID(pccInKitImpl);

	pcImpl->Copy(pccInKitImpl);

}

TextureOptionsKit const & H3DF::TextureOptionsKit::operator = (TextureOptionsKit const & cInKit)
{
	Set(cInKit);
	return *this;
}

void H3DF::TextureOptionsKit::Show(TextureOptionsKit & cOutKit) const
{
	cOutKit = *this;
}

bool H3DF::TextureOptionsKit::Empty() const
{
	TextureOptionsKitImpl * pcImpl = static_cast<TextureOptionsKitImpl *> (m_pcImpl);
	DEBUG_VALID(pcImpl);

	if (pcImpl->m_eParameterization != Material::Texture::Parameterization::None) {
		return false;
	}

	return true;
}

bool H3DF::TextureOptionsKit::operator == (TextureOptionsKit const & cInKit) const
{
	TextureOptionsKitImpl * pcImpl = static_cast<TextureOptionsKitImpl *> (m_pcImpl);
	DEBUG_VALID(pcImpl);

	TextureOptionsKitImpl * pcInKitImpl = static_cast<TextureOptionsKitImpl *>(cInKit.m_pcImpl);
	DEBUG_VALID(pcInKitImpl);

	if (pcImpl->m_eParameterization != pcInKitImpl->m_eParameterization) {
		return false;
	}

	return true;
}

bool H3DF::TextureOptionsKit::operator != (TextureOptionsKit const & cInKit) const
{
	return !(*this == cInKit);
}

TextureOptionsKit & H3DF::TextureOptionsKit::SetParameterizationSource(Material::Texture::Parameterization cInSource)
{
	TextureOptionsKitImpl * pcImpl = static_cast<TextureOptionsKitImpl *> (m_pcImpl);
	DEBUG_VALID(pcImpl);

	pcImpl->m_eParameterization = cInSource;

	return *this;
}


bool H3DF::TextureOptionsKit::ShowParameterizationSource(Material::Texture::Parameterization & cOutSource) const
{
	TextureOptionsKitImpl * pcImpl = static_cast<TextureOptionsKitImpl *> (m_pcImpl);
	DEBUG_VALID(pcImpl);

	if (Material::Texture::Parameterization::None == pcImpl->m_eParameterization) {
		return false;
	}

	cOutSource = pcImpl->m_eParameterization;

	return true;
}

//== TextureDefinition class =======================================================================
H3DF::TextureDefinition::TextureDefinition()
{
	m_pcImpl = new TextureDefinitionImpl();
}

H3DF::TextureDefinition::TextureDefinition(Definition const & cInThat)
{
	if (H3DF::Type::TextureDefinition != cInThat.Type())
	{
		DEBUG_STOP;
		return;
	}

	m_pcImpl = new TextureDefinitionImpl();
	Set(cInThat);
}

H3DF::TextureDefinition::TextureDefinition(TextureDefinition const & cInThat)
{
	m_pcImpl = new TextureDefinitionImpl();
	Set(cInThat);
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

void H3DF::TextureDefinition::Set(TextureDefinition const & cInKit)
{
	TextureDefinitionImpl * pcImpl = static_cast<TextureDefinitionImpl *> (m_pcImpl);

	DEBUG_VALID(pcImpl);
	TextureDefinitionImpl * pcInKitImpl = static_cast<TextureDefinitionImpl *>(cInKit.m_pcImpl);
	DEBUG_VALID(pcInKitImpl);

	pcImpl->Copy(pcInKitImpl);
}

TextureDefinition const & H3DF::TextureDefinition::operator = (TextureDefinition const & cInKit)
{
	Set(cInKit);
	return *this;
}
