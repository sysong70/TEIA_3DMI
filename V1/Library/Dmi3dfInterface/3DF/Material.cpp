#include "StdAfx.h"

#include "Material.h"

#include "Segment.h"
#include "./Impl/SegmentImpl.h"

#include "./Impl/ControlImpl.h"

#include "3DF.Utility.h"

#include <Common_Define.h>

#include <hc.h>
#include <HUtility.h>
#include <HTools.h>
#include <magick/api.h>
#include <varray.h>

using namespace H3DF;

//== MaterialKit ===================================================================================
class MaterialKitImpl : public Impl
{
public:
	MaterialKitImpl() { m_eType = H3DF::Type::MaterialKit; }

	void Copy(MaterialKitImpl * pcInThat) {
		for (int nColorIndex = 0; nColorIndex < (int)Material::Channel::Count; nColorIndex++) {
			m_pcColors[nColorIndex] = pcInThat->m_pcColors[nColorIndex];

			m_bTextureMirrors[nColorIndex] = pcInThat->m_bTextureMirrors[nColorIndex];
			m_strTextureNames[nColorIndex] = pcInThat->m_strTextureNames[nColorIndex];
			m_strTextureOptions[nColorIndex] = pcInThat->m_strTextureOptions[nColorIndex];
		}

		m_fGloss = pcInThat->m_fGloss;
	}

