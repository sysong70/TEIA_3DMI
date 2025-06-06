#include "StdAfx.h"

#include "Material.h"

#include "Segment.h"
#include "./Impl/SegmentImpl.h"

#include "./Impl/ControlImpl.h"

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
	std::unique_ptr<Impl> Clone() const override {
		auto pcClone = std::make_unique<MaterialKitImpl>();
		pcClone->Copy(this);
		return pcClone;
	}

	void Copy(const MaterialKitImpl * pcInThat) {
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
	bool ShowTexture(Material::Channel eInChannel, Material::Type & cOutType, RGBAColor & cOutColor, CStringA & strOutTextureName, CStringA & strOutTextureOptions) const;
	bool ShowChannel(Material::Channel eInChannel, Material::Type & cOutType, RGBAColor & cOutColor, CStringA & strOutTextureName, CStringA & strOutTextureOptions) const;

	RGBAColor m_pcColors[(int)Material::Channel::Count];
	float m_fGloss = -1.f;

	bool m_bTextureMirrors[(int)Material::Channel::Count]{};
	CStringA m_strTextureNames[(int)Material::Channel::Count];;
	CStringA m_strTextureOptions[(int)Material::Channel::Count];;
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

bool MaterialKitImpl::ShowTexture(Material::Channel eInChannel, Material::Type & cOutType, RGBAColor & cOutColor, CStringA & strOutTextureName, CStringA & strOutTextureOptions) const
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

	strOutTextureOptions = m_strTextureOptions[(int)eInChannel];

	return true;
}

bool MaterialKitImpl::ShowChannel(Material::Channel eInChannel, Material::Type & cOutType, RGBAColor & cOutColor, CStringA & strOutTextureName, CStringA & strOutTextureOptions) const
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

	strOutTextureOptions = m_strTextureOptions[(int)eInChannel];

	return true;
}

H3DF::MaterialKit::MaterialKit()
{
	if (staticType != Type()) {
		return;
	}

	m_pcImpl = std::make_unique<MaterialKitImpl>();
}

H3DF::MaterialKit::MaterialKit(MaterialKit const & cInKit)
{
	if (staticType != Type()) {
		return;
	}

	m_pcImpl = (nullptr == cInKit.m_pcImpl) ? cInKit.m_pcImpl->Clone() : nullptr;
}

MaterialKit & H3DF::MaterialKit::operator = (MaterialKit const & cInThat)
{
	if (nullptr != cInThat.m_pcImpl) {
		m_pcImpl = cInThat.m_pcImpl->Clone();
	}
	else {
		m_pcImpl.reset();
	}

	return *this;
}

bool H3DF::MaterialKit::operator == (MaterialKit const & cInThat) const
{
	auto pcImpl = static_cast<MaterialKitImpl *>(m_pcImpl.get());
	auto pcInThatImpl = static_cast<const MaterialKitImpl *>(cInThat.GetImpl());

	return (pcImpl == pcInThatImpl);
}

bool H3DF::MaterialKit::operator != (MaterialKit const & cInThat) const
{
	return !(*this == cInThat);
}

void H3DF::MaterialKit::Show(MaterialKit & cOutKit) const
{
	auto pcImpl = static_cast<MaterialKitImpl *>(m_pcImpl.get());
	auto pcOutImpl = static_cast<MaterialKitImpl *>(cOutKit.m_pcImpl.get());

	if (nullptr == pcImpl) {
		DEBUG_STOP;
		return;
	}

	if (nullptr == pcOutImpl) {
		// cOutKit이 Impl을 아직 할당받지 않았다면 새로 할당
		cOutKit.m_pcImpl = std::make_unique<MaterialKitImpl>();
		pcOutImpl = static_cast<MaterialKitImpl *>(cOutKit.m_pcImpl.get());
	}

	// 복사 (Copy 함수가 있다면 Copy 활용)
	pcOutImpl->Copy(pcImpl);
}

bool H3DF::MaterialKit::Empty() const
{
	auto pcImpl = static_cast<MaterialKitImpl *>(m_pcImpl.get());
	return pcImpl->Empty();
}

MaterialKit & H3DF::MaterialKit::SetDiffuse(RGBColor const & cInColor)
{
	auto pcImpl = static_cast<MaterialKitImpl *>(m_pcImpl.get());
	pcImpl->m_pcColors[(int)Material::Channel::DiffuseColor] = cInColor;
	pcImpl->m_pcColors[(int)Material::Channel::DiffuseTexture] = cInColor;
	return *this;
}

MaterialKit & H3DF::MaterialKit::SetDiffuse(RGBAColor const & cInColor)
{
	auto pcImpl = static_cast<MaterialKitImpl *>(m_pcImpl.get());
	pcImpl->m_pcColors[(int)Material::Channel::DiffuseColor] = cInColor;
	pcImpl->m_pcColors[(int)Material::Channel::DiffuseTexture] = cInColor;
	return *this;
}

MaterialKit & H3DF::MaterialKit::SetDiffuseColor(RGBColor const & cInColor)
{
	auto pcImpl = static_cast<MaterialKitImpl *>(m_pcImpl.get());
	pcImpl->m_pcColors[(int)Material::Channel::DiffuseColor] = cInColor;
	return *this;
}

MaterialKit & H3DF::MaterialKit::SetDiffuseColor(RGBAColor const & cInColor)
{
	auto pcImpl = static_cast<MaterialKitImpl *>(m_pcImpl.get());
	pcImpl->m_pcColors[(int)Material::Channel::DiffuseColor] = cInColor;
	return *this;
}

MaterialKit & H3DF::MaterialKit::SetDiffuseAlpha(float fInAlpha)
{
	auto pcImpl = static_cast<MaterialKitImpl *>(m_pcImpl.get());
	pcImpl->m_pcColors[(int)Material::Channel::DiffuseColor].alpha = fInAlpha;
	return *this;
}

MaterialKit & H3DF::MaterialKit::SetDiffuseTexture(CStringA strTextureName)
{
	auto pcImpl = static_cast<MaterialKitImpl *>(m_pcImpl.get());
	pcImpl->m_strTextureNames[(int)Material::Channel::DiffuseTexture] = strTextureName;
	return *this;
}

MaterialKit & H3DF::MaterialKit::SetDiffuseTexture(CStringA strTextureName, RGBAColor const & cInModulatingColor)
{
	auto pcImpl = static_cast<MaterialKitImpl *>(m_pcImpl.get());
	pcImpl->m_strTextureNames[(int)Material::Channel::DiffuseTexture] = strTextureName;
	pcImpl->m_pcColors[(int)Material::Channel::DiffuseTexture] = cInModulatingColor;
	return *this;
}

MaterialKit & H3DF::MaterialKit::SetDiffuseTextureOption(CStringA strTextureOption)
{
	auto pcImpl = static_cast<MaterialKitImpl *>(m_pcImpl.get());
	pcImpl->m_strTextureOptions[(int)Material::Channel::DiffuseTexture] = strTextureOption;
	return *this;
}

