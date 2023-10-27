#include "StdAfx.h"

#include "Material.h"

#include "Segment.h"
#include "./Private/SegmentPrivate.h"

#include "3DF.Utility.h"

#include <hc.h>
#include <HUtility.h>
#include <HTools.h>
#include <magick/api.h>
#include <varray.h>

USING_3DF_NAMESPACE

//== MaterialKit ===================================================================================
OPEN_3DF_NAMESPACE
class MaterialKitPrivate : public PrivateImpl
{
public:
	MaterialKitPrivate() { m_eType = H3DF::Type::MaterialKit; }

	void Copy(MaterialKitPrivate * pcInThat) {
		for (int nColorIndex = 0; nColorIndex < (int)Material::Channel::Count; nColorIndex++) {
			m_pcColors[nColorIndex] = pcInThat->m_pcColors[nColorIndex];

			m_bTextureMirrors[nColorIndex] = pcInThat->m_bTextureMirrors[nColorIndex];
			m_strTextureNames[nColorIndex] = pcInThat->m_strTextureNames[nColorIndex];
			m_strTextureOptions[nColorIndex] = pcInThat->m_strTextureOptions[nColorIndex];
		}

		m_fGloss = pcInThat->m_fGloss;
	}

	bool operator == (MaterialKitPrivate const & cInThat) const;

	bool Empty() const;

	bool ShowColor(Material::Channel eInChannel, RGBAColor & cOutColor) const;
	bool ShowAlpah(Material::Channel eInChannel, float & fOutAlpah) const;
	bool ShowTexture(Material::Channel eInChannel, Material::Type & cOutType, RGBAColor & cOutColor, CString & strOutTextureName) const;
	bool ShowChannel(Material::Channel eInChannel, Material::Type & cOutType, RGBAColor & cOutColor, CString & strOutTextureName) const;

	RGBAColor m_pcColors[(int)Material::Channel::Count];
	float m_fGloss = -1.f;

	bool m_bTextureMirrors[(int)Material::Channel::Count];
	CString m_strTextureNames[(int)Material::Channel::Count];;
	CString m_strTextureOptions[(int)Material::Channel::Count];;
};
CLOSE_3DF_NAMESPACE

bool MaterialKitPrivate::operator == (MaterialKitPrivate const & cInThat) const
{
	for (int nColorIndex = 0; nColorIndex < (int)Material::Channel::Count; nColorIndex++) {
		if (m_pcColors[nColorIndex] != cInThat.m_pcColors[nColorIndex]) {
			return false;
		}

		if (m_bTextureMirrors[nColorIndex] != cInThat.m_bTextureMirrors[nColorIndex]) {
			return false;
		}

		if (0 != m_strTextureNames[nColorIndex].CompareNoCase(cInThat.m_strTextureNames[nColorIndex])) {
			return false;
		}

		if (0 != m_strTextureOptions[nColorIndex].CompareNoCase(cInThat.m_strTextureOptions[nColorIndex])) {
			return false;
		}
	}

	if (m_fGloss != cInThat.m_fGloss) {
		return false;
	}

	return true;
}

bool MaterialKitPrivate::Empty() const
{
	for (int nColorIndex = 0; nColorIndex < (int)Material::Channel::Count; nColorIndex++) {
		if (true == m_pcColors[nColorIndex].IsValid()) {
			return false;
		}

		if (false == m_strTextureNames[nColorIndex].IsEmpty()) {
			return false;
		}
	}

	if (m_fGloss > 0.f) {
		return false;
	}

	return true;
}

bool MaterialKitPrivate::ShowColor(Material::Channel eInChannel, RGBAColor & cOutColor) const
{
	if (false == m_pcColors[(int)eInChannel].IsValid()) {
		return false;
	}

	cOutColor = m_pcColors[(int)eInChannel];

	return true;
}

bool MaterialKitPrivate::ShowAlpah(Material::Channel eInChannel, float & fOutAlpah) const
{
	if (0 > m_pcColors[(int)eInChannel].alpha) {
		return false;
	}
	fOutAlpah = m_pcColors[(int)eInChannel].alpha;

	return true;
}

bool MaterialKitPrivate::ShowTexture(Material::Channel eInChannel, Material::Type & cOutType, RGBAColor & cOutColor, CString & strOutTextureName) const
{
	if (true == m_strTextureNames[(int)eInChannel].IsEmpty()) {
		return false;
	}

	cOutType = Material::Type::TextureName;
	strOutTextureName = m_strTextureNames[(int)eInChannel];

	if (true == m_pcColors[(int)eInChannel].IsValid()) {
		cOutType = Material::Type::ModulatedTexture;
		cOutColor = m_pcColors[(int)eInChannel];
	}

	return true;
}

bool MaterialKitPrivate::ShowChannel(Material::Channel eInChannel, Material::Type & cOutType, RGBAColor & cOutColor, CString & strOutTextureName) const
{
	// Texture Name이 없는 경우 처리
	if (true == m_strTextureNames[(int)eInChannel].IsEmpty()) {
		if (false == m_pcColors[(int)eInChannel].IsValid()) {
			return false;
		}

		cOutType = Material::Type::RGBAColor;
		cOutColor = m_pcColors[(int)eInChannel];

		return true;
	}

	cOutType = Material::Type::TextureName;
	strOutTextureName = m_strTextureNames[(int)eInChannel];

	if (true == m_pcColors[(int)eInChannel].IsValid()) {
		cOutType = Material::Type::ModulatedTexture;
		cOutColor = m_pcColors[(int)eInChannel];
	}

	return true;
}

MaterialKit::MaterialKit()
{
	m_pcImpl = new MaterialKitPrivate();
}

MaterialKit::MaterialKit(MaterialKit const & cInKit)
{
	m_pcImpl = new MaterialKitPrivate();
	Set(cInKit);
}

void MaterialKit::Set(MaterialKit const & cInThat)
{
	MaterialKitPrivate * pcImpl = (MaterialKitPrivate *)m_pcImpl;
	MaterialKitPrivate * pcInThatImpl = (MaterialKitPrivate *)cInThat.m_pcImpl;
	pcImpl->Copy(pcInThatImpl);
}

MaterialKit & H3DF::MaterialKit::operator = (MaterialKit const & cInThat)
{
	Set(cInThat);
	return *this;
}

bool H3DF::MaterialKit::operator == (MaterialKit const & cInThat) const
{
	MaterialKitPrivate * pcImpl = (MaterialKitPrivate *)m_pcImpl;
	MaterialKitPrivate * pcInThatImpl = (MaterialKitPrivate *)cInThat.m_pcImpl;

	return (pcImpl == pcInThatImpl);
}

bool H3DF::MaterialKit::operator != (MaterialKit const & cInThat) const
{
	return !(*this == cInThat);
}