	bool operator == (MaterialKitImpl const & cInThat) const;

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

bool MaterialKitImpl::operator == (MaterialKitImpl const & cInThat) const
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

bool MaterialKitImpl::Empty() const
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

bool MaterialKitImpl::ShowColor(Material::Channel eInChannel, RGBAColor & cOutColor) const
{
	if (false == m_pcColors[(int)eInChannel].IsValid()) {
		return false;
	}

	cOutColor = m_pcColors[(int)eInChannel];

	return true;
}

bool MaterialKitImpl::ShowAlpah(Material::Channel eInChannel, float & fOutAlpah) const
{
	if (0 > m_pcColors[(int)eInChannel].alpha) {
		return false;
	}
	fOutAlpah = m_pcColors[(int)eInChannel].alpha;

	return true;
}

bool MaterialKitImpl::ShowTexture(Material::Channel eInChannel, Material::Type & cOutType, RGBAColor & cOutColor, CString & strOutTextureName) const
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

bool MaterialKitImpl::ShowChannel(Material::Channel eInChannel, Material::Type & cOutType, RGBAColor & cOutColor, CString & strOutTextureName) const
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

H3DF::MaterialKit::MaterialKit()
{
	m_pcImpl = new MaterialKitImpl();
}

H3DF::MaterialKit::MaterialKit(MaterialKit const & cInKit)
{
	m_pcImpl = new MaterialKitImpl();
	Set(cInKit);
}

void H3DF::MaterialKit::Set(MaterialKit const & cInThat)
{
	MaterialKitImpl * pcImpl = (MaterialKitImpl *)m_pcImpl;
	MaterialKitImpl * pcInThatImpl = (MaterialKitImpl *)cInThat.m_pcImpl;
	pcImpl->Copy(pcInThatImpl);
}

MaterialKit & H3DF::MaterialKit::operator = (MaterialKit const & cInThat)
{
	Set(cInThat);
	return *this;
}

bool H3DF::MaterialKit::operator == (MaterialKit const & cInThat) const
{
	MaterialKitImpl * pcImpl = (MaterialKitImpl *)m_pcImpl;
	MaterialKitImpl * pcInThatImpl = (MaterialKitImpl *)cInThat.m_pcImpl;

	return (pcImpl == pcInThatImpl);
}

bool H3DF::MaterialKit::operator != (MaterialKit const & cInThat) const
{
	return !(*this == cInThat);
}

void H3DF::MaterialKit::Show(MaterialKit & cOutKit) const
{
	cOutKit.Set(*this);
}

bool H3DF::MaterialKit::Empty() const
{
	MaterialKitImpl * pcImpl = (MaterialKitImpl *)m_pcImpl;
	return pcImpl->Empty();
}

MaterialKit & H3DF::MaterialKit::SetDiffuse(RGBColor const & cInColor)
{
	MaterialKitImpl * pcImpl = (MaterialKitImpl *)m_pcImpl;
	pcImpl->m_pcColors[(int)Material::Channel::DiffuseColor] = cInColor;
	pcImpl->m_pcColors[(int)Material::Channel::DiffuseTexture] = cInColor;
	return *this;
}

MaterialKit & H3DF::MaterialKit::SetDiffuse(RGBAColor const & cInColor)
{
	MaterialKitImpl * pcImpl = (MaterialKitImpl *)m_pcImpl;
	pcImpl->m_pcColors[(int)Material::Channel::DiffuseColor] = cInColor;
	pcImpl->m_pcColors[(int)Material::Channel::DiffuseTexture] = cInColor;
	return *this;
}

MaterialKit & H3DF::MaterialKit::SetDiffuseColor(RGBColor const & cInColor)
{
	MaterialKitImpl * pcImpl = (MaterialKitImpl *)m_pcImpl;
	pcImpl->m_pcColors[(int)Material::Channel::DiffuseColor] = cInColor;
	return *this;
}

MaterialKit & H3DF::MaterialKit::SetDiffuseColor(RGBAColor const & cInColor)
{
	MaterialKitImpl * pcImpl = (MaterialKitImpl *)m_pcImpl;
	pcImpl->m_pcColors[(int)Material::Channel::DiffuseColor] = cInColor;
	return *this;
}

MaterialKit & H3DF::MaterialKit::SetDiffuseAlpha(float fInAlpha)
{
	MaterialKitImpl * pcImpl = (MaterialKitImpl *)m_pcImpl;
	pcImpl->m_pcColors[(int)Material::Channel::DiffuseColor].alpha = fInAlpha;
	return *this;
}

MaterialKit & H3DF::MaterialKit::SetDiffuseTexture(CString strTextureName)
{
	MaterialKitImpl * pcImpl = (MaterialKitImpl *)m_pcImpl;
	pcImpl->m_strTextureNames[(int)Material::Channel::DiffuseTexture] = strTextureName;
	return *this;
}

MaterialKit & H3DF::MaterialKit::SetDiffuseTexture(CString strTextureName, RGBAColor const & cInModulatingColor)
{
	MaterialKitImpl * pcImpl = (MaterialKitImpl *)m_pcImpl;
	pcImpl->m_strTextureNames[(int)Material::Channel::DiffuseTexture] = strTextureName;
	pcImpl->m_pcColors[(int)Material::Channel::DiffuseTexture] = cInModulatingColor;
	return *this;
}

MaterialKit & H3DF::MaterialKit::SetDiffuseTextureOption(CString strTextureOption)
{
	MaterialKitImpl * pcImpl = (MaterialKitImpl *)m_pcImpl;
	pcImpl->m_strTextureOptions[(int)Material::Channel::DiffuseTexture] = strTextureOption;
	return *this;
}

MaterialKit & H3DF::MaterialKit::SetSpecular(RGBAColor const & cInColor)
{
	MaterialKitImpl * pcImpl = (MaterialKitImpl *)m_pcImpl;
	pcImpl->m_pcColors[(int)Material::Channel::Specular] = cInColor;
	return *this;
}

MaterialKit & H3DF::MaterialKit::SetSpecular(CString strTextureName, RGBAColor const & cInModulatingColor)
{
	MaterialKitImpl * pcImpl = (MaterialKitImpl *)m_pcImpl;
	pcImpl->m_strTextureNames[(int)Material::Channel::Specular] = strTextureName;
	pcImpl->m_pcColors[(int)Material::Channel::Specular] = cInModulatingColor;
	return *this;
}

MaterialKit & H3DF::MaterialKit::SetMirror(RGBAColor const & cInColor)
{
	MaterialKitImpl * pcImpl = (MaterialKitImpl *)m_pcImpl;
	pcImpl->m_pcColors[(int)Material::Channel::Mirror] = cInColor;
	return *this;
}

MaterialKit & H3DF::MaterialKit::SetMirror(CString strTextureName, RGBAColor const & cInModulatingColor)
{
	MaterialKitImpl * pcImpl = (MaterialKitImpl *)m_pcImpl;
	pcImpl->m_strTextureNames[(int)Material::Channel::Mirror] = strTextureName;
	pcImpl->m_pcColors[(int)Material::Channel::Mirror] = cInModulatingColor;
	return *this;
}

MaterialKit & H3DF::MaterialKit::SetTransmission(CString strTextureName, RGBAColor const & cInModulatingColor)
{
	MaterialKitImpl * pcImpl = (MaterialKitImpl *)m_pcImpl;
	pcImpl->m_strTextureNames[(int)Material::Channel::Transmission] = strTextureName;
	pcImpl->m_pcColors[(int)Material::Channel::Transmission] = cInModulatingColor;
	return *this;
}

MaterialKit & H3DF::MaterialKit::SetEmission(RGBAColor const & cInColor)
{
	MaterialKitImpl * pcImpl = (MaterialKitImpl *)m_pcImpl;
	pcImpl->m_pcColors[(int)Material::Channel::Emission] = cInColor;
	return *this;
}

MaterialKit & H3DF::MaterialKit::SetEmission(CString strTextureName, RGBAColor const & cInModulatingColor)
{
	MaterialKitImpl * pcImpl = (MaterialKitImpl *)m_pcImpl;
	pcImpl->m_strTextureNames[(int)Material::Channel::Emission] = strTextureName;
	pcImpl->m_pcColors[(int)Material::Channel::Emission] = cInModulatingColor;
	return *this;
}

MaterialKit & H3DF::MaterialKit::SetEnvironmentTexture(CString strTextureName)
{
	MaterialKitImpl * pcImpl = (MaterialKitImpl *)m_pcImpl;
	pcImpl->m_strTextureNames[(int)Material::Channel::EnvironmentTexture] = strTextureName;
	return *this;
}

MaterialKit & H3DF::MaterialKit::SetEnvironmentTexture(CString strTextureName, RGBAColor const & cInModulatingColor)
{
	MaterialKitImpl * pcImpl = (MaterialKitImpl *)m_pcImpl;
	pcImpl->m_strTextureNames[(int)Material::Channel::EnvironmentTexture] = strTextureName;
	pcImpl->m_pcColors[(int)Material::Channel::EnvironmentTexture] = cInModulatingColor;
	return *this;
}

MaterialKit & H3DF::MaterialKit::SetEnvironmentCubeMap(CString strTextureName)
{
	MaterialKitImpl * pcImpl = (MaterialKitImpl *)m_pcImpl;
	pcImpl->m_strTextureNames[(int)Material::Channel::EnvironmentCubeMap] = strTextureName;
	return *this;
}

MaterialKit & H3DF::MaterialKit::SetEnvironmentCubeMap(CString strTextureName, RGBAColor const & cInModulatingColor)
{
	MaterialKitImpl * pcImpl = (MaterialKitImpl *)m_pcImpl;
	pcImpl->m_strTextureNames[(int)Material::Channel::EnvironmentCubeMap] = strTextureName;
	pcImpl->m_pcColors[(int)Material::Channel::EnvironmentCubeMap] = cInModulatingColor;
	return *this;
}

MaterialKit & H3DF::MaterialKit::SetBump(CString strTextureName)
{
	MaterialKitImpl * pcImpl = (MaterialKitImpl *)m_pcImpl;
	pcImpl->m_strTextureNames[(int)Material::Channel::Bump] = strTextureName;
	return *this;
}

// Gloss is always positive, and most surfaces have a gloss in the range of 1.0 to 30.0.
MaterialKit & H3DF::MaterialKit::SetGloss(float fInGloss)
{
	MaterialKitImpl * pcImpl = (MaterialKitImpl *)m_pcImpl;
	pcImpl->m_fGloss = fInGloss;
	return *this;
}

// Removes all settings applied to the diffuse rgb channel.
MaterialKit & H3DF::MaterialKit::UnsetDiffuseColorRGB()
{
	MaterialKitImpl * pcImpl = (MaterialKitImpl *)m_pcImpl;
	pcImpl->m_pcColors[(int)Material::Channel::DiffuseColor].Invalid();
	return *this;
}

// Removes all settings applied to the diffuse color channel including alpha.
MaterialKit & H3DF::MaterialKit::UnsetDiffuseColor()
{
	MaterialKitImpl * pcImpl = (MaterialKitImpl *)m_pcImpl;
	pcImpl->m_pcColors[(int)Material::Channel::DiffuseColor].Invalid();
	pcImpl->m_pcColors[(int)Material::Channel::DiffuseTexture].Invalid();
	return *this;
}

// Removes all settings applied to the diffuse alpha channel.
MaterialKit & H3DF::MaterialKit::UnsetDiffuseAlpha()
{
	MaterialKitImpl * pcImpl = (MaterialKitImpl *)m_pcImpl;
	pcImpl->m_pcColors[(int)Material::Channel::DiffuseColor].alpha = -1;
	return *this;
}

MaterialKit & H3DF::MaterialKit::UnsetDiffuseTexture()
{
	MaterialKitImpl * pcImpl = (MaterialKitImpl *)m_pcImpl;
	pcImpl->m_strTextureNames[(int)Material::Channel::DiffuseTexture].Empty();
	return *this;
}

MaterialKit & H3DF::MaterialKit::UnsetSpecular()
{
	MaterialKitImpl * pcImpl = (MaterialKitImpl *)m_pcImpl;
	pcImpl->m_pcColors[(int)Material::Channel::Specular].Invalid();
	pcImpl->m_strTextureNames[(int)Material::Channel::Specular].Empty();
	return *this;
}

MaterialKit & H3DF::MaterialKit::UnsetMirror()
{
	MaterialKitImpl * pcImpl = (MaterialKitImpl *)m_pcImpl;
	pcImpl->m_pcColors[(int)Material::Channel::Mirror].Invalid();
	pcImpl->m_strTextureNames[(int)Material::Channel::Mirror].Empty();
	return *this;
}

MaterialKit & H3DF::MaterialKit::UnsetTransmission()
{
	MaterialKitImpl * pcImpl = (MaterialKitImpl *)m_pcImpl;
	pcImpl->m_strTextureNames[(int)Material::Channel::Transmission].Empty();
	return *this;
}

MaterialKit & H3DF::MaterialKit::UnsetEmission()
{
	MaterialKitImpl * pcImpl = (MaterialKitImpl *)m_pcImpl;
	pcImpl->m_pcColors[(int)Material::Channel::Emission].Invalid();
	pcImpl->m_strTextureNames[(int)Material::Channel::Emission].Empty();
	return *this;
}

MaterialKit & H3DF::MaterialKit::UnsetEnvironment()
{
	MaterialKitImpl * pcImpl = (MaterialKitImpl *)m_pcImpl;
	pcImpl->m_strTextureNames[(int)Material::Channel::EnvironmentTexture].Empty();
	pcImpl->m_strTextureNames[(int)Material::Channel::EnvironmentCubeMap].Empty();
	return *this;
}

MaterialKit & H3DF::MaterialKit::UnsetBump()
{
	MaterialKitImpl * pcImpl = (MaterialKitImpl *)m_pcImpl;
	pcImpl->m_strTextureNames[(int)Material::Channel::Bump].Empty();
	return *this;
}

MaterialKit & H3DF::MaterialKit::UnsetGloss()
{
	MaterialKitImpl * pcImpl = (MaterialKitImpl *)m_pcImpl;
	pcImpl->m_fGloss = -1;
	return *this;
}

MaterialKit & H3DF::MaterialKit::UnsetEverything()
{
	MaterialKitImpl * pcImpl = (MaterialKitImpl *)m_pcImpl;
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
	MaterialKitImpl * pcImpl = (MaterialKitImpl *)m_pcImpl;
	RGBAColor cTempOutColor;
	if (false == pcImpl->ShowColor(Material::Channel::DiffuseColor, cTempOutColor)) {
		return false;
	}

	cOutColor.Set(cTempOutColor.red, cTempOutColor.green, cTempOutColor.blue);

	return true;
}

bool H3DF::MaterialKit::ShowDiffuseColor(RGBAColor & cOutColor) const
{
	MaterialKitImpl * pcImpl = (MaterialKitImpl *)m_pcImpl;
	DEBUG_VALID(pcImpl);
	return pcImpl->ShowColor(Material::Channel::DiffuseColor, cOutColor);
}

bool H3DF::MaterialKit::ShowDiffuseAlpha(float & fOutAlpha) const
{
	MaterialKitImpl * pcImpl = (MaterialKitImpl *)m_pcImpl;
	return pcImpl->ShowAlpah(Material::Channel::DiffuseColor, fOutAlpha);
}

bool H3DF::MaterialKit::ShowDiffuseTexture(Material::Type & cOutType, RGBAColor & cOutColor, CString & strOutTextureName) const
{
	MaterialKitImpl * pcImpl = (MaterialKitImpl *)m_pcImpl;
	return pcImpl->ShowTexture(Material::Channel::DiffuseTexture, cOutType, cOutColor, strOutTextureName);
}

bool H3DF::MaterialKit::ShowSpecular(Material::Type & cOutType, RGBAColor & cOutColor, CString & strOutTextureName) const
{
	MaterialKitImpl * pcImpl = (MaterialKitImpl *)m_pcImpl;
	return pcImpl->ShowTexture(Material::Channel::Specular, cOutType, cOutColor, strOutTextureName);
}

bool H3DF::MaterialKit::ShowMirror(Material::Type & cOutType, RGBAColor & cOutColor, CString & strOutTextureName) const
{
	MaterialKitImpl * pcImpl = (MaterialKitImpl *)m_pcImpl;
	return pcImpl->ShowTexture(Material::Channel::Mirror, cOutType, cOutColor, strOutTextureName);
}

bool H3DF::MaterialKit::ShowTransmission(Material::Type & cOutType, RGBAColor & cOutColor, CString & strOutTextureName) const
{
	MaterialKitImpl * pcImpl = (MaterialKitImpl *)m_pcImpl;
	return pcImpl->ShowTexture(Material::Channel::Transmission, cOutType, cOutColor, strOutTextureName);
}

bool H3DF::MaterialKit::ShowEmission(Material::Type & cOutType, RGBAColor & cOutColor, CString & strOutTextureName) const
{
	MaterialKitImpl * pcImpl = (MaterialKitImpl *)m_pcImpl;
	return pcImpl->ShowTexture(Material::Channel::Emission, cOutType, cOutColor, strOutTextureName);
}

bool H3DF::MaterialKit::ShowEnvironment(Material::Type & cOutType, RGBAColor & cOutColor, CString & strOutTextureName) const
{
	MaterialKitImpl * pcImpl = (MaterialKitImpl *)m_pcImpl;
	return pcImpl->ShowTexture(Material::Channel::EnvironmentTexture, cOutType, cOutColor, strOutTextureName);
}

bool H3DF::MaterialKit::ShowBump(CString & strOutTextureName) const
{
	MaterialKitImpl * pcImpl = (MaterialKitImpl *)m_pcImpl;
	if (true == pcImpl->m_strTextureNames[(int)Material::Channel::Bump].IsEmpty()) {
		return false;
	}

	strOutTextureName = pcImpl->m_strTextureNames[(int)Material::Channel::Bump];

	return true;
}

bool H3DF::MaterialKit::ShowGloss(float & fOutGloss) const
{
	MaterialKitImpl * pcImpl = (MaterialKitImpl *)m_pcImpl;
	if (0 > pcImpl->m_fGloss) {
		return false;
	}

	fOutGloss = pcImpl->m_fGloss;

	return true;
}

// Region등에서 사용하기 위해서 추가된 함수
// 내부에서 단순히 SetColor만 호출한다.
void H3DF::MaterialKit::SetMaterial(CStringA strInGeometryName)
{
	MaterialKitImpl * pcImpl = (MaterialKitImpl *)m_pcImpl;
	pcImpl->m_strTextureNames[(int)Material::Channel::DiffuseTexture] = strInGeometryName;

	CStringA strColorText;
	RGBAColor cColor;

	if (true == ShowDiffuseColor(cColor)) {
		if (1.0f == cColor.alpha) {
			strColorText.Format("%s = (diffuse = (r=%f g=%f b=%f))", strInGeometryName, cColor.red, cColor.green, cColor.blue);
		}
		else {
			float fAlpha = 1.0f - cColor.alpha;
			strColorText.Format("%s = (diffuse = (r=%f g=%f b=%f), transmission = (r=%f g=%f b=%f))", strInGeometryName, cColor.red, cColor.green, cColor.blue, fAlpha, fAlpha, fAlpha);
		}

		HC_Set_Color(strColorText);
	}
}

//== MaterialMappingKit ============================================================================

namespace H3DF
{
	class MaterialMappingKitImpl : public Impl
	{
	public:
		enum class Type : uint32_t
		{
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

