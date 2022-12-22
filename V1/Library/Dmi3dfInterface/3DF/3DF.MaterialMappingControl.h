#pragma once

#include "3DF.h"

#include "3DF.Color.h"
#include "3DF.Material.h"

OPEN_3DF_NAMESPACE

class API_3DF MaterialMappingKit
{
public:
	MaterialMappingKit();

	//----- Face Color 설정 -----
	MaterialMappingKit & SetFaceColor(RGBAColor const & cInRgbaColor, Material::Color::Channel eInChannel = Material::Color::Channel::DiffuseColor);
	MaterialMappingKit & SetFaceGloss(float fGloss);
	bool ShowFaceChannel(Material::Channel eInChannel, Material::Type & eOutType, RGBAColor & cOutRgbaColor, CString & strOutTextureName, float & fOutValue) const;

	//----- Line Color 설정 -----
	MaterialMappingKit & SetLineColor(RGBAColor const & cInRgbaColor, Material::Color::Channel eInChannel = Material::Color::Channel::DiffuseColor);
	bool ShowLineChannel(Material::Channel eInChannel, Material::Type & eOutType, RGBAColor & cOutRgbaColor, CString & strOutTextureName, float & fOutValue) const;

	//----- Operator -----
	MaterialMappingKit & operator = (MaterialMappingKit const & cInThat);
	bool operator == (MaterialMappingKit const & cInThat) const;

	bool IsAllocate();

	RGBAColor Colors(int nTypeIndex, int nInChannel) const;
	bool SetColors(int nTypeIndex, int nInChannel) const;
	float * Gloss() const;
	bool * GlossFlag() const;

private:
	enum class TypeIndex
	{
		Face,
		Line,
		Vertex,
		Count
	};

	RGBAColor m_pcColors[(int) TypeIndex::Count][(int) Material::Channel::Count];
	bool m_pbSetColors[(int) TypeIndex::Count][(int) Material::Channel::Count];
	Material::Type m_eColorType[(int) TypeIndex::Count];
	float m_fGloss[(int) TypeIndex::Count];
	bool m_bGlossFlag[(int) TypeIndex::Count];

 	MaterialMappingKit & SetColor(TypeIndex nTypeIndex, Material::Channel eInChannel, RGBAColor const & cInRgbaColor);
	MaterialMappingKit & SetGloss(TypeIndex nTypeIndex, float fGloss);
	
	MaterialMappingKit & UnsetColor(TypeIndex nTypeIndex, Material::Channel eInChannel);
	MaterialMappingKit & UnsetGloss(TypeIndex nTypeIndex);

	bool ShowChannel(TypeIndex nTypeIndex, Material::Channel eInChannel, Material::Type & eOutType, RGBAColor & cOutRgbaColor, CString & strOutTextureName, float & fOutValue) const;
};

class MaterialMappingControl
{
public:
	MaterialMappingControl(SegmentKey & cInSegmentKey);

	//== Selectability 설정 =========================================================================
	MaterialMappingControl & SetWindows(bool bInValue);
	MaterialMappingControl & SetEdges(bool bInValue);
	MaterialMappingControl & SetFaces(bool bInValue);
	MaterialMappingControl & SetLights(bool bInValue);
	MaterialMappingControl & SetLines(bool bInValue);
	MaterialMappingControl & SetMarkers(bool bInValue);
	MaterialMappingControl & SetVertices(bool bInValue);
	MaterialMappingControl & SetText(bool bInValue);
	MaterialMappingControl & SetGeometry(bool bInValue);
	MaterialMappingControl & SetEverything(bool bInValue);

	MaterialMappingControl & UnsetWindows();
	MaterialMappingControl & UnsetEdges();
	MaterialMappingControl & UnsetFaces();
	MaterialMappingControl & UnsetLights();
	MaterialMappingControl & UnsetLines();
	MaterialMappingControl & UnsetMarkers();
	MaterialMappingControl & UnsetVertices();
	MaterialMappingControl & UnsetText();
	MaterialMappingControl & UnsetGeometry();
	MaterialMappingControl & UnsetEverything();

private:
	SegmentKey & m_cInSegmentKey;
	
	MaterialMappingControl & SetSelectability(CString strInType, bool bInValue);
	MaterialMappingControl & UnSetSelectability(CString strInType);
};

CLOSE_3DF_NAMESPACE