void MaterialKit::Show(MaterialKit & cOutKit) const
{
	cOutKit.Set(*this);
}

bool H3DF::MaterialKit::Empty() const
{
	MaterialKitPrivate * pcImpl = (MaterialKitPrivate *)m_pcImpl;
	return pcImpl->Empty();
}

MaterialKit & H3DF::MaterialKit::SetDiffuse(RGBColor const & cInColor)
{
	MaterialKitPrivate * pcImpl = (MaterialKitPrivate *)m_pcImpl;
	pcImpl->m_pcColors[(int)Material::Channel::DiffuseColor] = cInColor;
	pcImpl->m_pcColors[(int)Material::Channel::DiffuseTexture] = cInColor;
	return *this;
}

MaterialKit & H3DF::MaterialKit::SetDiffuse(RGBAColor const & cInColor)
{
	MaterialKitPrivate * pcImpl = (MaterialKitPrivate *)m_pcImpl;
	pcImpl->m_pcColors[(int)Material::Channel::DiffuseColor] = cInColor;
	pcImpl->m_pcColors[(int)Material::Channel::DiffuseTexture] = cInColor;
	return *this;
}

MaterialKit & H3DF::MaterialKit::SetDiffuseColor(RGBColor const & cInColor)
{
	MaterialKitPrivate * pcImpl = (MaterialKitPrivate *)m_pcImpl;
	pcImpl->m_pcColors[(int)Material::Channel::DiffuseColor] = cInColor;
	return *this;
}

MaterialKit & H3DF::MaterialKit::SetDiffuseColor(RGBAColor const & cInColor)
{
	MaterialKitPrivate * pcImpl = (MaterialKitPrivate *)m_pcImpl;
	pcImpl->m_pcColors[(int)Material::Channel::DiffuseColor] = cInColor;
	return *this;
}

MaterialKit & H3DF::MaterialKit::SetDiffuseAlpha(float fInAlpha)
{
	MaterialKitPrivate * pcImpl = (MaterialKitPrivate *)m_pcImpl;
	pcImpl->m_pcColors[(int)Material::Channel::DiffuseColor].alpha = fInAlpha;
	return *this;
}

MaterialKit & H3DF::MaterialKit::SetDiffuseTexture(CString strTextureName)
{
	MaterialKitPrivate * pcImpl = (MaterialKitPrivate *)m_pcImpl;
	pcImpl->m_strTextureNames[(int)Material::Channel::DiffuseTexture] = strTextureName;
	return *this;
}

MaterialKit & H3DF::MaterialKit::SetDiffuseTexture(CString strTextureName, RGBAColor const & cInModulatingColor)
{
	MaterialKitPrivate * pcImpl = (MaterialKitPrivate *)m_pcImpl;
	pcImpl->m_strTextureNames[(int)Material::Channel::DiffuseTexture] = strTextureName;
	pcImpl->m_pcColors[(int)Material::Channel::DiffuseTexture] = cInModulatingColor;
	return *this;
}

MaterialKit & H3DF::MaterialKit::SetDiffuseTextureOption(CString strTextureOption)
{
	MaterialKitPrivate * pcImpl = (MaterialKitPrivate *)m_pcImpl;
	pcImpl->m_strTextureOptions[(int)Material::Channel::DiffuseTexture] = strTextureOption;
	return *this;
}

MaterialKit & H3DF::MaterialKit::SetSpecular(RGBAColor const & cInColor)
{
	MaterialKitPrivate * pcImpl = (MaterialKitPrivate *)m_pcImpl;
	pcImpl->m_pcColors[(int)Material::Channel::Specular] = cInColor;
	return *this;
}

MaterialKit & H3DF::MaterialKit::SetSpecular(CString strTextureName, RGBAColor const & cInModulatingColor)
{
	MaterialKitPrivate * pcImpl = (MaterialKitPrivate *)m_pcImpl;
	pcImpl->m_strTextureNames[(int)Material::Channel::Specular] = strTextureName;
	pcImpl->m_pcColors[(int)Material::Channel::Specular] = cInModulatingColor;
	return *this;
}

MaterialKit & H3DF::MaterialKit::SetMirror(RGBAColor const & cInColor)
{
	MaterialKitPrivate * pcImpl = (MaterialKitPrivate *)m_pcImpl;
	pcImpl->m_pcColors[(int)Material::Channel::Mirror] = cInColor;
	return *this;
}

MaterialKit & H3DF::MaterialKit::SetMirror(CString strTextureName, RGBAColor const & cInModulatingColor)
{
	MaterialKitPrivate * pcImpl = (MaterialKitPrivate *)m_pcImpl;
	pcImpl->m_strTextureNames[(int)Material::Channel::Mirror] = strTextureName;
	pcImpl->m_pcColors[(int)Material::Channel::Mirror] = cInModulatingColor;
	return *this;
}

MaterialKit & H3DF::MaterialKit::SetTransmission(CString strTextureName, RGBAColor const & cInModulatingColor)
{
	MaterialKitPrivate * pcImpl = (MaterialKitPrivate *)m_pcImpl;
	pcImpl->m_strTextureNames[(int)Material::Channel::Transmission] = strTextureName;
	pcImpl->m_pcColors[(int)Material::Channel::Transmission] = cInModulatingColor;
	return *this;
}

MaterialKit & H3DF::MaterialKit::SetEmission(RGBAColor const & cInColor)
{
	MaterialKitPrivate * pcImpl = (MaterialKitPrivate *)m_pcImpl;
	pcImpl->m_pcColors[(int)Material::Channel::Emission] = cInColor;
	return *this;
}

MaterialKit & H3DF::MaterialKit::SetEmission(CString strTextureName, RGBAColor const & cInModulatingColor)
{
	MaterialKitPrivate * pcImpl = (MaterialKitPrivate *)m_pcImpl;
	pcImpl->m_strTextureNames[(int)Material::Channel::Emission] = strTextureName;
	pcImpl->m_pcColors[(int)Material::Channel::Emission] = cInModulatingColor;
	return *this;
}

MaterialKit & H3DF::MaterialKit::SetEnvironmentTexture(CString strTextureName)
{
	MaterialKitPrivate * pcImpl = (MaterialKitPrivate *)m_pcImpl;
	pcImpl->m_strTextureNames[(int)Material::Channel::EnvironmentTexture] = strTextureName;
	return *this;
}

MaterialKit & H3DF::MaterialKit::SetEnvironmentTexture(CString strTextureName, RGBAColor const & cInModulatingColor)
{
	MaterialKitPrivate * pcImpl = (MaterialKitPrivate *)m_pcImpl;
	pcImpl->m_strTextureNames[(int)Material::Channel::EnvironmentTexture] = strTextureName;
	pcImpl->m_pcColors[(int)Material::Channel::EnvironmentTexture] = cInModulatingColor;
	return *this;
}