MaterialKit & H3DF::MaterialKit::SetSpecular(RGBAColor const & cInColor)
{
	auto pcImpl = static_cast<MaterialKitImpl *>(m_pcImpl.get());
	pcImpl->m_pcColors[(int)Material::Channel::Specular] = cInColor;
	return *this;
}

MaterialKit & H3DF::MaterialKit::SetSpecular(CStringA strTextureName, RGBAColor const & cInModulatingColor)
{
	auto pcImpl = static_cast<MaterialKitImpl *>(m_pcImpl.get());
	pcImpl->m_strTextureNames[(int)Material::Channel::Specular] = strTextureName;
	pcImpl->m_pcColors[(int)Material::Channel::Specular] = cInModulatingColor;
	return *this;
}

MaterialKit & H3DF::MaterialKit::SetMirror(RGBAColor const & cInColor)
{
	auto pcImpl = static_cast<MaterialKitImpl *>(m_pcImpl.get());
	pcImpl->m_pcColors[(int)Material::Channel::Mirror] = cInColor;
	return *this;
}

MaterialKit & H3DF::MaterialKit::SetMirror(CStringA strTextureName, RGBAColor const & cInModulatingColor)
{
	auto pcImpl = static_cast<MaterialKitImpl *>(m_pcImpl.get());
	pcImpl->m_strTextureNames[(int)Material::Channel::Mirror] = strTextureName;
	pcImpl->m_pcColors[(int)Material::Channel::Mirror] = cInModulatingColor;
	return *this;
}

MaterialKit & H3DF::MaterialKit::SetTransmission(CStringA strTextureName, RGBAColor const & cInModulatingColor)
{
	auto pcImpl = static_cast<MaterialKitImpl *>(m_pcImpl.get());
	pcImpl->m_strTextureNames[(int)Material::Channel::Transmission] = strTextureName;
	pcImpl->m_pcColors[(int)Material::Channel::Transmission] = cInModulatingColor;
	return *this;
}

MaterialKit & H3DF::MaterialKit::SetEmission(RGBAColor const & cInColor)
{
	auto pcImpl = static_cast<MaterialKitImpl *>(m_pcImpl.get());
	pcImpl->m_pcColors[(int)Material::Channel::Emission] = cInColor;
	return *this;
}

MaterialKit & H3DF::MaterialKit::SetEmission(CStringA strTextureName, RGBAColor const & cInModulatingColor)
{
	auto pcImpl = static_cast<MaterialKitImpl *>(m_pcImpl.get());
	pcImpl->m_strTextureNames[(int)Material::Channel::Emission] = strTextureName;
	pcImpl->m_pcColors[(int)Material::Channel::Emission] = cInModulatingColor;
	return *this;
}

MaterialKit & H3DF::MaterialKit::SetEnvironmentTexture(CStringA strTextureName)
{
	auto pcImpl = static_cast<MaterialKitImpl *>(m_pcImpl.get());
	pcImpl->m_strTextureNames[(int)Material::Channel::EnvironmentTexture] = strTextureName;
	return *this;
}

MaterialKit & H3DF::MaterialKit::SetEnvironmentTexture(CStringA strTextureName, RGBAColor const & cInModulatingColor)
{
	auto pcImpl = static_cast<MaterialKitImpl *>(m_pcImpl.get());
	pcImpl->m_strTextureNames[(int)Material::Channel::EnvironmentTexture] = strTextureName;
	pcImpl->m_pcColors[(int)Material::Channel::EnvironmentTexture] = cInModulatingColor;
	return *this;
}

MaterialKit & H3DF::MaterialKit::SetEnvironmentCubeMap(CStringA strTextureName)
{
	auto pcImpl = static_cast<MaterialKitImpl *>(m_pcImpl.get());
	pcImpl->m_strTextureNames[(int)Material::Channel::EnvironmentCubeMap] = strTextureName;
	return *this;
}

MaterialKit & H3DF::MaterialKit::SetEnvironmentCubeMap(CStringA strTextureName, RGBAColor const & cInModulatingColor)
{
	auto pcImpl = static_cast<MaterialKitImpl *>(m_pcImpl.get());
	pcImpl->m_strTextureNames[(int)Material::Channel::EnvironmentCubeMap] = strTextureName;
	pcImpl->m_pcColors[(int)Material::Channel::EnvironmentCubeMap] = cInModulatingColor;
	return *this;
}

MaterialKit & H3DF::MaterialKit::SetBump(CStringA strTextureName)
{
	auto pcImpl = static_cast<MaterialKitImpl *>(m_pcImpl.get());
	pcImpl->m_strTextureNames[(int)Material::Channel::Bump] = strTextureName;
	return *this;
}

// Gloss is always positive, and most surfaces have a gloss in the range of 1.0 to 30.0.
MaterialKit & H3DF::MaterialKit::SetGloss(float fInGloss)
{
	auto pcImpl = static_cast<MaterialKitImpl *>(m_pcImpl.get());
	pcImpl->m_fGloss = fInGloss;
	return *this;
}

// Removes all settings applied to the diffuse rgb channel.
MaterialKit & H3DF::MaterialKit::UnsetDiffuseColorRGB()
{
	auto pcImpl = static_cast<MaterialKitImpl *>(m_pcImpl.get());
	pcImpl->m_pcColors[(int)Material::Channel::DiffuseColor].Invalid();
	return *this;
}

// Removes all settings applied to the diffuse color channel including alpha.
MaterialKit & H3DF::MaterialKit::UnsetDiffuseColor()
{
	auto pcImpl = static_cast<MaterialKitImpl *>(m_pcImpl.get());
	pcImpl->m_pcColors[(int)Material::Channel::DiffuseColor].Invalid();
	pcImpl->m_pcColors[(int)Material::Channel::DiffuseTexture].Invalid();
	return *this;
}

// Removes all settings applied to the diffuse alpha channel.
MaterialKit & H3DF::MaterialKit::UnsetDiffuseAlpha()
{
	auto pcImpl = static_cast<MaterialKitImpl *>(m_pcImpl.get());
	pcImpl->m_pcColors[(int)Material::Channel::DiffuseColor].alpha = -1;
	return *this;
}

MaterialKit & H3DF::MaterialKit::UnsetDiffuseTexture()
{
	auto pcImpl = static_cast<MaterialKitImpl *>(m_pcImpl.get());
	pcImpl->m_strTextureNames[(int)Material::Channel::DiffuseTexture].Empty();
	return *this;
}

MaterialKit & H3DF::MaterialKit::UnsetSpecular()
{
	auto pcImpl = static_cast<MaterialKitImpl *>(m_pcImpl.get());
	pcImpl->m_pcColors[(int)Material::Channel::Specular].Invalid();
	pcImpl->m_strTextureNames[(int)Material::Channel::Specular].Empty();
	return *this;
}

MaterialKit & H3DF::MaterialKit::UnsetMirror()
{
	auto pcImpl = static_cast<MaterialKitImpl *>(m_pcImpl.get());
	pcImpl->m_pcColors[(int)Material::Channel::Mirror].Invalid();
	pcImpl->m_strTextureNames[(int)Material::Channel::Mirror].Empty();
	return *this;
}