		MaterialMappingKitImpl();

		void Copy(MaterialMappingKitImpl * pcInThat) {
			for (int nTypeIndex = 0; nTypeIndex < (int) Type::Count; nTypeIndex++) {
				m_cMaterial[nTypeIndex] = pcInThat->m_cMaterial[nTypeIndex];
				m_bMaterialFlag[nTypeIndex] = pcInThat->m_bMaterialFlag[nTypeIndex];
			}
		}

		void SetColor(RGBAColor const & cInRgbaColor, H3DF::MaterialMappingKitImpl::Type eType, Material::Color::Channel eChannel);

		bool ShowChannel(H3DF::MaterialMappingKitImpl::Type eInType, Material::Channel eInChannel, Material::Type & cOutType, RGBAColor & cOutColor, CString & strOutTextureName) const;
		bool ShowMaterial(H3DF::MaterialMappingKitImpl::Type eInType, MaterialKit & cOutKit) const;

		MaterialKit m_cMaterial[(int) Type::Count];
		bool m_bMaterialFlag[(int) Type::Count];
	};
}

H3DF::MaterialMappingKitImpl::MaterialMappingKitImpl()
{
	m_eType = H3DF::Type::MaterialMappingKit;

	for(bool & bMaterialFlag : m_bMaterialFlag) {
		bMaterialFlag = false;
	}
}

void H3DF::MaterialMappingKitImpl::SetColor(RGBAColor const & cInRgbaColor, H3DF::MaterialMappingKitImpl::Type eType, Material::Color::Channel eChannel)
{
	switch (eChannel)
	{
		case H3DF::Material::Color::Channel::DiffuseColor:
			m_cMaterial[(int)eType].SetDiffuseColor(cInRgbaColor);
			break;

		case H3DF::Material::Color::Channel::Specular:
			m_cMaterial[(int)eType].SetSpecular(cInRgbaColor);
			break;

		case H3DF::Material::Color::Channel::Emission:
			m_cMaterial[(int)eType].SetDiffuseColor(cInRgbaColor);
			break;

		case H3DF::Material::Color::Channel::Mirror:
			m_cMaterial[(int)eType].SetDiffuseColor(cInRgbaColor);
			break;
	}

	m_bMaterialFlag[(int)eType] = true;
}

bool H3DF::MaterialMappingKitImpl::ShowChannel(H3DF::MaterialMappingKitImpl::Type eInType, Material::Channel eInChannel, Material::Type & cOutType, RGBAColor & cOutColor, CString & strOutTextureName) const
{
	MaterialKitImpl * pcImpl = (MaterialKitImpl *)m_cMaterial[(int)eInType].GetImpl();
	return pcImpl->ShowChannel(eInChannel, cOutType, cOutColor, strOutTextureName);
}

bool H3DF::MaterialMappingKitImpl::ShowMaterial(H3DF::MaterialMappingKitImpl::Type eInType, MaterialKit & cOutKit) const
{
	if (true == m_cMaterial[(int)eInType].Empty()) {
		return false;
	}

	cOutKit = m_cMaterial[(int)eInType];
	return true;
}

H3DF::MaterialMappingKit::MaterialMappingKit()
{
	m_pcImpl = new MaterialMappingKitImpl();
}

H3DF::MaterialMappingKit::MaterialMappingKit(MaterialMappingKit const & cInKit)
{
	m_pcImpl = new MaterialMappingKitImpl();
	Set(cInKit);
}

void H3DF::MaterialMappingKit::Set(MaterialMappingKit const & cInThat)
{
	MaterialMappingKitImpl * pcImpl = (MaterialMappingKitImpl *)m_pcImpl;
	MaterialMappingKitImpl * pcInThatImpl = (MaterialMappingKitImpl *)cInThat.m_pcImpl;
	pcImpl->Copy(pcInThatImpl);
}

MaterialMappingKit & H3DF::MaterialMappingKit::operator = (MaterialMappingKit const & cInThat)
{
	Set(cInThat);
	return *this;
}

//== Color 설정 =====================================================================================

MaterialMappingKit & H3DF::MaterialMappingKit::SetAmbientLightUpColor(RGBAColor const & cInRgbaColor)
{
	MaterialMappingKitImpl * pcImpl = (MaterialMappingKitImpl *)m_pcImpl;
	pcImpl->m_cMaterial[(int)H3DF::MaterialMappingKitImpl::Type::AmbientLightUp].SetDiffuseColor(cInRgbaColor);
	return *this;
}

MaterialMappingKit & H3DF::MaterialMappingKit::SetAmbientLightDownColor(RGBAColor const & cInRgbaColor)
{
	MaterialMappingKitImpl * pcImpl = (MaterialMappingKitImpl *)m_pcImpl;
	pcImpl->m_cMaterial[(int)H3DF::MaterialMappingKitImpl::Type::AmbientLightDown].SetDiffuseColor(cInRgbaColor);
	return *this;
}

MaterialMappingKit & H3DF::MaterialMappingKit::SetBackFaceColor(RGBAColor const & cInRgbaColor, Material::Color::Channel eInChannel)
{
	MaterialMappingKitImpl * pcImpl = (MaterialMappingKitImpl *)m_pcImpl;
	pcImpl->SetColor(cInRgbaColor, H3DF::MaterialMappingKitImpl::Type::BackFace, eInChannel);
	return *this;
}

MaterialMappingKit & H3DF::MaterialMappingKit::SetBackFaceAlpha(float fInAlpha)
{
	MaterialMappingKitImpl * pcImpl = (MaterialMappingKitImpl *)m_pcImpl;
	pcImpl->m_cMaterial[(int)H3DF::MaterialMappingKitImpl::Type::BackFace].SetDiffuseAlpha(fInAlpha);
	return *this;
}

MaterialMappingKit & H3DF::MaterialMappingKit::SetBackFaceTexture(CString strTextureName)
{
	MaterialMappingKitImpl * pcImpl = (MaterialMappingKitImpl *)m_pcImpl;
	pcImpl->m_cMaterial[(int)H3DF::MaterialMappingKitImpl::Type::BackFace].SetDiffuseTexture(strTextureName);
	return *this;
}

MaterialMappingKit & H3DF::MaterialMappingKit::SetBackFaceTextureOption(CString strTextureOption)
{
	MaterialMappingKitImpl * pcImpl = (MaterialMappingKitImpl *)m_pcImpl;
	pcImpl->m_cMaterial[(int)H3DF::MaterialMappingKitImpl::Type::BackFace].SetDiffuseTextureOption(strTextureOption);
	return *this;
}

MaterialMappingKit & H3DF::MaterialMappingKit::SetBackFaceGloss(float fInValue)
{
	MaterialMappingKitImpl * pcImpl = (MaterialMappingKitImpl *)m_pcImpl;
	pcImpl->m_cMaterial[(int)H3DF::MaterialMappingKitImpl::Type::BackFace].SetGloss(fInValue);
	return *this;
}

MaterialMappingKit & H3DF::MaterialMappingKit::SetBackFaceMaterial(MaterialKit const & cInMaterial)
{
	MaterialMappingKitImpl * pcImpl = (MaterialMappingKitImpl *)m_pcImpl;
	pcImpl->m_cMaterial[(int)H3DF::MaterialMappingKitImpl::Type::BackFace] = cInMaterial;
	return *this;
}

MaterialMappingKit & H3DF::MaterialMappingKit::SetFrontFaceColor(RGBAColor const & cInRgbaColor, Material::Color::Channel eInChannel)
{
	MaterialMappingKitImpl * pcImpl = (MaterialMappingKitImpl *)m_pcImpl;
	pcImpl->SetColor(cInRgbaColor, H3DF::MaterialMappingKitImpl::Type::FrontFace, eInChannel);
	return *this;
}

MaterialMappingKit & H3DF::MaterialMappingKit::SetFrontFaceAlpha(float fInAlpha)
{
	MaterialMappingKitImpl * pcImpl = (MaterialMappingKitImpl *)m_pcImpl;
	pcImpl->m_cMaterial[(int)H3DF::MaterialMappingKitImpl::Type::FrontFace].SetDiffuseAlpha(fInAlpha);
	return *this;
}

MaterialMappingKit & H3DF::MaterialMappingKit::SetFrontFaceTexture(CString strTextureName)
{
	MaterialMappingKitImpl * pcImpl = (MaterialMappingKitImpl *)m_pcImpl;
	pcImpl->m_cMaterial[(int)H3DF::MaterialMappingKitImpl::Type::FrontFace].SetDiffuseTexture(strTextureName);
	return *this;
}

MaterialMappingKit & H3DF::MaterialMappingKit::SetFrontFaceTextureOption(CString strTextureOption)
{
	MaterialMappingKitImpl * pcImpl = (MaterialMappingKitImpl *)m_pcImpl;
	pcImpl->m_cMaterial[(int)H3DF::MaterialMappingKitImpl::Type::FrontFace].SetDiffuseTextureOption(strTextureOption);
	return *this;
}

MaterialMappingKit & H3DF::MaterialMappingKit::SetFrontFaceGloss(float fInValue)
{
	MaterialMappingKitImpl * pcImpl = (MaterialMappingKitImpl *)m_pcImpl;
	pcImpl->m_cMaterial[(int)H3DF::MaterialMappingKitImpl::Type::FrontFace].SetGloss(fInValue);
	return *this;
}

MaterialMappingKit & H3DF::MaterialMappingKit::SetFrontFaceMaterial(MaterialKit const & cInMaterial)
{
	MaterialMappingKitImpl * pcImpl = (MaterialMappingKitImpl *)m_pcImpl;
	pcImpl->m_cMaterial[(int)H3DF::MaterialMappingKitImpl::Type::FrontFace] = cInMaterial;
	return *this;
}

MaterialMappingKit & H3DF::MaterialMappingKit::SetEdgeAlpha(float fInAlpha)
{
	MaterialMappingKitImpl * pcImpl = (MaterialMappingKitImpl *)m_pcImpl;
	pcImpl->m_cMaterial[(int)H3DF::MaterialMappingKitImpl::Type::Edge].SetDiffuseAlpha(fInAlpha);
	return *this;
}

MaterialMappingKit & H3DF::MaterialMappingKit::SetEdgeColor(RGBAColor const & cInRgbaColor, Material::Color::Channel eInChannel)
{
	MaterialMappingKitImpl * pcImpl = (MaterialMappingKitImpl *)m_pcImpl;
	pcImpl->SetColor(cInRgbaColor, H3DF::MaterialMappingKitImpl::Type::Edge, eInChannel);
	return *this;
}

MaterialMappingKit & H3DF::MaterialMappingKit::SetEdgeTexture(char const * strTextureName)
{
	MaterialMappingKitImpl * pcImpl = (MaterialMappingKitImpl *)m_pcImpl;
	pcImpl->m_cMaterial[(int)H3DF::MaterialMappingKitImpl::Type::Edge].SetDiffuseTexture(strTextureName);
	return *this;
}

MaterialMappingKit & H3DF::MaterialMappingKit::SetEdgeGloss(float fInValue)
{
	MaterialMappingKitImpl * pcImpl = (MaterialMappingKitImpl *)m_pcImpl;
	pcImpl->m_cMaterial[(int)H3DF::MaterialMappingKitImpl::Type::Edge].SetGloss(fInValue);
	return *this;
}

MaterialMappingKit & H3DF::MaterialMappingKit::SetEdgeMaterial(MaterialKit const & cInMaterial)
{
	MaterialMappingKitImpl * pcImpl = (MaterialMappingKitImpl *)m_pcImpl;
	pcImpl->m_cMaterial[(int)H3DF::MaterialMappingKitImpl::Type::Edge] = cInMaterial;
	return *this;
}

MaterialMappingKit & H3DF::MaterialMappingKit::SetFaceColor(RGBAColor const & cInRgbaColor, Material::Color::Channel eInChannel)
{
	MaterialMappingKitImpl * pcImpl = (MaterialMappingKitImpl *)m_pcImpl;
	pcImpl->SetColor(cInRgbaColor, H3DF::MaterialMappingKitImpl::Type::Face, eInChannel);
	return *this;
}

MaterialMappingKit & H3DF::MaterialMappingKit::SetFaceAlpha(float fInAlpha)
{
	MaterialMappingKitImpl * pcImpl = (MaterialMappingKitImpl *)m_pcImpl;
	pcImpl->m_cMaterial[(int)H3DF::MaterialMappingKitImpl::Type::Face].SetDiffuseAlpha(fInAlpha);
	return *this;
}

MaterialMappingKit & H3DF::MaterialMappingKit::SetFaceTexture(CString strTextureName)
{
	MaterialMappingKitImpl * pcImpl = (MaterialMappingKitImpl *)m_pcImpl;
	pcImpl->m_cMaterial[(int)H3DF::MaterialMappingKitImpl::Type::Face].SetDiffuseTexture(strTextureName);
	return *this;
}

MaterialMappingKit & H3DF::MaterialMappingKit::SetFaceTextureOption(CString strTextureOption)
{
	MaterialMappingKitImpl * pcImpl = (MaterialMappingKitImpl *)m_pcImpl;
	pcImpl->m_cMaterial[(int)H3DF::MaterialMappingKitImpl::Type::Face].SetDiffuseTextureOption(strTextureOption);
	return *this;
}

MaterialMappingKit & H3DF::MaterialMappingKit::SetFaceGloss(float fInValue)
{
	MaterialMappingKitImpl * pcImpl = (MaterialMappingKitImpl *)m_pcImpl;
	pcImpl->m_cMaterial[(int)H3DF::MaterialMappingKitImpl::Type::Face].SetGloss(fInValue);
	return *this;
}

MaterialMappingKit & H3DF::MaterialMappingKit::SetFaceMaterial(MaterialKit const & cInMaterial)
{
	MaterialMappingKitImpl * pcImpl = (MaterialMappingKitImpl *)m_pcImpl;
	pcImpl->m_cMaterial[(int)H3DF::MaterialMappingKitImpl::Type::Face] = cInMaterial;
	return *this;
}

MaterialMappingKit & H3DF::MaterialMappingKit::SetLineAlpha(float fInAlpha)
{
	MaterialMappingKitImpl * pcImpl = (MaterialMappingKitImpl *)m_pcImpl;
	pcImpl->m_cMaterial[(int)H3DF::MaterialMappingKitImpl::Type::Line].SetDiffuseAlpha(fInAlpha);
	return *this;
}

MaterialMappingKit & H3DF::MaterialMappingKit::SetLineColor(RGBAColor const & cInRgbaColor)
{
	MaterialMappingKitImpl * pcImpl = (MaterialMappingKitImpl *)m_pcImpl;
	pcImpl->m_cMaterial[(int)H3DF::MaterialMappingKitImpl::Type::Line].SetDiffuseColor(cInRgbaColor);
	return *this;
}

MaterialMappingKit & H3DF::MaterialMappingKit::SetMarkerColor(RGBAColor const & cInRgbaColor)
{
	MaterialMappingKitImpl * pcImpl = (MaterialMappingKitImpl *)m_pcImpl;
	pcImpl->m_cMaterial[(int)H3DF::MaterialMappingKitImpl::Type::Marker].SetDiffuseColor(cInRgbaColor);
	return *this;
}

MaterialMappingKit & H3DF::MaterialMappingKit::SetTextColor(RGBAColor const & cInRgbaColor)
{
	MaterialMappingKitImpl * pcImpl = (MaterialMappingKitImpl *)m_pcImpl;
	pcImpl->m_cMaterial[(int)H3DF::MaterialMappingKitImpl::Type::Text].SetDiffuseColor(cInRgbaColor);
	return *this;
}

MaterialMappingKit & H3DF::MaterialMappingKit::SetVertexAlpha(float fInAlpha)
{
	MaterialMappingKitImpl * pcImpl = (MaterialMappingKitImpl *)m_pcImpl;
	pcImpl->m_cMaterial[(int)H3DF::MaterialMappingKitImpl::Type::Vertex].SetDiffuseAlpha(fInAlpha);
	return *this;
}

MaterialMappingKit & H3DF::MaterialMappingKit::SetVertexColor(RGBAColor const & cInRgbaColor, Material::Color::Channel eInChannel)
{
	MaterialMappingKitImpl * pcImpl = (MaterialMappingKitImpl *)m_pcImpl;
	pcImpl->SetColor(cInRgbaColor, H3DF::MaterialMappingKitImpl::Type::Vertex, eInChannel);
	return *this;
}

MaterialMappingKit & H3DF::MaterialMappingKit::SetVertexTexture(CString strTextureName)
{
	MaterialMappingKitImpl * pcImpl = (MaterialMappingKitImpl *)m_pcImpl;
	pcImpl->m_cMaterial[(int)H3DF::MaterialMappingKitImpl::Type::Vertex].SetDiffuseTexture(strTextureName);
	return *this;
}

MaterialMappingKit & H3DF::MaterialMappingKit::SetVertexGloss(float fInValue)
{
	MaterialMappingKitImpl * pcImpl = (MaterialMappingKitImpl *)m_pcImpl;
	pcImpl->m_cMaterial[(int)H3DF::MaterialMappingKitImpl::Type::Vertex].SetGloss(fInValue);
	return *this;
}

MaterialMappingKit & H3DF::MaterialMappingKit::SetVertexMaterial(MaterialKit const & cInMaterial)
{
	MaterialMappingKitImpl * pcImpl = (MaterialMappingKitImpl *)m_pcImpl;
	pcImpl->m_cMaterial[(int)H3DF::MaterialMappingKitImpl::Type::Vertex] = cInMaterial;
	return *this;
}

MaterialMappingKit & H3DF::MaterialMappingKit::SetGeometryColor(RGBAColor const & cInRgbaColor)
{
	SetEdgeColor(cInRgbaColor);
	SetFaceColor(cInRgbaColor);
	SetLineColor(cInRgbaColor);
	SetMarkerColor(cInRgbaColor);
	SetTextColor(cInRgbaColor);
	SetVertexColor(cInRgbaColor);

	return *this;
}

bool H3DF::MaterialMappingKit::operator == (MaterialMappingKit const & cInThat) const
{
	MaterialMappingKitImpl * pcImpl = (MaterialMappingKitImpl *)m_pcImpl;
	MaterialMappingKitImpl * pcInThatImpl = (MaterialMappingKitImpl *)cInThat.m_pcImpl;

	for (int nTypeIndex = 0; nTypeIndex < (int)H3DF::MaterialMappingKitImpl::Type::Count; nTypeIndex++) {
		if (pcImpl->m_cMaterial[nTypeIndex] != pcInThatImpl->m_cMaterial[nTypeIndex]) {
			return false;
		}
	}

	return true;
}

bool H3DF::MaterialMappingKit::operator != (MaterialMappingKit const & cInThat) const
{
	if (*this == cInThat) {
		return false;
	}

	return true;
}

bool H3DF::MaterialMappingKit::ShowAmbientLightUpColor(Material::Type & cOutType, RGBAColor & cOutColor) const
{
	MaterialMappingKitImpl * pcImpl = (MaterialMappingKitImpl *)m_pcImpl;
	return pcImpl->m_cMaterial[(int)H3DF::MaterialMappingKitImpl::Type::AmbientLightUp].ShowDiffuseColor(cOutColor);

}

bool H3DF::MaterialMappingKit::ShowAmbientLightDownColor(Material::Type & cOutType, RGBAColor & cOutColor) const
{
	MaterialMappingKitImpl * pcImpl = (MaterialMappingKitImpl *)m_pcImpl;
	return pcImpl->m_cMaterial[(int)H3DF::MaterialMappingKitImpl::Type::AmbientLightDown].ShowDiffuseColor(cOutColor);
}

bool H3DF::MaterialMappingKit::ShowBackFaceChannel(Material::Channel eInChannel, Material::Type & cOutType, RGBAColor & cOutColor, CString & strOutTextureName) const
{
	MaterialMappingKitImpl * pcImpl = (MaterialMappingKitImpl *)m_pcImpl;
	return pcImpl->ShowChannel(H3DF::MaterialMappingKitImpl::Type::BackFace, eInChannel, cOutType, cOutColor, strOutTextureName);
}

bool H3DF::MaterialMappingKit::ShowBackFaceMaterial(MaterialKit & cOutKit) const
{
	MaterialMappingKitImpl * pcImpl = (MaterialMappingKitImpl *)m_pcImpl;
	return pcImpl->ShowMaterial(H3DF::MaterialMappingKitImpl::Type::BackFace, cOutKit);
}

bool H3DF::MaterialMappingKit::ShowFrontFaceChannel(Material::Channel eInChannel, Material::Type & cOutType, RGBAColor & cOutColor, CString & strOutTextureName) const
{
	MaterialMappingKitImpl * pcImpl = (MaterialMappingKitImpl *)m_pcImpl;
	return pcImpl->ShowChannel(H3DF::MaterialMappingKitImpl::Type::FrontFace, eInChannel, cOutType, cOutColor, strOutTextureName);
}

bool H3DF::MaterialMappingKit::ShowFrontFaceMaterial(MaterialKit & cOutKit) const
{
	MaterialMappingKitImpl * pcImpl = (MaterialMappingKitImpl *)m_pcImpl;
	return pcImpl->ShowMaterial(H3DF::MaterialMappingKitImpl::Type::FrontFace, cOutKit);
}

bool H3DF::MaterialMappingKit::ShowEdgeChannel(Material::Channel eInChannel, Material::Type & cOutType, RGBAColor & cOutColor, CString & strOutTextureName) const
{
	MaterialMappingKitImpl * pcImpl = (MaterialMappingKitImpl *)m_pcImpl;
	return pcImpl->ShowChannel(H3DF::MaterialMappingKitImpl::Type::Edge, eInChannel, cOutType, cOutColor, strOutTextureName);
}

bool H3DF::MaterialMappingKit::ShowEdgeMaterial(MaterialKit & cOutKit) const
{
	MaterialMappingKitImpl * pcImpl = (MaterialMappingKitImpl *)m_pcImpl;
	return pcImpl->ShowMaterial(H3DF::MaterialMappingKitImpl::Type::Edge, cOutKit);
}

bool H3DF::MaterialMappingKit::ShowFaceChannel(Material::Channel eInChannel, Material::Type & cOutType, RGBAColor & cOutColor, CString & strOutTextureName) const
{
	MaterialMappingKitImpl * pcImpl = (MaterialMappingKitImpl *)m_pcImpl;
	return pcImpl->ShowChannel(H3DF::MaterialMappingKitImpl::Type::Face, eInChannel, cOutType, cOutColor, strOutTextureName);
}

bool H3DF::MaterialMappingKit::ShowFaceMaterial(MaterialKit & cOutKit) const
{
	MaterialMappingKitImpl * pcImpl = (MaterialMappingKitImpl *)m_pcImpl;
	return pcImpl->ShowMaterial(H3DF::MaterialMappingKitImpl::Type::Face, cOutKit);
}

bool H3DF::MaterialMappingKit::ShowLineAlpha(float & fOutAlpha) const
{
	MaterialMappingKitImpl * pcImpl = (MaterialMappingKitImpl *)m_pcImpl;
	return pcImpl->m_cMaterial[(int)H3DF::MaterialMappingKitImpl::Type::Line].ShowDiffuseAlpha(fOutAlpha);
}

bool H3DF::MaterialMappingKit::ShowLineColor(RGBAColor & cOutColor) const
{
	MaterialMappingKitImpl * pcImpl = (MaterialMappingKitImpl *)m_pcImpl;
	return pcImpl->m_cMaterial[(int)H3DF::MaterialMappingKitImpl::Type::Line].ShowDiffuseColor(cOutColor);
}

bool H3DF::MaterialMappingKit::ShowMarkerColor(RGBAColor & cOutColor) const
{
	MaterialMappingKitImpl * pcImpl = (MaterialMappingKitImpl *)m_pcImpl;
	return pcImpl->m_cMaterial[(int)H3DF::MaterialMappingKitImpl::Type::Marker].ShowDiffuseColor(cOutColor);
}

bool H3DF::MaterialMappingKit::ShowTextColor(RGBAColor & cOutColor) const
{
	MaterialMappingKitImpl * pcImpl = (MaterialMappingKitImpl *)m_pcImpl;
	return pcImpl->m_cMaterial[(int)H3DF::MaterialMappingKitImpl::Type::Text].ShowDiffuseColor(cOutColor);
}

bool H3DF::MaterialMappingKit::ShowVertexChannel(Material::Channel eInChannel, Material::Type & cOutType, RGBAColor & cOutColor, CString & strOutTextureName) const
{
	MaterialMappingKitImpl * pcImpl = (MaterialMappingKitImpl *)m_pcImpl;
	return pcImpl->ShowChannel(H3DF::MaterialMappingKitImpl::Type::Vertex, eInChannel, cOutType, cOutColor, strOutTextureName);
}

bool H3DF::MaterialMappingKit::ShowVertexMaterial(MaterialKit & cOutKit) const
{
	MaterialMappingKitImpl * pcImpl = (MaterialMappingKitImpl *)m_pcImpl;
	return pcImpl->ShowMaterial(H3DF::MaterialMappingKitImpl::Type::Vertex, cOutKit);
}

//== MaterialMappingControl ========================================================================
class MaterialMappingControlImpl : public ControlImpl
{
public:
	void Copy(ControlImpl * pcInThat)
	{
		ControlImpl::Copy(pcInThat);
	}