MaterialKit & H3DF::MaterialKit::SetEnvironmentCubeMap(CString strTextureName)
{
	MaterialKitPrivate * pcImpl = (MaterialKitPrivate *)m_pcImpl;
	pcImpl->m_strTextureNames[(int)Material::Channel::EnvironmentCubeMap] = strTextureName;
	return *this;
}

MaterialKit & H3DF::MaterialKit::SetEnvironmentCubeMap(CString strTextureName, RGBAColor const & cInModulatingColor)
{
	MaterialKitPrivate * pcImpl = (MaterialKitPrivate *)m_pcImpl;
	pcImpl->m_strTextureNames[(int)Material::Channel::EnvironmentCubeMap] = strTextureName;
	pcImpl->m_pcColors[(int)Material::Channel::EnvironmentCubeMap] = cInModulatingColor;
	return *this;
}

MaterialKit & H3DF::MaterialKit::SetBump(CString strTextureName)
{
	MaterialKitPrivate * pcImpl = (MaterialKitPrivate *)m_pcImpl;
	pcImpl->m_strTextureNames[(int)Material::Channel::Bump] = strTextureName;
	return *this;
}

// Gloss is always positive, and most surfaces have a gloss in the range of 1.0 to 30.0.
MaterialKit & H3DF::MaterialKit::SetGloss(float fInGloss)
{
	MaterialKitPrivate * pcImpl = (MaterialKitPrivate *)m_pcImpl;
	pcImpl->m_fGloss = fInGloss;
	return *this;
}

// Removes all settings applied to the diffuse rgb channel.
MaterialKit & H3DF::MaterialKit::UnsetDiffuseColorRGB()
{
	MaterialKitPrivate * pcImpl = (MaterialKitPrivate *)m_pcImpl;
	pcImpl->m_pcColors[(int)Material::Channel::DiffuseColor].Invalid();
	return *this;
}

// Removes all settings applied to the diffuse color channel including alpha.
MaterialKit & H3DF::MaterialKit::UnsetDiffuseColor()
{
	MaterialKitPrivate * pcImpl = (MaterialKitPrivate *)m_pcImpl;
	pcImpl->m_pcColors[(int)Material::Channel::DiffuseColor].Invalid();
	pcImpl->m_pcColors[(int)Material::Channel::DiffuseTexture].Invalid();
	return *this;
}

// Removes all settings applied to the diffuse alpha channel.
MaterialKit & H3DF::MaterialKit::UnsetDiffuseAlpha()
{
	MaterialKitPrivate * pcImpl = (MaterialKitPrivate *)m_pcImpl;
	pcImpl->m_pcColors[(int)Material::Channel::DiffuseColor].alpha = -1;
	return *this;
}

MaterialKit & H3DF::MaterialKit::UnsetDiffuseTexture()
{
	MaterialKitPrivate * pcImpl = (MaterialKitPrivate *)m_pcImpl;
	pcImpl->m_strTextureNames[(int)Material::Channel::DiffuseTexture].Empty();
	return *this;
}

MaterialKit & H3DF::MaterialKit::UnsetSpecular()
{
	MaterialKitPrivate * pcImpl = (MaterialKitPrivate *)m_pcImpl;
	pcImpl->m_pcColors[(int)Material::Channel::Specular].Invalid();
	pcImpl->m_strTextureNames[(int)Material::Channel::Specular].Empty();
	return *this;
}

MaterialKit & H3DF::MaterialKit::UnsetMirror()
{
	MaterialKitPrivate * pcImpl = (MaterialKitPrivate *)m_pcImpl;
	pcImpl->m_pcColors[(int)Material::Channel::Mirror].Invalid();
	pcImpl->m_strTextureNames[(int)Material::Channel::Mirror].Empty();
	return *this;
}

MaterialKit & H3DF::MaterialKit::UnsetTransmission()
{
	MaterialKitPrivate * pcImpl = (MaterialKitPrivate *)m_pcImpl;
	pcImpl->m_strTextureNames[(int)Material::Channel::Transmission].Empty();
	return *this;
}

MaterialKit & H3DF::MaterialKit::UnsetEmission()
{
	MaterialKitPrivate * pcImpl = (MaterialKitPrivate *)m_pcImpl;
	pcImpl->m_pcColors[(int)Material::Channel::Emission].Invalid();
	pcImpl->m_strTextureNames[(int)Material::Channel::Emission].Empty();
	return *this;
}

MaterialKit & H3DF::MaterialKit::UnsetEnvironment()
{
	MaterialKitPrivate * pcImpl = (MaterialKitPrivate *)m_pcImpl;
	pcImpl->m_strTextureNames[(int)Material::Channel::EnvironmentTexture].Empty();
	pcImpl->m_strTextureNames[(int)Material::Channel::EnvironmentCubeMap].Empty();
	return *this;
}

MaterialKit & H3DF::MaterialKit::UnsetBump()
{
	MaterialKitPrivate * pcImpl = (MaterialKitPrivate *)m_pcImpl;
	pcImpl->m_strTextureNames[(int)Material::Channel::Bump].Empty();
	return *this;
}

MaterialKit & H3DF::MaterialKit::UnsetGloss()
{
	MaterialKitPrivate * pcImpl = (MaterialKitPrivate *)m_pcImpl;
	pcImpl->m_fGloss = -1;
	return *this;
}

MaterialKit & H3DF::MaterialKit::UnsetEverything()
{
	MaterialKitPrivate * pcImpl = (MaterialKitPrivate *)m_pcImpl;
	for (int nChannelIndex = 0; nChannelIndex < (int)Material::Channel::Count; nChannelIndex++) {
		pcImpl->m_pcColors[nChannelIndex].Invalid();
		pcImpl->m_strTextureNames[nChannelIndex].Empty();
		pcImpl->m_strTextureOptions[nChannelIndex].Empty();
	}

	pcImpl->m_fGloss = -1;

	return *this;
}

bool H3DF::MaterialKit::ShowDiffuseColor(RGBColor & cOutColor) const
{
	MaterialKitPrivate * pcImpl = (MaterialKitPrivate *)m_pcImpl;
	RGBAColor cTempOutColor;
	if (false == pcImpl->ShowColor(Material::Channel::DiffuseColor, cTempOutColor)) {
		return false;
	}

	cOutColor.Set(cTempOutColor.red, cTempOutColor.green, cTempOutColor.blue);

	return true;
}

bool H3DF::MaterialKit::ShowDiffuseColor(RGBAColor & cOutColor) const
{
	MaterialKitPrivate * pcImpl = (MaterialKitPrivate *)m_pcImpl;
	return pcImpl->ShowColor(Material::Channel::DiffuseColor, cOutColor);
}