MaterialKit & H3DF::MaterialKit::UnsetTransmission()
{
	auto pcImpl = static_cast<MaterialKitImpl *>(m_pcImpl.get());
	pcImpl->m_strTextureNames[(int)Material::Channel::Transmission].Empty();
	return *this;
}

MaterialKit & H3DF::MaterialKit::UnsetEmission()
{
	auto pcImpl = static_cast<MaterialKitImpl *>(m_pcImpl.get());
	pcImpl->m_pcColors[(int)Material::Channel::Emission].Invalid();
	pcImpl->m_strTextureNames[(int)Material::Channel::Emission].Empty();
	return *this;
}

MaterialKit & H3DF::MaterialKit::UnsetEnvironment()
{
	auto pcImpl = static_cast<MaterialKitImpl *>(m_pcImpl.get());
	pcImpl->m_strTextureNames[(int)Material::Channel::EnvironmentTexture].Empty();
	pcImpl->m_strTextureNames[(int)Material::Channel::EnvironmentCubeMap].Empty();
	return *this;
}

MaterialKit & H3DF::MaterialKit::UnsetBump()
{
	auto pcImpl = static_cast<MaterialKitImpl *>(m_pcImpl.get());
	pcImpl->m_strTextureNames[(int)Material::Channel::Bump].Empty();
	return *this;
}

MaterialKit & H3DF::MaterialKit::UnsetGloss()
{
	auto pcImpl = static_cast<MaterialKitImpl *>(m_pcImpl.get());
	pcImpl->m_fGloss = -1;
	return *this;
}

MaterialKit & H3DF::MaterialKit::UnsetEverything()
{
	auto pcImpl = static_cast<MaterialKitImpl *>(m_pcImpl.get());
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
	auto pcImpl = static_cast<MaterialKitImpl *>(m_pcImpl.get());
	RGBAColor cTempOutColor;
	if (false == pcImpl->ShowColor(Material::Channel::DiffuseColor, cTempOutColor)) {
		return false;
	}

	cOutColor.Set(cTempOutColor.red, cTempOutColor.green, cTempOutColor.blue);

	return true;
}

bool H3DF::MaterialKit::ShowDiffuseColor(RGBAColor & cOutColor) const
{
	auto pcImpl = static_cast<MaterialKitImpl *>(m_pcImpl.get());
	DEBUG_VALID(pcImpl);
	return pcImpl->ShowColor(Material::Channel::DiffuseColor, cOutColor);
}

bool H3DF::MaterialKit::ShowDiffuseAlpha(float & fOutAlpha) const
{
	auto pcImpl = static_cast<MaterialKitImpl *>(m_pcImpl.get());
	return pcImpl->ShowAlpah(Material::Channel::DiffuseColor, fOutAlpha);
}

bool H3DF::MaterialKit::ShowDiffuseTexture(Material::Type & cOutType, RGBAColor & cOutColor, CStringA & strOutTextureName, CStringA & strOutTextureOptions) const
{
	auto pcImpl = static_cast<MaterialKitImpl *>(m_pcImpl.get());
	return pcImpl->ShowTexture(Material::Channel::DiffuseTexture, cOutType, cOutColor, strOutTextureName, strOutTextureOptions);
}

bool H3DF::MaterialKit::ShowSpecular(Material::Type & cOutType, RGBAColor & cOutColor, CStringA & strOutTextureName, CStringA & strOutTextureOptions) const
{
	auto pcImpl = static_cast<MaterialKitImpl *>(m_pcImpl.get());
	return pcImpl->ShowTexture(Material::Channel::Specular, cOutType, cOutColor, strOutTextureName, strOutTextureOptions);
}

bool H3DF::MaterialKit::ShowMirror(Material::Type & cOutType, RGBAColor & cOutColor, CStringA & strOutTextureName, CStringA & strOutTextureOptions) const
{
	auto pcImpl = static_cast<MaterialKitImpl *>(m_pcImpl.get());
	return pcImpl->ShowTexture(Material::Channel::Mirror, cOutType, cOutColor, strOutTextureName, strOutTextureOptions);
}

bool H3DF::MaterialKit::ShowTransmission(Material::Type & cOutType, RGBAColor & cOutColor, CStringA & strOutTextureName, CStringA & strOutTextureOptions) const
{
	auto pcImpl = static_cast<MaterialKitImpl *>(m_pcImpl.get());
	return pcImpl->ShowTexture(Material::Channel::Transmission, cOutType, cOutColor, strOutTextureName, strOutTextureOptions);
}

bool H3DF::MaterialKit::ShowEmission(Material::Type & cOutType, RGBAColor & cOutColor, CStringA & strOutTextureName, CStringA & strOutTextureOptions) const
{
	auto pcImpl = static_cast<MaterialKitImpl *>(m_pcImpl.get());
	return pcImpl->ShowTexture(Material::Channel::Emission, cOutType, cOutColor, strOutTextureName, strOutTextureOptions);
}

bool H3DF::MaterialKit::ShowEnvironment(Material::Type & cOutType, RGBAColor & cOutColor, CStringA & strOutTextureName, CStringA & strOutTextureOptions) const
{
	auto pcImpl = static_cast<MaterialKitImpl *>(m_pcImpl.get());
	return pcImpl->ShowTexture(Material::Channel::EnvironmentTexture, cOutType, cOutColor, strOutTextureName, strOutTextureOptions);
}

bool H3DF::MaterialKit::ShowBump(CStringA & strOutTextureName, CStringA & strOutTextureOptions) const
{
	auto pcImpl = static_cast<MaterialKitImpl *>(m_pcImpl.get());
	if (true == pcImpl->m_strTextureNames[(int)Material::Channel::Bump].IsEmpty()) {
		return false;
	}

	strOutTextureName = pcImpl->m_strTextureNames[(int)Material::Channel::Bump];

	strOutTextureOptions = pcImpl->m_strTextureOptions[(int)Material::Channel::Bump];

	return true;
}

bool H3DF::MaterialKit::ShowGloss(float & fOutGloss) const
{
	auto pcImpl = static_cast<MaterialKitImpl *>(m_pcImpl.get());
	if (0 > pcImpl->m_fGloss) {
		return false;
	}

	fOutGloss = pcImpl->m_fGloss;

	return true;
}

