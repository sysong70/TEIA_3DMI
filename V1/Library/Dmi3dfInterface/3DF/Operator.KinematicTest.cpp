#include "StdAfx.h"

#include "Operator.KinematicTest.h"

#include "Segment.h"

#include <HUtilityGeometryCreation.h>

USING_3DF_NAMESPACE

bool H3DF::Operator::KinematicTest::CreateTestModel(SegmentKey cModelKey)
{
	MaterialMappingKit cMaterialMappingKit;
	cMaterialMappingKit.SetFaceColor(RGBColor(1, 0, 0));

	SegmentKey cBaseKey = cModelKey.Subsegment(L"Base");
	cBaseKey.SetMaterialMapping(cMaterialMappingKit);

	Point cCenter;
	Vector cXAxis(1, 0, 0);
	Vector cZAxis(0, 0, 1);

	CreateCylinder(cBaseKey, cCenter, 150, 100, cXAxis, cZAxis);

	SegmentKey cArm1Key = cBaseKey.Subsegment(L"Arm1");

	cMaterialMappingKit.SetFaceColor(RGBColor(0, 1, 0));
	cArm1Key.SetMaterialMapping(cMaterialMappingKit);

	cCenter.Set(0, 0, 150);
	CreateCylinder(cArm1Key, cCenter, 30, 500, cXAxis, cZAxis);
	SegmentKey cArm2Key = cArm1Key.Subsegment(L"Arm2");

	cMaterialMappingKit.SetFaceColor(RGBColor(0, 0, 1));
	cArm2Key.SetMaterialMapping(cMaterialMappingKit);

	cCenter.Set(0, 0, 700);
	cXAxis.Set(0, 0, 1);
	cZAxis.Set(1, 0, 0);

	CreateCylinder(cArm2Key, cCenter, 30, 400, cXAxis, cZAxis);

	MatrixKit cMatrix;
	// 원점으로 이동
	cMatrix.Translate(0, 0, -150);
	cMatrix.RotateOffAxis(Vector(0, 1, 0), 30);
	cMatrix.Translate(0, 0, 150);

	cArm1Key.SetModellingMatrix(cMatrix);

	return true;
}

bool H3DF::Operator::KinematicTest::CreateCylinder(SegmentKey cKey, Point cCenter, float fRadius, float fHeight, Vector cXAxis, Vector cZAxis, int nSidesCount)
{
	cKey.Open();

	HPoint cHCenter(cCenter.x, cCenter.y, cCenter.z);
	HVector cHXAxis(cXAxis.x, cXAxis.y, cXAxis.z);
	HVector cHZAxis(cZAxis.x, cZAxis.y, cZAxis.z);
	
	HUtilityGeometryCreation::CreateCylinder(cHCenter, fRadius, fHeight, nSidesCount, cHXAxis, cHZAxis);

	cKey.Close();

	return true;
}