bool H3DF::MaterialKit::ShowDiffuseAlpha(float & fOutAlpha) const
{
	MaterialKitPrivate * pcImpl = (MaterialKitPrivate *)m_pcImpl;
	return pcImpl->ShowAlpah(Material::Channel::DiffuseColor, fOutAlpha);
}

bool H3DF::MaterialKit::ShowDiffuseTexture(Material::Type & cOutType, RGBAColor & cOutColor, CString & strOutTextureName) const
{
	MaterialKitPrivate * pcImpl = (MaterialKitPrivate *)m_pcImpl;
	return pcImpl->ShowTexture(Material::Channel::DiffuseTexture, cOutType, cOutColor, strOutTextureName);
}

bool H3DF::MaterialKit::ShowSpecular(Material::Type & cOutType, RGBAColor & cOutColor, CString & strOutTextureName) const
{
	MaterialKitPrivate * pcImpl = (MaterialKitPrivate *)m_pcImpl;
	return pcImpl->ShowTexture(Material::Channel::Specular, cOutType, cOutColor, strOutTextureName);
}

bool H3DF::MaterialKit::ShowMirror(Material::Type & cOutType, RGBAColor & cOutColor, CString & strOutTextureName) const
{
	MaterialKitPrivate * pcImpl = (MaterialKitPrivate *)m_pcImpl;
	return pcImpl->ShowTexture(Material::Channel::Mirror, cOutType, cOutColor, strOutTextureName);
}

bool H3DF::MaterialKit::ShowTransmission(Material::Type & cOutType, RGBAColor & cOutColor, CString & strOutTextureName) const
{
	MaterialKitPrivate * pcImpl = (MaterialKitPrivate *)m_pcImpl;
	return pcImpl->ShowTexture(Material::Channel::Transmission, cOutType, cOutColor, strOutTextureName);
}

bool H3DF::MaterialKit::ShowEmission(Material::Type & cOutType, RGBAColor & cOutColor, CString & strOutTextureName) const
{
	MaterialKitPrivate * pcImpl = (MaterialKitPrivate *)m_pcImpl;
	return pcImpl->ShowTexture(Material::Channel::Emission, cOutType, cOutColor, strOutTextureName);
}

bool H3DF::MaterialKit::ShowEnvironment(Material::Type & cOutType, RGBAColor & cOutColor, CString & strOutTextureName) const
{
	MaterialKitPrivate * pcImpl = (MaterialKitPrivate *)m_pcImpl;
	return pcImpl->ShowTexture(Material::Channel::EnvironmentTexture, cOutType, cOutColor, strOutTextureName);
}

bool H3DF::MaterialKit::ShowBump(CString & strOutTextureName) const
{
	MaterialKitPrivate * pcImpl = (MaterialKitPrivate *)m_pcImpl;
	if (true == pcImpl->m_strTextureNames[(int)Material::Channel::Bump].IsEmpty()) {
		return false;
	}

	strOutTextureName = pcImpl->m_strTextureNames[(int)Material::Channel::Bump];

	return true;
}

bool H3DF::MaterialKit::ShowGloss(float & fOutGloss) const
{
	MaterialKitPrivate * pcImpl = (MaterialKitPrivate *)m_pcImpl;
	if (0 > pcImpl->m_fGloss) {
		return false;
	}

	fOutGloss = pcImpl->m_fGloss;

	return true;
}

//== MaterialMappingKit ============================================================================

class MaterialMappingKitPrivate : public PrivateImpl
{
public:
	enum class Type : uint32_t {
		Line,
		Edge,
		Marker,
		Text,
		Vertex,
		Face,
		FrontFace,
		BackFace,
		AmbientLightUp,
		AmbientLightDown,
		Count
	};

	MaterialMappingKitPrivate();

	void Copy(MaterialMappingKitPrivate * pcInThat) {
		for (int nTypeIndex = 0; nTypeIndex < (int)Type::Count; nTypeIndex++) {
			m_cMaterialKits[nTypeIndex] = pcInThat->m_cMaterialKits[nTypeIndex];
		}
	}

	void SetColor(RGBAColor const & cInRgbaColor, MaterialMappingKitPrivate::Type eType, Material::Color::Channel eChannel);

	bool ShowChannel(MaterialMappingKitPrivate::Type eInType, Material::Channel eInChannel, Material::Type & cOutType, RGBAColor & cOutColor, CString & strOutTextureName) const;
	bool ShowMaterial(MaterialMappingKitPrivate::Type eInType, MaterialKit & cOutKit) const;

	MaterialKit m_cMaterialKits[(int)Type::Count];
};

MaterialMappingKitPrivate::MaterialMappingKitPrivate()
{
	m_eType = H3DF::Type::MaterialMappingKit;
}

void MaterialMappingKitPrivate::SetColor(RGBAColor const & cInRgbaColor, MaterialMappingKitPrivate::Type eType, Material::Color::Channel eChannel)
{
	switch (eChannel)
	{
		case H3DF::Material::Color::Channel::DiffuseColor:
			m_cMaterialKits[(int)eType].SetDiffuseColor(cInRgbaColor);
			break;

		case H3DF::Material::Color::Channel::Specular:
			m_cMaterialKits[(int)eType].SetSpecular(cInRgbaColor);
			break;

		case H3DF::Material::Color::Channel::Emission:
			m_cMaterialKits[(int)eType].SetDiffuseColor(cInRgbaColor);
			break;

		case H3DF::Material::Color::Channel::Mirror:
			m_cMaterialKits[(int)eType].SetDiffuseColor(cInRgbaColor);
			break;
	}
}

bool MaterialMappingKitPrivate::ShowChannel(MaterialMappingKitPrivate::Type eInType, Material::Channel eInChannel, Material::Type & cOutType, RGBAColor & cOutColor, CString & strOutTextureName) const
{
	MaterialKitPrivate * pcImpl = (MaterialKitPrivate *)m_cMaterialKits[(int)eInType].GetImpl();
	return pcImpl->ShowChannel(eInChannel, cOutType, cOutColor, strOutTextureName);
}

bool MaterialMappingKitPrivate::ShowMaterial(MaterialMappingKitPrivate::Type eInType, MaterialKit & cOutKit) const
{
	if (true == m_cMaterialKits[(int)eInType].Empty()) {
		return false;
	}

	cOutKit = m_cMaterialKits[(int)eInType];
	return true;
}

MaterialMappingKit::MaterialMappingKit()
{
	m_pcImpl = new MaterialMappingKitPrivate();
}

MaterialMappingKit::MaterialMappingKit(MaterialMappingKit const & cInKit)
{
	m_pcImpl = new MaterialMappingKitPrivate();
	Set(cInKit);
}

