#pragma once

#include "ConvObject.h"

#include "curve.h"
#include "surface.h"

class MbModel;

class ConvSurface : public ConvObject
{
public:
	ConvSurface(const A3DSurfBase * pcSurfBase, double dContexScale);
	~ConvSurface();

	bool ConvertSurface(double dContextScale);
	bool ConvertSurface(double dParentScale, double dContextScale, c3d::SurfaceSPtr & pcSurface);

	A3DSurfBase * GetSurfBase() const { return (A3DSurfBase *) m_pcSurfBase; }
	DWORD_PTR * GetSurfBaseData() const { return m_cEntityInfo.pcEntityData[0]; }
	A3DEEntityType GetSurfaceType() const { return m_cEntityInfo.peEntityType[0]; }

	c3d::SurfaceSPtr & GetSurface() { return m_pcSurface; }

private:
	const A3DSurfBase * m_pcSurfBase = nullptr;
	c3d::SurfaceSPtr m_pcSurface;
	ConvEntityInfo m_cEntityInfo;
};

// == Surface 변환 함수 =============================================================================

namespace ConvSurfaceBase
{
	bool GetSurfBaseData(const A3DSurfBase * pcSurfBase, double dContextScale, ConvEntityInfo & cEntityInfo);
	bool ClearSurfBaseData(ConvEntityInfo & cEntityInfo);

	bool ConvertSurfBase(const A3DSurfBase * pcSurfBase, double dParentScale, double dContextScale, c3d::SurfaceSPtr & pcSurface);
	bool ConvertSurfBaseData(ConvEntityInfo & cEntityInfo, double dParentScale, double dContextScale, c3d::SurfaceSPtr & pcSurface);

	bool ConvertSurfPlane(const A3DSurfBase * pcSurfBase, double dParentScale, bool & bSwapUV, c3d::SurfaceSPtr & pcSurface);

	bool ConvertSurfCylinder(const A3DSurfBase * pcSurfBase, double dParentScale, bool & bSwapUV, c3d::SurfaceSPtr & pcSurface);
	bool ConvertSurfCylinderData(ConvEntityInfo & cEntityInfo, double dParentScale, bool bSwapUV, c3d::SurfaceSPtr & pcSurface);

	bool ConvertSurfCone(const A3DSurfBase * pcSurfBase, double dParentScale, bool & bSwapUV, c3d::SurfaceSPtr & pcSurface);
	bool ConvertSurfConeData(ConvEntityInfo & cEntityInfo, double dParentScale, c3d::SurfaceSPtr & pcSurface);

	bool ConvertSurfSphere(const A3DSurfBase * pcSurfBase, double dParentScale, bool & bSwapUV, c3d::SurfaceSPtr & pcSurface);
	bool ConvertSurfSphereData(ConvEntityInfo & cEntityInfo, double dParentScale, c3d::SurfaceSPtr & pcSurface);

	bool ConvertSurfTorus(const A3DSurfBase * pcSurfBase, double dParentScale, bool & bSwapUV, c3d::SurfaceSPtr & pcSurface);
	bool ConvertSurfTorusData(ConvEntityInfo & cEntityInfo, double dParentScale, c3d::SurfaceSPtr & pcSurface);

	bool ConvertSurfNurbs(const A3DSurfBase * pcSurfBase, double dParentScale, c3d::SurfaceSPtr & pcSurface);
	bool ConvertSurfNurbsData(ConvEntityInfo & cEntityInfo, double dParentScale, c3d::SurfaceSPtr & pcSurface);

	bool ConvertSurfExtrusion(const A3DSurfBase * pcSurfBase, double dParentScale, double dContextScale, bool & bSwapUV, c3d::SurfaceSPtr & pcSurface);
	bool ConvertSurfExtrusionData(ConvEntityInfo & cEntityInfo, double dParentScale, double dContextScale, c3d::SurfaceSPtr & pcSurface);

	bool ConvertSurfOffset(const A3DSurfBase * pcSurfBase, double dParentScale, double dContextScale, bool & bSwapUV, c3d::SurfaceSPtr & pcSurface);
	bool ConvertSurfOffsetData(ConvEntityInfo & cEntityInfo, double dParentScale, double dContextScale, c3d::SurfaceSPtr & pcSurface);

	bool ConvertSurfBlend03(const A3DSurfBase * pcSurfBase, double dContextScale, bool & bSwapUV, c3d::SurfaceSPtr & pcSurface);
	bool ConvertSurfBlend03Data(ConvEntityInfo & cEntityInfo, double dContextScale, MbSurface *& pcSurface, MbModel * pcModel);

	bool ConvertSurfRuled(const A3DSurfBase * pcSurfBase, double dParentScale, double dContextScale, bool & bSwapUV, c3d::SurfaceSPtr & pcSurface);
	bool ConvertSurfRuledData(ConvEntityInfo & cEntityInfo, double dParentScale, double dContextScale, c3d::SurfaceSPtr & pcSurface);

	bool ConvertSurfRevolution(const A3DSurfBase * pcSurfBase, double dParentScale, double dContextScale, bool & bSwapUV, c3d::SurfaceSPtr & pcSurface);
	bool ConvertSurfRevolutionData(ConvEntityInfo & cEntityInfo, double dParentScale, double dContextScale, c3d::SurfaceSPtr & pcSurface);

	bool CreateBoundedSurface(MbSurface *& pcSurface, c3d::PlaneCurveSPtr & pcPlaneCurve, MbSurface * cBoundedSurface);;

	// == 3. Utitliy Function ======================================================================
	bool GetTargetEntityIndex(ConvEntityInfo & cEntityInfo, int & nTargetIndex);
};