	void SetAlpha(CString strGeometry, float fInAlpha);
	void SetColor(CString strGeometry, RGBAColor const & cInRgbaColor, Material::Color::Channel cInChannel);

protected:
	CString GetColorChannelString(Material::Color::Channel cInChannel);
};

CString MaterialMappingControlImpl::GetColorChannelString(Material::Color::Channel cInChannel)
{
	switch (cInChannel)
	{
		case H3DF::Material::Color::Channel::DiffuseColor:
			return L"diffuse";

		case H3DF::Material::Color::Channel::Specular:
			return L"specular";

		case H3DF::Material::Color::Channel::Emission:
			return L"emission";

		case H3DF::Material::Color::Channel::Mirror:
			return L"mirror";
	}

	return L"";
}

void MaterialMappingControlImpl::SetAlpha(CString strGeometry, float fInAlpha)
{
	CString strColorText;
	strColorText.Format(L"%s = (transmission = r=%f g=%f b=%f)", strGeometry, fInAlpha, fInAlpha, fInAlpha);

	SegmentKeyImpl::LocalOpen(m_cOverrideKey); {
		HC_Set_Color(Utility::ToChar(strColorText));
	} SegmentKeyImpl::LocalClose(m_cOverrideKey);
}

void MaterialMappingControlImpl::SetColor(CString strGeometry, RGBAColor const & cInRgbaColor, Material::Color::Channel cInChannel)
{
	CString strColorText;
	CString strColorChannel = GetColorChannelString(cInChannel);

	if (1.0f > cInRgbaColor.alpha) {
		strColorText.Format(L"%s = (%s = (r=%f g=%f b=%f), (transmission = r=%f g=%f b=%f))", strGeometry, strColorChannel, 
			cInRgbaColor.red, cInRgbaColor.green, cInRgbaColor.blue,
			cInRgbaColor.alpha, cInRgbaColor.alpha, cInRgbaColor.alpha);
	}
	else {
		strColorText.Format(L"%s = (%s = (r=%f g=%f b=%f))", strGeometry, strColorChannel, cInRgbaColor.red, cInRgbaColor.green, cInRgbaColor.blue);
	}
	
	SegmentKeyImpl::LocalOpen(m_cOverrideKey); {
		HC_Set_Color(Utility::ToChar(strColorText));
	} SegmentKeyImpl::LocalClose(m_cOverrideKey);
}

H3DF::MaterialMappingControl::MaterialMappingControl(SegmentKey const & cInThat)
{
	MaterialMappingControlImpl * pcImpl = new MaterialMappingControlImpl();
	m_pcImpl = pcImpl;
	
	pcImpl->m_cOverrideKey = cInThat;
}

H3DF::MaterialMappingControl::MaterialMappingControl(MaterialMappingControl const & cInThat)
{
	m_pcImpl = new MaterialMappingControlImpl();
	Set(cInThat);
}

void H3DF::MaterialMappingControl::Set(MaterialMappingControl const & cInThat)
{
	MaterialMappingControlImpl * pcImpl = static_cast<MaterialMappingControlImpl *>(m_pcImpl);
	DEBUG_VALID(pcImpl);

	MaterialMappingControlImpl * pcInThatImpl = static_cast<MaterialMappingControlImpl *>(cInThat.m_pcImpl);
	DEBUG_VALID(pcInThatImpl);

	pcImpl->Copy(pcInThatImpl);
}

MaterialMappingControl & H3DF::MaterialMappingControl::operator = (MaterialMappingControl const & cInThat)
{
	Set(cInThat);
	return *this;
}

MaterialMappingControl & H3DF::MaterialMappingControl::SetFaceAlpha(float fInAlpha)
{
	MaterialMappingControlImpl * pcImpl = static_cast<MaterialMappingControlImpl *>(m_pcImpl);
	if (nullptr == pcImpl) { assert(false); }

	pcImpl->SetAlpha("faces", fInAlpha);

	return *this;
}

MaterialMappingControl & H3DF::MaterialMappingControl::SetFaceColor(RGBAColor const & cInRgbaColor, Material::Color::Channel cInChannel)
{
	MaterialMappingControlImpl * pcImpl = static_cast<MaterialMappingControlImpl *>(m_pcImpl);
	if (nullptr == pcImpl) { assert(false); }

	pcImpl->SetColor("faces", cInRgbaColor, cInChannel);

	return *this;
}

MaterialMappingControl & H3DF::MaterialMappingControl::SetEdgeColor(RGBAColor const & cInRgbaColor, Material::Color::Channel cInChannel)
{
	MaterialMappingControlImpl * pcImpl = static_cast<MaterialMappingControlImpl *>(m_pcImpl);
	if (nullptr == pcImpl) { assert(false); }

	pcImpl->SetColor("edges", cInRgbaColor, cInChannel);

	return *this;
}

MaterialMappingControl & H3DF::MaterialMappingControl::SetMarkerColor(RGBAColor const & cInRgbaColor, Material::Color::Channel cInChannel)
{
	MaterialMappingControlImpl * pcImpl = static_cast<MaterialMappingControlImpl *>(m_pcImpl);
	if (nullptr == pcImpl) { assert(false); }

	pcImpl->SetColor("markers", cInRgbaColor, cInChannel);

	return *this;
}

MaterialMappingControl & H3DF::MaterialMappingControl::UnSetColor(CString strInType)
{
	MaterialMappingControlImpl * pcImpl = static_cast<MaterialMappingControlImpl *>(m_pcImpl);
	if (nullptr == pcImpl) { assert(false); }

	SegmentKeyImpl::LocalOpen(pcImpl->m_cOverrideKey); {
		HC_UnSet_One_Selectability(Utility::ToChar(strInType));
	}SegmentKeyImpl::LocalClose(pcImpl->m_cOverrideKey);

	return *this;
}

void H3DF::MaterialMappingControl::InitPopulateTextures()
{
	MaterialMappingControlImpl * pcImpl = static_cast<MaterialMappingControlImpl *>(m_pcImpl);
	if (nullptr == pcImpl) { assert(false); }

	InitializeMagick(".");

	SegmentKeyImpl::LocalOpen(pcImpl->m_cOverrideKey); {
		HC_Set_Visibility("image = off");
	} SegmentKeyImpl::LocalClose(pcImpl->m_cOverrideKey);
}

void H3DF::MaterialMappingControl::InsertPicture(UINT nIndex, UINT nPixelWidth, UINT nPixelHeight, UCHAR * pucBinaryData)
{
	MaterialMappingControlImpl * pcImpl = static_cast<MaterialMappingControlImpl *>(m_pcImpl);
	if (nullptr == pcImpl) { assert(false); }

	SegmentKeyImpl::LocalOpen(pcImpl->m_cOverrideKey); {
		//HC_KEY nKey = HC_Insert_Image(0.0, 0.0, 0.0, H_FORMAT_TEXT("rgba, name = image %u, local = on", nIndex), nPixelWidth, nPixelHeight, pucBinaryData);
		HC_KEY nKey = HC_Insert_Image(0.0, 0.0, 0.0, H_FORMAT_TEXT("rgba, name = image %u", nIndex), nPixelWidth, nPixelHeight, pucBinaryData);
	}SegmentKeyImpl::LocalClose(pcImpl->m_cOverrideKey);
}

void H3DF::MaterialMappingControl::InsertDifaultPicture(UINT nIndex, UINT nSize, UCHAR * pucBinaryData)
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