void MaterialMappingKit::Set(MaterialMappingKit const & cInThat)
{
	MaterialMappingKitPrivate * pcImpl = (MaterialMappingKitPrivate *)m_pcImpl;
	MaterialMappingKitPrivate * pcInThatImpl = (MaterialMappingKitPrivate *)cInThat.m_pcImpl;
	pcImpl->Copy(pcInThatImpl);
}

MaterialMappingKit & MaterialMappingKit::operator = (MaterialMappingKit const & cInThat)
{
	Set(cInThat);
	return *this;
}

//== Color 설정 =====================================================================================

MaterialMappingKit & MaterialMappingKit::SetAmbientLightUpColor(RGBAColor const & cInRgbaColor)
{
	MaterialMappingKitPrivate * pcImpl = (MaterialMappingKitPrivate *)m_pcImpl;
	pcImpl->m_cMaterialKits[(int)MaterialMappingKitPrivate::Type::AmbientLightUp].SetDiffuseColor(cInRgbaColor);
	return *this;
}

MaterialMappingKit & MaterialMappingKit::SetAmbientLightDownColor(RGBAColor const & cInRgbaColor)
{
	MaterialMappingKitPrivate * pcImpl = (MaterialMappingKitPrivate *)m_pcImpl;
	pcImpl->m_cMaterialKits[(int)MaterialMappingKitPrivate::Type::AmbientLightDown].SetDiffuseColor(cInRgbaColor);
	return *this;
}

MaterialMappingKit & MaterialMappingKit::SetBackFaceColor(RGBAColor const & cInRgbaColor, Material::Color::Channel eInChannel)
{
	MaterialMappingKitPrivate * pcImpl = (MaterialMappingKitPrivate *)m_pcImpl;
	pcImpl->SetColor(cInRgbaColor, MaterialMappingKitPrivate::Type::BackFace, eInChannel);
	return *this;
}

MaterialMappingKit & MaterialMappingKit::SetBackFaceAlpha(float fInAlpha)
{
	MaterialMappingKitPrivate * pcImpl = (MaterialMappingKitPrivate *)m_pcImpl;
	pcImpl->m_cMaterialKits[(int)MaterialMappingKitPrivate::Type::BackFace].SetDiffuseAlpha(fInAlpha);
	return *this;
}

MaterialMappingKit & MaterialMappingKit::SetBackFaceTexture(CString strTextureName)
{
	MaterialMappingKitPrivate * pcImpl = (MaterialMappingKitPrivate *)m_pcImpl;
	pcImpl->m_cMaterialKits[(int)MaterialMappingKitPrivate::Type::BackFace].SetDiffuseTexture(strTextureName);
	return *this;
}

MaterialMappingKit & MaterialMappingKit::SetBackFaceTextureOption(CString strTextureOption)
{
	MaterialMappingKitPrivate * pcImpl = (MaterialMappingKitPrivate *)m_pcImpl;
	pcImpl->m_cMaterialKits[(int)MaterialMappingKitPrivate::Type::BackFace].SetDiffuseTextureOption(strTextureOption);
	return *this;
}

MaterialMappingKit & MaterialMappingKit::SetBackFaceGloss(float fInValue)
{
	MaterialMappingKitPrivate * pcImpl = (MaterialMappingKitPrivate *)m_pcImpl;
	pcImpl->m_cMaterialKits[(int)MaterialMappingKitPrivate::Type::BackFace].SetGloss(fInValue);
	return *this;
}

MaterialMappingKit & MaterialMappingKit::SetBackFaceMaterial(MaterialKit const & cInMaterial)
{
	MaterialMappingKitPrivate * pcImpl = (MaterialMappingKitPrivate *)m_pcImpl;
	pcImpl->m_cMaterialKits[(int)MaterialMappingKitPrivate::Type::BackFace] = cInMaterial;
	return *this;
}

MaterialMappingKit & MaterialMappingKit::SetFrontFaceColor(RGBAColor const & cInRgbaColor, Material::Color::Channel eInChannel)
{
	MaterialMappingKitPrivate * pcImpl = (MaterialMappingKitPrivate *)m_pcImpl;
	pcImpl->SetColor(cInRgbaColor, MaterialMappingKitPrivate::Type::FrontFace, eInChannel);
	return *this;
}

MaterialMappingKit & MaterialMappingKit::SetFrontFaceAlpha(float fInAlpha)
{
	MaterialMappingKitPrivate * pcImpl = (MaterialMappingKitPrivate *)m_pcImpl;
	pcImpl->m_cMaterialKits[(int)MaterialMappingKitPrivate::Type::FrontFace].SetDiffuseAlpha(fInAlpha);
	return *this;
}

MaterialMappingKit & MaterialMappingKit::SetFrontFaceTexture(CString strTextureName)
{
	MaterialMappingKitPrivate * pcImpl = (MaterialMappingKitPrivate *)m_pcImpl;
	pcImpl->m_cMaterialKits[(int)MaterialMappingKitPrivate::Type::FrontFace].SetDiffuseTexture(strTextureName);
	return *this;
}

MaterialMappingKit & MaterialMappingKit::SetFrontFaceTextureOption(CString strTextureOption)
{
	MaterialMappingKitPrivate * pcImpl = (MaterialMappingKitPrivate *)m_pcImpl;
	pcImpl->m_cMaterialKits[(int)MaterialMappingKitPrivate::Type::FrontFace].SetDiffuseTextureOption(strTextureOption);
	return *this;
}

MaterialMappingKit & MaterialMappingKit::SetFrontFaceGloss(float fInValue)
{
	MaterialMappingKitPrivate * pcImpl = (MaterialMappingKitPrivate *)m_pcImpl;
	pcImpl->m_cMaterialKits[(int)MaterialMappingKitPrivate::Type::FrontFace].SetGloss(fInValue);
	return *this;
}

MaterialMappingKit & MaterialMappingKit::SetFrontFaceMaterial(MaterialKit const & cInMaterial)
{
	MaterialMappingKitPrivate * pcImpl = (MaterialMappingKitPrivate *)m_pcImpl;
	pcImpl->m_cMaterialKits[(int)MaterialMappingKitPrivate::Type::FrontFace] = cInMaterial;
	return *this;
}

MaterialMappingKit & MaterialMappingKit::SetEdgeAlpha(float fInAlpha)
{
	MaterialMappingKitPrivate * pcImpl = (MaterialMappingKitPrivate *)m_pcImpl;
	pcImpl->m_cMaterialKits[(int)MaterialMappingKitPrivate::Type::Edge].SetDiffuseAlpha(fInAlpha);
	return *this;
}

