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
	MaterialMappingKit & SetColor(RGBAColor const & cInRgbaColor, Material::Color::Type eType = Material::Color::Type::Diffuse);
	MaterialMappingKit & SetGloss(float fGloss);
	bool ShowColor(Material::Color::Type eType, RGBAColor & cOutRgbaColor) const;

	//----- Operator -----
	MaterialMappingKit & operator = (MaterialMappingKit const & cInThat);
	bool operator == (MaterialMappingKit const & cInThat) const;
	bool operator != (MaterialMappingKit const & cInThat) const;

	bool IsAllocate();

	RGBAColor Colors(int nType) const;
	bool SetColorFlags(int nType) const;
	float Gloss() const;
	bool GlossFlag() const;

private:
	RGBAColor m_pcColors[(int) Material::Color::Type::Count];
	bool m_pbSetColorFlags[(int) Material::Color::Type::Count];
	float m_fGloss;
	bool m_bGlossFlag;
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