		MaterialMappingControlImpl * pcImpl = static_cast<MaterialMappingControlImpl *>(m_pcImpl);
		if (nullptr == pcImpl) { assert(false); }

		SegmentKeyImpl::LocalOpen(pcImpl->m_cOverrideKey); {

			//HC_KEY nKey = HC_Insert_Image(0.0, 0.0, 0.0, H_FORMAT_TEXT("rgba, name = image %u, local = on", nIndex), width, height, &anPixels[0]);
			HC_KEY nKey = HC_Insert_Image(0.0, 0.0, 0.0, H_FORMAT_TEXT("rgba, name = image %u", nIndex), width, height, &anPixels[0]);

		} SegmentKeyImpl::LocalClose(pcImpl->m_cOverrideKey);
	}
}

void H3DF::MaterialMappingControl::SetTextureMatrix(float * pfTextureMatrix, char * pchTextureTransformSegment)
{
	MaterialMappingControlImpl * pcImpl = static_cast<MaterialMappingControlImpl *>(m_pcImpl);
	if (nullptr == pcImpl) { assert(false); }

	SegmentKeyImpl::LocalOpen(pcImpl->m_cOverrideKey); {

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

	} SegmentKeyImpl::LocalClose(pcImpl->m_cOverrideKey);
}

void H3DF::MaterialMappingControl::SetDefineLocalTexture(UINT nIndex, CString strTextureOptions)
{
	MaterialMappingControlImpl * pcImpl = static_cast<MaterialMappingControlImpl *>(m_pcImpl);
	if (nullptr == pcImpl) { assert(false); }

	CString strText;
	strText.Format(L"texture_%u", nIndex);

	SegmentKeyImpl::LocalOpen(pcImpl->m_cOverrideKey); {
		HC_Define_Local_Texture(Utility::ToChar(strText), Utility::ToChar(strTextureOptions));
	} SegmentKeyImpl::LocalClose(pcImpl->m_cOverrideKey);
}

void H3DF::MaterialMappingControl::EndPopulateTextures()
{
	DestroyMagick();
}