MaterialMappingKit & MaterialMappingKit::SetEdgeColor(RGBAColor const & cInRgbaColor, Material::Color::Channel eInChannel)
{
	MaterialMappingKitPrivate * pcImpl = (MaterialMappingKitPrivate *)m_pcImpl;
	pcImpl->SetColor(cInRgbaColor, MaterialMappingKitPrivate::Type::Edge, eInChannel);
	return *this;
}

MaterialMappingKit & MaterialMappingKit::SetEdgeTexture(char const * strTextureName)
{
	MaterialMappingKitPrivate * pcImpl = (MaterialMappingKitPrivate *)m_pcImpl;
	pcImpl->m_cMaterialKits[(int)MaterialMappingKitPrivate::Type::Edge].SetDiffuseTexture(strTextureName);
	return *this;
}

MaterialMappingKit & MaterialMappingKit::SetEdgeGloss(float fInValue)
{
	MaterialMappingKitPrivate * pcImpl = (MaterialMappingKitPrivate *)m_pcImpl;
	pcImpl->m_cMaterialKits[(int)MaterialMappingKitPrivate::Type::Edge].SetGloss(fInValue);
	return *this;
}

MaterialMappingKit & MaterialMappingKit::SetEdgeMaterial(MaterialKit const & cInMaterial)
{
	MaterialMappingKitPrivate * pcImpl = (MaterialMappingKitPrivate *)m_pcImpl;
	pcImpl->m_cMaterialKits[(int)MaterialMappingKitPrivate::Type::Edge] = cInMaterial;
	return *this;
}

MaterialMappingKit & MaterialMappingKit::SetFaceColor(RGBAColor const & cInRgbaColor, Material::Color::Channel eInChannel)
{
	MaterialMappingKitPrivate * pcImpl = (MaterialMappingKitPrivate *)m_pcImpl;
	pcImpl->SetColor(cInRgbaColor, MaterialMappingKitPrivate::Type::Face, eInChannel);
	return *this;
}

MaterialMappingKit & MaterialMappingKit::SetFaceAlpha(float fInAlpha)
{
	MaterialMappingKitPrivate * pcImpl = (MaterialMappingKitPrivate *)m_pcImpl;
	pcImpl->m_cMaterialKits[(int)MaterialMappingKitPrivate::Type::Face].SetDiffuseAlpha(fInAlpha);
	return *this;
}

MaterialMappingKit & MaterialMappingKit::SetFaceTexture(CString strTextureName)
{
	MaterialMappingKitPrivate * pcImpl = (MaterialMappingKitPrivate *)m_pcImpl;
	pcImpl->m_cMaterialKits[(int)MaterialMappingKitPrivate::Type::Face].SetDiffuseTexture(strTextureName);
	return *this;
}

MaterialMappingKit & MaterialMappingKit::SetFaceTextureOption(CString strTextureOption)
{
	MaterialMappingKitPrivate * pcImpl = (MaterialMappingKitPrivate *)m_pcImpl;
	pcImpl->m_cMaterialKits[(int)MaterialMappingKitPrivate::Type::Face].SetDiffuseTextureOption(strTextureOption);
	return *this;
}

MaterialMappingKit & MaterialMappingKit::SetFaceGloss(float fInValue)
{
	MaterialMappingKitPrivate * pcImpl = (MaterialMappingKitPrivate *)m_pcImpl;
	pcImpl->m_cMaterialKits[(int)MaterialMappingKitPrivate::Type::Face].SetGloss(fInValue);
	return *this;
}

MaterialMappingKit & MaterialMappingKit::SetFaceMaterial(MaterialKit const & cInMaterial)
{
	MaterialMappingKitPrivate * pcImpl = (MaterialMappingKitPrivate *)m_pcImpl;
	pcImpl->m_cMaterialKits[(int)MaterialMappingKitPrivate::Type::Face] = cInMaterial;
	return *this;
}

MaterialMappingKit & MaterialMappingKit::SetLineAlpha(float fInAlpha)
{
	MaterialMappingKitPrivate * pcImpl = (MaterialMappingKitPrivate *)m_pcImpl;
	pcImpl->m_cMaterialKits[(int)MaterialMappingKitPrivate::Type::Line].SetDiffuseAlpha(fInAlpha);
	return *this;
}

MaterialMappingKit & MaterialMappingKit::SetLineColor(RGBAColor const & cInRgbaColor)
{
	MaterialMappingKitPrivate * pcImpl = (MaterialMappingKitPrivate *)m_pcImpl;
	pcImpl->m_cMaterialKits[(int)MaterialMappingKitPrivate::Type::Line].SetDiffuseColor(cInRgbaColor);
	return *this;
}

MaterialMappingKit & MaterialMappingKit::SetMarkerColor(RGBAColor const & cInRgbaColor)
{
	MaterialMappingKitPrivate * pcImpl = (MaterialMappingKitPrivate *)m_pcImpl;
	pcImpl->m_cMaterialKits[(int)MaterialMappingKitPrivate::Type::Marker].SetDiffuseColor(cInRgbaColor);
	return *this;
}

MaterialMappingKit & MaterialMappingKit::SetTextColor(RGBAColor const & cInRgbaColor)
{
	MaterialMappingKitPrivate * pcImpl = (MaterialMappingKitPrivate *)m_pcImpl;
	pcImpl->m_cMaterialKits[(int)MaterialMappingKitPrivate::Type::Text].SetDiffuseColor(cInRgbaColor);
	return *this;
}

MaterialMappingKit & MaterialMappingKit::SetVertexAlpha(float fInAlpha)
{
	MaterialMappingKitPrivate * pcImpl = (MaterialMappingKitPrivate *)m_pcImpl;
	pcImpl->m_cMaterialKits[(int)MaterialMappingKitPrivate::Type::Vertex].SetDiffuseAlpha(fInAlpha);
	return *this;
}

MaterialMappingKit & MaterialMappingKit::SetVertexColor(RGBAColor const & cInRgbaColor, Material::Color::Channel eInChannel)
{
	MaterialMappingKitPrivate * pcImpl = (MaterialMappingKitPrivate *)m_pcImpl;
	pcImpl->SetColor(cInRgbaColor, MaterialMappingKitPrivate::Type::Vertex, eInChannel);
	return *this;
}

MaterialMappingKit & MaterialMappingKit::SetVertexTexture(CString strTextureName)
{
	MaterialMappingKitPrivate * pcImpl = (MaterialMappingKitPrivate *)m_pcImpl;
	pcImpl->m_cMaterialKits[(int)MaterialMappingKitPrivate::Type::Vertex].SetDiffuseTexture(strTextureName);
	return *this;
}

MaterialMappingKit & MaterialMappingKit::SetVertexGloss(float fInValue)
{
	MaterialMappingKitPrivate * pcImpl = (MaterialMappingKitPrivate *)m_pcImpl;
	pcImpl->m_cMaterialKits[(int)MaterialMappingKitPrivate::Type::Vertex].SetGloss(fInValue);
	return *this;
}