// Region등에서 사용하기 위해서 추가된 함수
// 내부에서 단순히 SetColor만 호출한다.
void H3DF::MaterialKit::SetMaterial(CStringA strInGeometryName, bool bFaseFlag)
{
	auto pcImpl = static_cast<MaterialKitImpl *>(m_pcImpl.get());
	pcImpl->m_strTextureNames[(int)Material::Channel::DiffuseTexture] = strInGeometryName;

	CStringA strColorText;
	RGBAColor cColor;

	if (true == ShowDiffuseColor(cColor)) {

		if (true == bFaseFlag && 1.0f > cColor.alpha) {
			float fAlpha = 1.0f - cColor.alpha;
			strColorText.Format("%s = (diffuse = (r=%f g=%f b=%f), transmission = (r=%f g=%f b=%f))", strInGeometryName, cColor.red, cColor.green, cColor.blue, fAlpha, fAlpha, fAlpha);

		}
		else {
			strColorText.Format("%s = (diffuse = (r=%f g=%f b=%f))", strInGeometryName, cColor.red, cColor.green, cColor.blue);
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

		std::unique_ptr<Impl> Clone() const override {
			auto pcClone = std::make_unique<MaterialMappingKitImpl>();
			pcClone->Copy(this);
			return pcClone;
		}

		void Copy(const MaterialMappingKitImpl * pcInThat) {
			for (int nTypeIndex = 0; nTypeIndex < (int) Type::Count; nTypeIndex++) {
				m_cMaterial[nTypeIndex] = pcInThat->m_cMaterial[nTypeIndex];
				m_bMaterialFlag[nTypeIndex] = pcInThat->m_bMaterialFlag[nTypeIndex];
			}
		}

		void SetColor(RGBAColor const & cInRgbaColor, H3DF::MaterialMappingKitImpl::Type eType, Material::Color::Channel eChannel);
		bool ShowChannel(H3DF::MaterialMappingKitImpl::Type eInType, Material::Channel eInChannel, Material::Type & cOutType, RGBAColor & cOutColor, 
			CStringA & strOutTextureName, CStringA & strOutTextureOptions) const;
		bool ShowMaterial(H3DF::MaterialMappingKitImpl::Type eInType, MaterialKit & cOutKit) const;

		MaterialKit m_cMaterial[(int) Type::Count];
		bool m_bMaterialFlag[(int) Type::Count];
	};
}

H3DF::MaterialMappingKitImpl::MaterialMappingKitImpl()
{
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

bool H3DF::MaterialMappingKitImpl::ShowChannel(H3DF::MaterialMappingKitImpl::Type eInType, Material::Channel eInChannel, Material::Type & cOutType, RGBAColor & cOutColor, 
	CStringA & strOutTextureName, CStringA & strOutTextureOptions) const
{
	MaterialKitImpl * pcImpl = (MaterialKitImpl *)m_cMaterial[(int)eInType].GetImpl();
	return pcImpl->ShowChannel(eInChannel, cOutType, cOutColor, strOutTextureName, strOutTextureOptions);
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
	if (staticType != Type()) {
		return;
	}

	m_pcImpl = std::make_unique<MaterialMappingKitImpl>();
}

H3DF::MaterialMappingKit::MaterialMappingKit(MaterialMappingKit const & cInKit)
{
	if (staticType != Type()) {
		return;
	}

	m_pcImpl = (nullptr == cInKit.m_pcImpl) ? cInKit.m_pcImpl->Clone() : nullptr;
}

MaterialMappingKit & H3DF::MaterialMappingKit::operator = (MaterialMappingKit const & cInThat)
{
	if (nullptr != cInThat.m_pcImpl) {
		m_pcImpl = cInThat.m_pcImpl->Clone();
	}
	else {
		m_pcImpl.reset();
	}

	return *this;
}

//== Color 설정 =====================================================================================

MaterialMappingKit & H3DF::MaterialMappingKit::SetAmbientLightUpColor(RGBAColor const & cInRgbaColor)
{
	auto pcImpl = static_cast<MaterialMappingKitImpl *>(m_pcImpl.get());
	pcImpl->m_cMaterial[(int)H3DF::MaterialMappingKitImpl::Type::AmbientLightUp].SetDiffuseColor(cInRgbaColor);
	return *this;
}

MaterialMappingKit & H3DF::MaterialMappingKit::SetAmbientLightDownColor(RGBAColor const & cInRgbaColor)
{
	auto pcImpl = static_cast<MaterialMappingKitImpl *>(m_pcImpl.get());
	pcImpl->m_cMaterial[(int)H3DF::MaterialMappingKitImpl::Type::AmbientLightDown].SetDiffuseColor(cInRgbaColor);
	return *this;
}

MaterialMappingKit & H3DF::MaterialMappingKit::SetBackFaceColor(RGBAColor const & cInRgbaColor, Material::Color::Channel eInChannel)
{
	auto pcImpl = static_cast<MaterialMappingKitImpl *>(m_pcImpl.get());
	pcImpl->SetColor(cInRgbaColor, H3DF::MaterialMappingKitImpl::Type::BackFace, eInChannel);
	return *this;
}

MaterialMappingKit & H3DF::MaterialMappingKit::SetBackFaceAlpha(float fInAlpha)
{
	auto pcImpl = static_cast<MaterialMappingKitImpl *>(m_pcImpl.get());
	pcImpl->m_cMaterial[(int)H3DF::MaterialMappingKitImpl::Type::BackFace].SetDiffuseAlpha(fInAlpha);
	return *this;
}

MaterialMappingKit & H3DF::MaterialMappingKit::SetBackFaceTexture(CStringA strTextureName)
{
	auto pcImpl = static_cast<MaterialMappingKitImpl *>(m_pcImpl.get());
	pcImpl->m_cMaterial[(int)H3DF::MaterialMappingKitImpl::Type::BackFace].SetDiffuseTexture(strTextureName);
	return *this;
}

MaterialMappingKit & H3DF::MaterialMappingKit::SetBackFaceTextureOption(CStringA strTextureOption)
{
	auto pcImpl = static_cast<MaterialMappingKitImpl *>(m_pcImpl.get());
	pcImpl->m_cMaterial[(int)H3DF::MaterialMappingKitImpl::Type::BackFace].SetDiffuseTextureOption(strTextureOption);
	return *this;
}

MaterialMappingKit & H3DF::MaterialMappingKit::SetBackFaceGloss(float fInValue)
{
	auto pcImpl = static_cast<MaterialMappingKitImpl *>(m_pcImpl.get());
	pcImpl->m_cMaterial[(int)H3DF::MaterialMappingKitImpl::Type::BackFace].SetGloss(fInValue);
	return *this;
}

MaterialMappingKit & H3DF::MaterialMappingKit::SetBackFaceMaterial(MaterialKit const & cInMaterial)
{
	auto pcImpl = static_cast<MaterialMappingKitImpl *>(m_pcImpl.get());
	pcImpl->m_cMaterial[(int)H3DF::MaterialMappingKitImpl::Type::BackFace] = cInMaterial;
	return *this;
}

MaterialMappingKit & H3DF::MaterialMappingKit::SetFrontFaceColor(RGBAColor const & cInRgbaColor, Material::Color::Channel eInChannel)
{
	auto pcImpl = static_cast<MaterialMappingKitImpl *>(m_pcImpl.get());
	pcImpl->SetColor(cInRgbaColor, H3DF::MaterialMappingKitImpl::Type::FrontFace, eInChannel);
	return *this;
}

MaterialMappingKit & H3DF::MaterialMappingKit::SetFrontFaceAlpha(float fInAlpha)
{
	auto pcImpl = static_cast<MaterialMappingKitImpl *>(m_pcImpl.get());
	pcImpl->m_cMaterial[(int)H3DF::MaterialMappingKitImpl::Type::FrontFace].SetDiffuseAlpha(fInAlpha);
	return *this;
}

MaterialMappingKit & H3DF::MaterialMappingKit::SetFrontFaceTexture(CStringA strTextureName)
{
	auto pcImpl = static_cast<MaterialMappingKitImpl *>(m_pcImpl.get());
	pcImpl->m_cMaterial[(int)H3DF::MaterialMappingKitImpl::Type::FrontFace].SetDiffuseTexture(strTextureName);
	return *this;
}

MaterialMappingKit & H3DF::MaterialMappingKit::SetFrontFaceTextureOption(CStringA strTextureOption)
{
	auto pcImpl = static_cast<MaterialMappingKitImpl *>(m_pcImpl.get());
	pcImpl->m_cMaterial[(int)H3DF::MaterialMappingKitImpl::Type::FrontFace].SetDiffuseTextureOption(strTextureOption);
	return *this;
}

MaterialMappingKit & H3DF::MaterialMappingKit::SetFrontFaceGloss(float fInValue)
{
	auto pcImpl = static_cast<MaterialMappingKitImpl *>(m_pcImpl.get());
	pcImpl->m_cMaterial[(int)H3DF::MaterialMappingKitImpl::Type::FrontFace].SetGloss(fInValue);
	return *this;
}

MaterialMappingKit & H3DF::MaterialMappingKit::SetFrontFaceMaterial(MaterialKit const & cInMaterial)
{
	auto pcImpl = static_cast<MaterialMappingKitImpl *>(m_pcImpl.get());
	pcImpl->m_cMaterial[(int)H3DF::MaterialMappingKitImpl::Type::FrontFace] = cInMaterial;
	return *this;
}

MaterialMappingKit & H3DF::MaterialMappingKit::SetEdgeAlpha(float fInAlpha)
{
	auto pcImpl = static_cast<MaterialMappingKitImpl *>(m_pcImpl.get());
	pcImpl->m_cMaterial[(int)H3DF::MaterialMappingKitImpl::Type::Edge].SetDiffuseAlpha(fInAlpha);
	return *this;
}

MaterialMappingKit & H3DF::MaterialMappingKit::SetEdgeColor(RGBAColor const & cInRgbaColor, Material::Color::Channel eInChannel)
{
	auto pcImpl = static_cast<MaterialMappingKitImpl *>(m_pcImpl.get());
	pcImpl->SetColor(cInRgbaColor, H3DF::MaterialMappingKitImpl::Type::Edge, eInChannel);
	return *this;
}

MaterialMappingKit & H3DF::MaterialMappingKit::SetEdgeTexture(char const * strTextureName)
{
	auto pcImpl = static_cast<MaterialMappingKitImpl *>(m_pcImpl.get());
	pcImpl->m_cMaterial[(int)H3DF::MaterialMappingKitImpl::Type::Edge].SetDiffuseTexture(strTextureName);
	return *this;
}

MaterialMappingKit & H3DF::MaterialMappingKit::SetEdgeGloss(float fInValue)
{
	auto pcImpl = static_cast<MaterialMappingKitImpl *>(m_pcImpl.get());
	pcImpl->m_cMaterial[(int)H3DF::MaterialMappingKitImpl::Type::Edge].SetGloss(fInValue);
	return *this;
}

MaterialMappingKit & H3DF::MaterialMappingKit::SetEdgeMaterial(MaterialKit const & cInMaterial)
{
	auto pcImpl = static_cast<MaterialMappingKitImpl *>(m_pcImpl.get());
	pcImpl->m_cMaterial[(int)H3DF::MaterialMappingKitImpl::Type::Edge] = cInMaterial;
	return *this;
}

MaterialMappingKit & H3DF::MaterialMappingKit::SetFaceColor(RGBAColor const & cInRgbaColor, Material::Color::Channel eInChannel)
{
	auto pcImpl = static_cast<MaterialMappingKitImpl *>(m_pcImpl.get());
	pcImpl->SetColor(cInRgbaColor, H3DF::MaterialMappingKitImpl::Type::Face, eInChannel);
	return *this;
}

MaterialMappingKit & H3DF::MaterialMappingKit::SetFaceAlpha(float fInAlpha)
{
	auto pcImpl = static_cast<MaterialMappingKitImpl *>(m_pcImpl.get());
	pcImpl->m_cMaterial[(int)H3DF::MaterialMappingKitImpl::Type::Face].SetDiffuseAlpha(fInAlpha);
	return *this;
}

MaterialMappingKit & H3DF::MaterialMappingKit::SetFaceTexture(CStringA strTextureName)
{
	auto pcImpl = static_cast<MaterialMappingKitImpl *>(m_pcImpl.get());
	pcImpl->m_cMaterial[(int)H3DF::MaterialMappingKitImpl::Type::Face].SetDiffuseTexture(strTextureName);
	return *this;
}

MaterialMappingKit & H3DF::MaterialMappingKit::SetFaceTextureOption(CStringA strTextureOption)
{
	auto pcImpl = static_cast<MaterialMappingKitImpl *>(m_pcImpl.get());
	pcImpl->m_cMaterial[(int)H3DF::MaterialMappingKitImpl::Type::Face].SetDiffuseTextureOption(strTextureOption);
	return *this;
}

MaterialMappingKit & H3DF::MaterialMappingKit::SetFaceGloss(float fInValue)
{
	auto pcImpl = static_cast<MaterialMappingKitImpl *>(m_pcImpl.get());
	pcImpl->m_cMaterial[(int)H3DF::MaterialMappingKitImpl::Type::Face].SetGloss(fInValue);
	return *this;
}

MaterialMappingKit & H3DF::MaterialMappingKit::SetFaceMaterial(MaterialKit const & cInMaterial)
{
	auto pcImpl = static_cast<MaterialMappingKitImpl *>(m_pcImpl.get());
	pcImpl->m_cMaterial[(int)H3DF::MaterialMappingKitImpl::Type::Face] = cInMaterial;
	return *this;
}

MaterialMappingKit & H3DF::MaterialMappingKit::SetLineAlpha(float fInAlpha)
{
	auto pcImpl = static_cast<MaterialMappingKitImpl *>(m_pcImpl.get());
	pcImpl->m_cMaterial[(int)H3DF::MaterialMappingKitImpl::Type::Line].SetDiffuseAlpha(fInAlpha);
	return *this;
}

MaterialMappingKit & H3DF::MaterialMappingKit::SetLineColor(RGBAColor const & cInRgbaColor)
{
	auto pcImpl = static_cast<MaterialMappingKitImpl *>(m_pcImpl.get());
	pcImpl->m_cMaterial[(int)H3DF::MaterialMappingKitImpl::Type::Line].SetDiffuseColor(cInRgbaColor);
	return *this;
}

MaterialMappingKit & H3DF::MaterialMappingKit::SetMarkerColor(RGBAColor const & cInRgbaColor)
{
	auto pcImpl = static_cast<MaterialMappingKitImpl *>(m_pcImpl.get());
	pcImpl->m_cMaterial[(int)H3DF::MaterialMappingKitImpl::Type::Marker].SetDiffuseColor(cInRgbaColor);
	return *this;
}

MaterialMappingKit & H3DF::MaterialMappingKit::SetTextColor(RGBAColor const & cInRgbaColor)
{
	auto pcImpl = static_cast<MaterialMappingKitImpl *>(m_pcImpl.get());
	pcImpl->m_cMaterial[(int)H3DF::MaterialMappingKitImpl::Type::Text].SetDiffuseColor(cInRgbaColor);
	return *this;
}

MaterialMappingKit & H3DF::MaterialMappingKit::SetVertexAlpha(float fInAlpha)
{
	auto pcImpl = static_cast<MaterialMappingKitImpl *>(m_pcImpl.get());
	pcImpl->m_cMaterial[(int)H3DF::MaterialMappingKitImpl::Type::Vertex].SetDiffuseAlpha(fInAlpha);
	return *this;
}

MaterialMappingKit & H3DF::MaterialMappingKit::SetVertexColor(RGBAColor const & cInRgbaColor, Material::Color::Channel eInChannel)
{
	auto pcImpl = static_cast<MaterialMappingKitImpl *>(m_pcImpl.get());
	pcImpl->SetColor(cInRgbaColor, H3DF::MaterialMappingKitImpl::Type::Vertex, eInChannel);
	return *this;
}

MaterialMappingKit & H3DF::MaterialMappingKit::SetVertexTexture(CStringA strTextureName)
{
	auto pcImpl = static_cast<MaterialMappingKitImpl *>(m_pcImpl.get());
	pcImpl->m_cMaterial[(int)H3DF::MaterialMappingKitImpl::Type::Vertex].SetDiffuseTexture(strTextureName);
	return *this;
}

MaterialMappingKit & H3DF::MaterialMappingKit::SetVertexGloss(float fInValue)
{
	auto pcImpl = static_cast<MaterialMappingKitImpl *>(m_pcImpl.get());
	pcImpl->m_cMaterial[(int)H3DF::MaterialMappingKitImpl::Type::Vertex].SetGloss(fInValue);
	return *this;
}

MaterialMappingKit & H3DF::MaterialMappingKit::SetVertexMaterial(MaterialKit const & cInMaterial)
{
	auto pcImpl = static_cast<MaterialMappingKitImpl *>(m_pcImpl.get());
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
	auto pcImpl = static_cast<MaterialMappingKitImpl *>(m_pcImpl.get());
	auto pcInThatImpl = static_cast<const MaterialMappingKitImpl *>(cInThat.GetImpl());

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
	auto pcImpl = static_cast<MaterialMappingKitImpl *>(m_pcImpl.get());
	return pcImpl->m_cMaterial[(int)H3DF::MaterialMappingKitImpl::Type::AmbientLightUp].ShowDiffuseColor(cOutColor);

}

bool H3DF::MaterialMappingKit::ShowAmbientLightDownColor(Material::Type & cOutType, RGBAColor & cOutColor) const
{
	auto pcImpl = static_cast<MaterialMappingKitImpl *>(m_pcImpl.get());
	return pcImpl->m_cMaterial[(int)H3DF::MaterialMappingKitImpl::Type::AmbientLightDown].ShowDiffuseColor(cOutColor);
}

bool H3DF::MaterialMappingKit::ShowBackFaceChannel(Material::Channel eInChannel, Material::Type & cOutType, RGBAColor & cOutColor, CStringA & strOutTextureName, CStringA & strOutTextureOptions) const
{
	auto pcImpl = static_cast<MaterialMappingKitImpl *>(m_pcImpl.get());
	return pcImpl->ShowChannel(H3DF::MaterialMappingKitImpl::Type::BackFace, eInChannel, cOutType, cOutColor, strOutTextureName, strOutTextureOptions);
}

bool H3DF::MaterialMappingKit::ShowBackFaceMaterial(MaterialKit & cOutKit) const
{
	auto pcImpl = static_cast<MaterialMappingKitImpl *>(m_pcImpl.get());
	return pcImpl->ShowMaterial(H3DF::MaterialMappingKitImpl::Type::BackFace, cOutKit);
}

bool H3DF::MaterialMappingKit::ShowFrontFaceChannel(Material::Channel eInChannel, Material::Type & cOutType, RGBAColor & cOutColor, CStringA & strOutTextureName, CStringA & strOutTextureOptions) const
{
	auto pcImpl = static_cast<MaterialMappingKitImpl *>(m_pcImpl.get());
	return pcImpl->ShowChannel(H3DF::MaterialMappingKitImpl::Type::FrontFace, eInChannel, cOutType, cOutColor, strOutTextureName, strOutTextureOptions);
}

bool H3DF::MaterialMappingKit::ShowFrontFaceMaterial(MaterialKit & cOutKit) const
{
	auto pcImpl = static_cast<MaterialMappingKitImpl *>(m_pcImpl.get());
	return pcImpl->ShowMaterial(H3DF::MaterialMappingKitImpl::Type::FrontFace, cOutKit);
}

bool H3DF::MaterialMappingKit::ShowEdgeChannel(Material::Channel eInChannel, Material::Type & cOutType, RGBAColor & cOutColor, CStringA & strOutTextureName, CStringA & strOutTextureOptions) const
{
	auto pcImpl = static_cast<MaterialMappingKitImpl *>(m_pcImpl.get());
	return pcImpl->ShowChannel(H3DF::MaterialMappingKitImpl::Type::Edge, eInChannel, cOutType, cOutColor, strOutTextureName, strOutTextureOptions);
}

bool H3DF::MaterialMappingKit::ShowEdgeMaterial(MaterialKit & cOutKit) const
{
	auto pcImpl = static_cast<MaterialMappingKitImpl *>(m_pcImpl.get());
	return pcImpl->ShowMaterial(H3DF::MaterialMappingKitImpl::Type::Edge, cOutKit);
}

bool H3DF::MaterialMappingKit::ShowFaceChannel(Material::Channel eInChannel, Material::Type & cOutType, RGBAColor & cOutColor, CStringA & strOutTextureName, CStringA & strOutTextureOptions) const
{
	auto pcImpl = static_cast<MaterialMappingKitImpl *>(m_pcImpl.get());
	return pcImpl->ShowChannel(H3DF::MaterialMappingKitImpl::Type::Face, eInChannel, cOutType, cOutColor, strOutTextureName, strOutTextureOptions);
}

bool H3DF::MaterialMappingKit::ShowFaceMaterial(MaterialKit & cOutKit) const
{
	auto pcImpl = static_cast<MaterialMappingKitImpl *>(m_pcImpl.get());
	return pcImpl->ShowMaterial(H3DF::MaterialMappingKitImpl::Type::Face, cOutKit);
}

bool H3DF::MaterialMappingKit::ShowLineAlpha(float & fOutAlpha) const
{
	auto pcImpl = static_cast<MaterialMappingKitImpl *>(m_pcImpl.get());
	return pcImpl->m_cMaterial[(int)H3DF::MaterialMappingKitImpl::Type::Line].ShowDiffuseAlpha(fOutAlpha);
}

bool H3DF::MaterialMappingKit::ShowLineColor(RGBAColor & cOutColor) const
{
	auto pcImpl = static_cast<MaterialMappingKitImpl *>(m_pcImpl.get());
	return pcImpl->m_cMaterial[(int)H3DF::MaterialMappingKitImpl::Type::Line].ShowDiffuseColor(cOutColor);
}

bool H3DF::MaterialMappingKit::ShowMarkerColor(RGBAColor & cOutColor) const
{
	auto pcImpl = static_cast<MaterialMappingKitImpl *>(m_pcImpl.get());
	return pcImpl->m_cMaterial[(int)H3DF::MaterialMappingKitImpl::Type::Marker].ShowDiffuseColor(cOutColor);
}

bool H3DF::MaterialMappingKit::ShowTextColor(RGBAColor & cOutColor) const
{
	auto pcImpl = static_cast<MaterialMappingKitImpl *>(m_pcImpl.get());
	return pcImpl->m_cMaterial[(int)H3DF::MaterialMappingKitImpl::Type::Text].ShowDiffuseColor(cOutColor);
}

bool H3DF::MaterialMappingKit::ShowVertexChannel(Material::Channel eInChannel, Material::Type & cOutType, RGBAColor & cOutColor, CStringA & strOutTextureName, CStringA & strOutTextureOptions) const
{
	auto pcImpl = static_cast<MaterialMappingKitImpl *>(m_pcImpl.get());
	return pcImpl->ShowChannel(H3DF::MaterialMappingKitImpl::Type::Vertex, eInChannel, cOutType, cOutColor, strOutTextureName, strOutTextureOptions);
}

bool H3DF::MaterialMappingKit::ShowVertexMaterial(MaterialKit & cOutKit) const
{
	auto pcImpl = static_cast<MaterialMappingKitImpl *>(m_pcImpl.get());
	return pcImpl->ShowMaterial(H3DF::MaterialMappingKitImpl::Type::Vertex, cOutKit);
}

//== MaterialMappingControl ========================================================================
class MaterialMappingControlImpl : public ControlImpl
{
public:
	std::unique_ptr<Impl> Clone() const override {
		auto pcClone = std::make_unique<MaterialMappingControlImpl>();
		pcClone->Copy(this);
		return pcClone;
	}

	void Copy(const ControlImpl * pcInThat)
	{
		ControlImpl::Copy(pcInThat);
	}

	void SetFaceTexture(CStringA strInTextureName, Material::Texture::Channel eInChannel, size_t nInLayer);

	void SetAlpha(CStringA strGeometry, float fInAlpha);
	void SetColor(CStringA strGeometry, RGBAColor const & cInRgbaColor, Material::Color::Channel eInChannel);

protected:
	CStringA GetColorChannelString(Material::Color::Channel eInChannel);
	CStringA GetTextureChannelString(Material::Texture::Channel eInChannel);
};

CStringA MaterialMappingControlImpl::GetColorChannelString(Material::Color::Channel eInChannel)
{
	switch (eInChannel)
	{
		case H3DF::Material::Color::Channel::DiffuseColor:
			return "diffuse";

		case H3DF::Material::Color::Channel::Specular:
			return "specular";

		case H3DF::Material::Color::Channel::Emission:
			return "emission";

		case H3DF::Material::Color::Channel::Mirror:
			return "mirror";
	}

	return L"";
}

CStringA MaterialMappingControlImpl::GetTextureChannelString(Material::Texture::Channel eInChannel)
{
	switch (eInChannel)
	{
		case H3DF::Material::Texture::Channel::DiffuseTexture:
			return "diffuse";

		case H3DF::Material::Texture::Channel::Specular:
			return "specular";

		case H3DF::Material::Texture::Channel::Emission:
			return "emission";

		case H3DF::Material::Texture::Channel::Transmission:
			return "transmission";

		case H3DF::Material::Texture::Channel::Mirror:
			return "mirror";

		case H3DF::Material::Texture::Channel::Bump:
			return "bump";

		case H3DF::Material::Texture::Channel::EnvironmentTexture:
		case H3DF::Material::Texture::Channel::EnvironmentCubeMap:
			return "environment";

		default:
			DEBUG_STOP;
			return "";
			break;
	}
}

void MaterialMappingControlImpl::SetFaceTexture(CStringA strInTextureName, Material::Texture::Channel eInChannel, size_t nInLayer)
{
	CStringA strColorText;
	CStringA strColorChannel = GetTextureChannelString(eInChannel);

	if (H3DF::Material::Texture::Channel::EnvironmentTexture == eInChannel) {
		strColorText.Format("faces = (%s = %s)", strColorChannel, strInTextureName);
	}
	else {
		DEBUG_STOP;
		return;
	}

	SegmentKeyImpl::LocalOpen(m_cOverrideKey); {
		HC_Set_Color(strColorText);
	} SegmentKeyImpl::LocalClose(m_cOverrideKey);
}

void MaterialMappingControlImpl::SetAlpha(CStringA strGeometry, float fInAlpha)
{
	CStringA strColorText;
	strColorText.Format("%s = (transmission = r=%f g=%f b=%f)", strGeometry, fInAlpha, fInAlpha, fInAlpha);

	SegmentKeyImpl::LocalOpen(m_cOverrideKey); {
		HC_Set_Color(strColorText);
	} SegmentKeyImpl::LocalClose(m_cOverrideKey);
}

void MaterialMappingControlImpl::SetColor(CStringA strGeometry, RGBAColor const & cInRgbaColor, Material::Color::Channel eInChannel)
{
	CStringA strColorText;
	CStringA strColorChannel = GetColorChannelString(eInChannel);

	if (1.0f > cInRgbaColor.alpha) {
		strColorText.Format("%s = (%s = (r=%f g=%f b=%f), (transmission = r=%f g=%f b=%f))", strGeometry, strColorChannel, 
			cInRgbaColor.red, cInRgbaColor.green, cInRgbaColor.blue,
			cInRgbaColor.alpha, cInRgbaColor.alpha, cInRgbaColor.alpha);
	}
	else {
		strColorText.Format("%s = (%s = (r=%f g=%f b=%f))", strGeometry, strColorChannel, cInRgbaColor.red, cInRgbaColor.green, cInRgbaColor.blue);
	}
	
	SegmentKeyImpl::LocalOpen(m_cOverrideKey); {
		HC_Set_Color(strColorText);
	} SegmentKeyImpl::LocalClose(m_cOverrideKey);
}

H3DF::MaterialMappingControl::MaterialMappingControl(SegmentKey const & cInThat)
{
	if (staticType != Type()) {
		return;
	}

	m_pcImpl = std::make_unique<MaterialMappingControlImpl>();
	auto pcImpl = dynamic_cast<MaterialMappingControlImpl *>(m_pcImpl.get());

	pcImpl->m_cOverrideKey = cInThat;
}

H3DF::MaterialMappingControl::MaterialMappingControl(MaterialMappingControl const & cInThat)
{
	if (staticType != Type()) {
		return;
	}

	m_pcImpl = (nullptr == cInThat.m_pcImpl) ? cInThat.m_pcImpl->Clone() : nullptr;
}

MaterialMappingControl & H3DF::MaterialMappingControl::operator = (MaterialMappingControl const & cInThat)
{
	if (nullptr != cInThat.m_pcImpl) {
		m_pcImpl = cInThat.m_pcImpl->Clone();
	}
	else {
		m_pcImpl.reset();
	}

	return *this;
}

MaterialMappingControl & H3DF::MaterialMappingControl::SetFaceColor(RGBAColor const & cInRgbaColor, Material::Color::Channel eInChannel)
{
	auto pcImpl = static_cast<MaterialMappingControlImpl *>(m_pcImpl.get());
	DEBUG_VALID(pcImpl);

	pcImpl->SetColor("faces", cInRgbaColor, eInChannel);

	return *this;
}

MaterialMappingControl & H3DF::MaterialMappingControl::SetFaceAlpha(float fInAlpha)
{
	auto pcImpl = static_cast<MaterialMappingControlImpl *>(m_pcImpl.get());
	DEBUG_VALID(pcImpl);

	pcImpl->SetAlpha("faces", fInAlpha);

	return *this;
}

MaterialMappingControl & H3DF::MaterialMappingControl::SetFaceTexture(CStringA strInTextureName, Material::Texture::Channel eInChannel, size_t nInLayer)
{
	auto pcImpl = static_cast<MaterialMappingControlImpl *>(m_pcImpl.get());
	DEBUG_VALID(pcImpl);

	pcImpl->SetFaceTexture(strInTextureName, eInChannel, nInLayer);

	return *this;
}

MaterialMappingControl & H3DF::MaterialMappingControl::SetBackFaceColor(RGBAColor const & cInRgbaColor, Material::Color::Channel eInChannel)
{
	auto pcImpl = static_cast<MaterialMappingControlImpl *>(m_pcImpl.get());
	DEBUG_VALID(pcImpl);

	pcImpl->SetColor("back", cInRgbaColor, eInChannel);

	return *this;
}

MaterialMappingControl & H3DF::MaterialMappingControl::SetBackFaceAlpha(float fInAlpha)
{
	auto pcImpl = static_cast<MaterialMappingControlImpl *>(m_pcImpl.get());
	DEBUG_VALID(pcImpl);

	pcImpl->SetAlpha("back", fInAlpha);

	return *this;
}

MaterialMappingControl & H3DF::MaterialMappingControl::SetFrontFaceColor(RGBAColor const & cInRgbaColor, Material::Color::Channel eInChannel)
{
	auto pcImpl = static_cast<MaterialMappingControlImpl *>(m_pcImpl.get());
	DEBUG_VALID(pcImpl);

	pcImpl->SetColor("front", cInRgbaColor, eInChannel);

	return *this;
}

MaterialMappingControl & H3DF::MaterialMappingControl::SetFrontFaceAlpha(float fInAlpha)
{ 
	auto pcImpl = static_cast<MaterialMappingControlImpl *>(m_pcImpl.get());
	DEBUG_VALID(pcImpl);

	pcImpl->SetAlpha("front", fInAlpha);

	return *this;
}

MaterialMappingControl & H3DF::MaterialMappingControl::SetEdgeColor(RGBAColor const & cInRgbaColor, Material::Color::Channel eInChannel)
{
	auto pcImpl = static_cast<MaterialMappingControlImpl *>(m_pcImpl.get());
	if (nullptr == pcImpl) { assert(false); }

	pcImpl->SetColor("edges", cInRgbaColor, eInChannel);

	return *this;
}

MaterialMappingControl & H3DF::MaterialMappingControl::SetMarkerColor(RGBAColor const & cInRgbaColor, Material::Color::Channel eInChannel)
{
	auto pcImpl = static_cast<MaterialMappingControlImpl *>(m_pcImpl.get());
	if (nullptr == pcImpl) { assert(false); }

	pcImpl->SetColor("markers", cInRgbaColor, eInChannel);

	return *this;
}

MaterialMappingControl & H3DF::MaterialMappingControl::UnSetColor(CStringA strInType)
{
	auto pcImpl = static_cast<MaterialMappingControlImpl *>(m_pcImpl.get());
	if (nullptr == pcImpl) { assert(false); }

	SegmentKeyImpl::LocalOpen(pcImpl->m_cOverrideKey); {
		HC_UnSet_One_Selectability(strInType);
	}SegmentKeyImpl::LocalClose(pcImpl->m_cOverrideKey);

	return *this;
}

void H3DF::MaterialMappingControl::InitPopulateTextures()
{
	auto pcImpl = static_cast<MaterialMappingControlImpl *>(m_pcImpl.get());
	if (nullptr == pcImpl) { assert(false); }

	InitializeMagick(".");

	SegmentKeyImpl::LocalOpen(pcImpl->m_cOverrideKey); {
		HC_Set_Visibility("image = off");
	} SegmentKeyImpl::LocalClose(pcImpl->m_cOverrideKey);
}

void H3DF::MaterialMappingControl::InsertPicture(UINT nIndex, UINT nPixelWidth, UINT nPixelHeight, UCHAR * pucBinaryData)
{
	auto pcImpl = static_cast<MaterialMappingControlImpl *>(m_pcImpl.get());
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

		auto pcImpl = static_cast<MaterialMappingControlImpl *>(m_pcImpl.get());
		if (nullptr == pcImpl) { assert(false); }

		SegmentKeyImpl::LocalOpen(pcImpl->m_cOverrideKey); {

			//HC_KEY nKey = HC_Insert_Image(0.0, 0.0, 0.0, H_FORMAT_TEXT("rgba, name = image %u, local = on", nIndex), width, height, &anPixels[0]);
			HC_KEY nKey = HC_Insert_Image(0.0, 0.0, 0.0, H_FORMAT_TEXT("rgba, name = image %u", nIndex), width, height, &anPixels[0]);

		} SegmentKeyImpl::LocalClose(pcImpl->m_cOverrideKey);
	}
}

void H3DF::MaterialMappingControl::SetTextureMatrix(float * pfTextureMatrix, char * pchTextureTransformSegment)
{
	auto pcImpl = static_cast<MaterialMappingControlImpl *>(m_pcImpl.get());
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

void H3DF::MaterialMappingControl::SetDefineLocalTexture(UINT nIndex, CStringA strTextureOptions)
{
	auto pcImpl = static_cast<MaterialMappingControlImpl *>(m_pcImpl.get());
	if (nullptr == pcImpl) { assert(false); }

	CStringA strText;
	strText.Format("texture_%u", nIndex);

	SegmentKeyImpl::LocalOpen(pcImpl->m_cOverrideKey); {
		HC_Define_Local_Texture(strText, strTextureOptions);
	} SegmentKeyImpl::LocalClose(pcImpl->m_cOverrideKey);
}

void H3DF::MaterialMappingControl::EndPopulateTextures()
{
	DestroyMagick();
}