MaterialMappingKit & MaterialMappingKit::SetVertexMaterial(MaterialKit const & cInMaterial)
{
	MaterialMappingKitPrivate * pcImpl = (MaterialMappingKitPrivate *)m_pcImpl;
	pcImpl->m_cMaterialKits[(int)MaterialMappingKitPrivate::Type::Vertex] = cInMaterial;
	return *this;
}

MaterialMappingKit & MaterialMappingKit::SetGeometryColor(RGBAColor const & cInRgbaColor)
{
	SetEdgeColor(cInRgbaColor);
	SetFaceColor(cInRgbaColor);
	SetLineColor(cInRgbaColor);
	SetMarkerColor(cInRgbaColor);
	SetTextColor(cInRgbaColor);
	SetVertexColor(cInRgbaColor);

	return *this;
}

bool MaterialMappingKit::operator == (MaterialMappingKit const & cInThat) const
{
	MaterialMappingKitPrivate * pcImpl = (MaterialMappingKitPrivate *)m_pcImpl;
	MaterialMappingKitPrivate * pcInThatImpl = (MaterialMappingKitPrivate *)cInThat.m_pcImpl;

	for (int nTypeIndex = 0; nTypeIndex < (int)MaterialMappingKitPrivate::Type::Count; nTypeIndex++) {
		if (pcImpl->m_cMaterialKits[nTypeIndex] != pcInThatImpl->m_cMaterialKits[nTypeIndex]) {
			return false;
		}
	}

	return true;
}

bool MaterialMappingKit::operator != (MaterialMappingKit const & cInThat) const
{
	if (*this == cInThat) {
		return false;
	}

	return true;
}

bool MaterialMappingKit::ShowAmbientLightUpColor(Material::Type & cOutType, RGBAColor & cOutColor) const
{
	MaterialMappingKitPrivate * pcImpl = (MaterialMappingKitPrivate *)m_pcImpl;
	return pcImpl->m_cMaterialKits[(int)MaterialMappingKitPrivate::Type::AmbientLightUp].ShowDiffuseColor(cOutColor);

}

bool MaterialMappingKit::ShowAmbientLightDownColor(Material::Type & cOutType, RGBAColor & cOutColor) const
{
	MaterialMappingKitPrivate * pcImpl = (MaterialMappingKitPrivate *)m_pcImpl;
	return pcImpl->m_cMaterialKits[(int)MaterialMappingKitPrivate::Type::AmbientLightDown].ShowDiffuseColor(cOutColor);
}

bool MaterialMappingKit::ShowBackFaceChannel(Material::Channel eInChannel, Material::Type & cOutType, RGBAColor & cOutColor, CString & strOutTextureName) const
{
	MaterialMappingKitPrivate * pcImpl = (MaterialMappingKitPrivate *)m_pcImpl;
	return pcImpl->ShowChannel(MaterialMappingKitPrivate::Type::BackFace, eInChannel, cOutType, cOutColor, strOutTextureName);
}

bool MaterialMappingKit::ShowBackFaceMaterial(MaterialKit & cOutKit) const
{
	MaterialMappingKitPrivate * pcImpl = (MaterialMappingKitPrivate *)m_pcImpl;
	return pcImpl->ShowMaterial(MaterialMappingKitPrivate::Type::BackFace, cOutKit);
}

bool MaterialMappingKit::ShowFrontFaceChannel(Material::Channel eInChannel, Material::Type & cOutType, RGBAColor & cOutColor, CString & strOutTextureName) const
{
	MaterialMappingKitPrivate * pcImpl = (MaterialMappingKitPrivate *)m_pcImpl;
	return pcImpl->ShowChannel(MaterialMappingKitPrivate::Type::FrontFace, eInChannel, cOutType, cOutColor, strOutTextureName);
}

bool MaterialMappingKit::ShowFrontFaceMaterial(MaterialKit & cOutKit) const
{
	MaterialMappingKitPrivate * pcImpl = (MaterialMappingKitPrivate *)m_pcImpl;
	return pcImpl->ShowMaterial(MaterialMappingKitPrivate::Type::FrontFace, cOutKit);
}

bool MaterialMappingKit::ShowEdgeChannel(Material::Channel eInChannel, Material::Type & cOutType, RGBAColor & cOutColor, CString & strOutTextureName) const
{
	MaterialMappingKitPrivate * pcImpl = (MaterialMappingKitPrivate *)m_pcImpl;
	return pcImpl->ShowChannel(MaterialMappingKitPrivate::Type::Edge, eInChannel, cOutType, cOutColor, strOutTextureName);
}

bool MaterialMappingKit::ShowEdgeMaterial(MaterialKit & cOutKit) const
{
	MaterialMappingKitPrivate * pcImpl = (MaterialMappingKitPrivate *)m_pcImpl;
	return pcImpl->ShowMaterial(MaterialMappingKitPrivate::Type::Edge, cOutKit);
}

bool MaterialMappingKit::ShowFaceChannel(Material::Channel eInChannel, Material::Type & cOutType, RGBAColor & cOutColor, CString & strOutTextureName) const
{
	MaterialMappingKitPrivate * pcImpl = (MaterialMappingKitPrivate *)m_pcImpl;
	return pcImpl->ShowChannel(MaterialMappingKitPrivate::Type::Face, eInChannel, cOutType, cOutColor, strOutTextureName);
}

bool MaterialMappingKit::ShowFaceMaterial(MaterialKit & cOutKit) const
{
	MaterialMappingKitPrivate * pcImpl = (MaterialMappingKitPrivate *)m_pcImpl;
	return pcImpl->ShowMaterial(MaterialMappingKitPrivate::Type::Face, cOutKit);
}

bool MaterialMappingKit::ShowLineAlpha(float & fOutAlpha) const
{
	MaterialMappingKitPrivate * pcImpl = (MaterialMappingKitPrivate *)m_pcImpl;
	return pcImpl->m_cMaterialKits[(int)MaterialMappingKitPrivate::Type::Line].ShowDiffuseAlpha(fOutAlpha);
}

bool MaterialMappingKit::ShowLineColor(RGBAColor & cOutColor) const
{
	MaterialMappingKitPrivate * pcImpl = (MaterialMappingKitPrivate *)m_pcImpl;
	return pcImpl->m_cMaterialKits[(int)MaterialMappingKitPrivate::Type::Line].ShowDiffuseColor(cOutColor);
}

bool MaterialMappingKit::ShowMarkerColor(RGBAColor & cOutColor) const
{
	MaterialMappingKitPrivate * pcImpl = (MaterialMappingKitPrivate *)m_pcImpl;
	return pcImpl->m_cMaterialKits[(int)MaterialMappingKitPrivate::Type::Marker].ShowDiffuseColor(cOutColor);
}

bool MaterialMappingKit::ShowTextColor(RGBAColor & cOutColor) const
{
	MaterialMappingKitPrivate * pcImpl = (MaterialMappingKitPrivate *)m_pcImpl;
	return pcImpl->m_cMaterialKits[(int)MaterialMappingKitPrivate::Type::Text].ShowDiffuseColor(cOutColor);
}

bool MaterialMappingKit::ShowVertexChannel(Material::Channel eInChannel, Material::Type & cOutType, RGBAColor & cOutColor, CString & strOutTextureName) const
{
	MaterialMappingKitPrivate * pcImpl = (MaterialMappingKitPrivate *)m_pcImpl;
	return pcImpl->ShowChannel(MaterialMappingKitPrivate::Type::Vertex, eInChannel, cOutType, cOutColor, strOutTextureName);
}

bool MaterialMappingKit::ShowVertexMaterial(MaterialKit & cOutKit) const
{
	MaterialMappingKitPrivate * pcImpl = (MaterialMappingKitPrivate *)m_pcImpl;
	return pcImpl->ShowMaterial(MaterialMappingKitPrivate::Type::Vertex, cOutKit);
}

//== MaterialMappingControl ========================================================================

MaterialMappingControl::MaterialMappingControl(SegmentKey & cInSegmentKey) :
	m_cInSegmentKey(cInSegmentKey)
{

}

//== Color 설정 =====================================================================================

MaterialMappingControl & MaterialMappingControl::SetFaceColor(RGBAColor const & cInRgbaColor, Material::Color::Channel cInChannel)
{
	return SetColor(L"faces", "diffuse", cInRgbaColor);
}

MaterialMappingControl & MaterialMappingControl::SetMarkerColor(RGBAColor const & cInRgbaColor)
{
	return SetColor(L"markers", "diffuse", cInRgbaColor);
}

MaterialMappingControl & MaterialMappingControl::SetColor(CString strGeometry, CString strChannel, RGBAColor const & cInRgbaColor)
{
	SegmentKeyPrivate::LocalOpen(m_cInSegmentKey);

	CString strColorText;
	strColorText.Format(L"%s = (%s = (r=%f g=%f b=%f))", strGeometry, strChannel, cInRgbaColor.red, cInRgbaColor.green, cInRgbaColor.blue);
	HC_Set_Color(Utility::ToChar(strColorText));

	SegmentKeyPrivate::LocalClose(m_cInSegmentKey);

	return *this;
}

MaterialMappingControl & MaterialMappingControl::UnSetColor(CString strInType)
{
	SegmentKeyPrivate::LocalOpen(m_cInSegmentKey);

	HC_UnSet_One_Selectability(Utility::ToChar(strInType));

	SegmentKeyPrivate::LocalClose(m_cInSegmentKey);

	return *this;
}

void MaterialMappingControl::InitPopulateTextures()
{
	InitializeMagick(".");

	SegmentKeyPrivate::LocalOpen(m_cInSegmentKey);

	HC_Set_Visibility("image = off");

	SegmentKeyPrivate::LocalClose(m_cInSegmentKey);
}

void MaterialMappingControl::InsertPicture(UINT nIndex, UINT nPixelWidth, UINT nPixelHeight, UCHAR * pucBinaryData)
{
	SegmentKeyPrivate::LocalOpen(m_cInSegmentKey);

	//HC_KEY nKey = HC_Insert_Image(0.0, 0.0, 0.0, H_FORMAT_TEXT("rgba, name = image %u, local = on", nIndex), nPixelWidth, nPixelHeight, pucBinaryData);
	HC_KEY nKey = HC_Insert_Image(0.0, 0.0, 0.0, H_FORMAT_TEXT("rgba, name = image %u", nIndex), nPixelWidth, nPixelHeight, pucBinaryData);

	SegmentKeyPrivate::LocalClose(m_cInSegmentKey);
}

void MaterialMappingControl::InsertDifaultPicture(UINT nIndex, UINT nSize, UCHAR * pucBinaryData)
{
	VArray<unsigned char> anPixels;

	ExceptionInfo exception;
	GetExceptionInfo(&exception);
	ImageInfo * image_info = CloneImageInfo((ImageInfo *) nullptr);
	Image * image = BlobToImage(image_info, (void *)pucBinaryData, nSize, &exception);
	if (image != nullptr)
	{
		unsigned long width = image->magick_columns;
		unsigned long height = image->magick_rows;
		anPixels.EnsureSize(width * height * 4);

		ExportImagePixels(image, 0, 0, width, height, "RGBA", CharPixel, &anPixels[0], &exception);
		DestroyImage(image);
		DestroyImageInfo(image_info);
		DestroyExceptionInfo(&exception);

		SegmentKeyPrivate::LocalOpen(m_cInSegmentKey);

		//HC_KEY nKey = HC_Insert_Image(0.0, 0.0, 0.0, H_FORMAT_TEXT("rgba, name = image %u, local = on", nIndex), width, height, &anPixels[0]);
		HC_KEY nKey = HC_Insert_Image(0.0, 0.0, 0.0, H_FORMAT_TEXT("rgba, name = image %u", nIndex), width, height, &anPixels[0]);

		SegmentKeyPrivate::LocalClose(m_cInSegmentKey);
	}
}

void MaterialMappingControl::SetTextureMatrix(float * pfTextureMatrix, char * pchTextureTransformSegment)
{
	SegmentKeyPrivate::LocalOpen(m_cInSegmentKey);

	HC_Compute_Matrix_Inverse(pfTextureMatrix, pfTextureMatrix);

	char textureTransformSegment[4096] = "";
	HC_Open_Segment("texture_transformations");
	{
		HC_Open_Segment("");
		{
			HC_Show_Segment(HC_Create_Segment("."), pchTextureTransformSegment);
			HC_Set_Texture_Matrix(pfTextureMatrix);
		}
		HC_Close_Segment();
	}
	HC_Close_Segment();

	SegmentKeyPrivate::LocalClose(m_cInSegmentKey);
}

void MaterialMappingControl::SetDefineLocalTexture(UINT nIndex, CString strTextureOptions)
{
	CString strText;
	strText.Format(L"texture_%u", nIndex);

	SegmentKeyPrivate::LocalOpen(m_cInSegmentKey);

	HC_Define_Local_Texture(Utility::ToChar(strText), Utility::ToChar(strTextureOptions));

	SegmentKeyPrivate::LocalClose(m_cInSegmentKey);
}

void MaterialMappingControl::EndPopulateTextures()
{
	DestroyMagick();
}
