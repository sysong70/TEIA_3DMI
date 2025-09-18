#include "StdAfx.h"

#include "Am.DatalConverter.Impl.h"

#include <Common_Define.h>

#include "../../Dmi3dx.h"

#include "../../3DX.DataGuard.h"

#include <cassert>
#include <iomanip>
#include <algorithm>
#include <cmath>

using namespace H3DX;
using namespace H3DF;
using namespace AM;

#define EXCHAGE_SCALE 1000

//== AM Datal Writer class =========================================================================

struct AM::DatalWriter::Impl 
{
	std::ostream * os { nullptr };
	bool inEquipment { false };
	std::string currentEquipmentName;
};

AM::DatalWriter::DatalWriter() : m_impl(std::make_unique<Impl>())
{

}

AM::DatalWriter::~DatalWriter()
{

}

Result AM::DatalWriter::Bind(std::ostream * os) noexcept 
{
	if (nullptr == os) {
		return Result::Fail(Error::InvalidArgument, "DatalWriter::Bind: os is null");
	}

	m_impl->os = os;

	return Result::Ok();
}


bool AM::DatalWriter::IsOpen() const noexcept
{
	return (nullptr != m_impl->os);
}

Result AM::DatalWriter::BeginEquipment(std::string_view equipmentName) noexcept
{
	if (nullptr == m_impl->os) {
		return Result::Fail(Error::NotInitialized, "No bound ostream.");
	}

	// 사전에 입력된 Equipment가 있는지 확인
	if (false == m_impl->inEquipment) {
		return Result::Fail(Error::StateError, "Already inside equipment.");
	}

	(*m_impl->os) << "NEW EQUIPMENT " << equipmentName << "\n";
	m_impl->inEquipment = true;

	m_impl->currentEquipmentName.assign(equipmentName);

	return Result::Ok();
}

Result AM::DatalWriter::EndEquipment() noexcept 
{
	if (nullptr == m_impl->os) {
		return Result::Fail(Error::NotInitialized, "No bound ostream.");
	}

	if (false == m_impl->inEquipment) {
		return Result::Fail(Error::StateError, "No active equipment.");
	}

	// 관행상 EQUIPMENT 블록 종료 마커가 별도 없으면 본문 종료 시그널만 남김
	(*m_impl->os) << "END\n"; // 필요 없으면 주석 처리 가능

	m_impl->inEquipment = false;
	m_impl->currentEquipmentName.clear();

	return Result::Ok();
}


AM::DatalConverterImpl::DatalConverterImpl()
{

}

bool AM::DatalConverterImpl::ExportTopoConnex(const A3DTopoConnex * connex) noexcept
{
	if(nullptr == connex) {
		return false;
	}

	H3DX::TopoConnexDataGuard dataGuard(connex);
	if (!dataGuard.IsValid()) {
		return false;
	}

	m_writer.Open("Z:\\AmDataData.mac", false);

	m_writer.StartWithTemplate("/SSD-TEST", "SEPARATED OIL TANK HEATER-2661", "EQUI", 310.0, false, "unset", "unset", "unset");

	const auto & connexData = dataGuard.Data();

	for (A3DUns32 i = 0; i < connexData.m_uiShellSize; ++i) {
		ExportTopoShell(connexData.m_ppShells[i]);
	}

	// Close TMPLATE explicitly at the very end
	m_writer.End();

	// Footer lines as requested
	m_writer.EndInput(); // -> "INPUT END EQUIPMENT /SSD-TEST"
	m_writer.FinishInput(); // -> "INPUT FINISH"

	return true;
}

bool AM::DatalConverterImpl::ExportTopoShell(const A3DTopoShell * shell) noexcept
{
	if (nullptr == shell) {
		return false;
	}

	H3DX::TopoShellDataGuard dataGuard(shell);
	if (!dataGuard.IsValid()) {
		return false;
	}

	// 1. 솔리드 원기둥 판별 (허용공차는 모델 스케일에 맞게 조정)
	constexpr double kLenTol = 1e-4;   // 길이/거리 허용오차
	constexpr double kAngTol = 1e-5;   // 라디안(≈0.00057°)

	// #AM_DATAL: Cylinder detect
	if (auto solid = DetectSolidCylinder(shell, kLenTol, kAngTol)) {
		if (true == solid->isSolidCylinder) {
			const auto & cylinder = solid->cyl;

			// 축/프레임/반지름/높이
			H3DF::Vector axis = cylinder.Axis();  // Z = X×Y
			double radius = cylinder.radius;
			double height = solid->height;

			axis.x = 0.5;
			axis.y = 0.5;
			axis.z = 0.5;

			m_writer.WriteSolidCylinder(cylinder.origin.x, cylinder.origin.y, cylinder.origin.z,
				axis.x, axis.y, axis.z, radius, height);

			return true;
		}
	}

	// 2. Hole(내경) 케이스 처리:
	//    - 실린더 옆면(face)와 평면(face)을 수집
	//    - 실린더에 대해 캡 역할을 하는 평면 2개 찾기
	//    - 요청 포맷으로 순서 출력:
	//        END
	//        NEW CYLINDER ... (ORI 없음, DIAM/HEIG만)
	//        NEW NCYLINDER ... (ORI + PRODHT 'Hole')
	//        END
	//    - 마지막 템플릿 END는 상위에서 m_writer.End()가 수행
	const auto & shellData = dataGuard.Data();

	std::vector<FaceSummary> planes;
	std::vector<FaceSummary> cylinders;
	planes.reserve(shellData.m_uiFaceSize);
	cylinders.reserve(shellData.m_uiFaceSize);

	for (A3DUns32 nIndex = 0; nIndex < shellData.m_uiFaceSize; ++nIndex) {
		const A3DTopoFace * face = shellData.m_ppFaces[nIndex];

		FaceSummary fs{};
		if (!AnalyzeFaceSurface(face, fs, kLenTol, kAngTol)) {
			continue;
		}

		switch (fs.kind) {
			case SurfKind::Plane:
			case SurfKind::NurbsPlane:
				planes.push_back(std::move(fs));
				break;

			case SurfKind::Cylinder:
				cylinders.push_back(std::move(fs));
				break;

			default:
				break;
		}
	}

	// 실린더가 없거나, 평면이 2개 미만이면 hole 판단 불가
	if (cylinders.empty() || planes.size() < 2) {
		// 그래도 페이스 내보내기 루프는 실행
		for (A3DUns32 i = 0; i < shellData.m_uiFaceSize; ++i) {
			ExportTopoFace(shellData.m_ppFaces[i]);
		}
		return true;
	}

	// 각 실린더에 대해 캡 2개를 찾아 높이 산출
	bool wroteHole = false;

	for (const auto & cf : cylinders) {
		const CylinderParam & cy = cf.cyl;

		// 캡 후보 모으기 (필요 시 원 추출 재시도)
		std::vector<const FaceSummary *> caps;
		caps.reserve(4);

		for (auto & pf : planes) {
			if (!pf.outerHasCircle) {
				CircleParam tmp{};
				if (ExtractOuterCircle(pf.face, tmp)) {
					pf.outerHasCircle = true;
					pf.outerCircle = tmp;
				}
			}
			if (!pf.outerHasCircle) {
				continue;
			}
			if (IsCapForCylinder(pf.outerCircle, cy, static_cast<Scalar>(kLenTol), static_cast<Scalar>(kAngTol))) {
				caps.push_back(&pf);
			}
		}

		if (caps.size() < 2) {
			continue;
		}

		// 축 방향으로 가장 멀리 떨어진 두 캡 선택 → 높이
		const H3DF::DVector axis = cy.Axis();
		auto projH = [&](const FaceSummary * a, const FaceSummary * b) -> double {
			H3DF::DVector ca(a->outerCircle.origin.x, a->outerCircle.origin.y, a->outerCircle.origin.z);
			H3DF::DVector cb(b->outerCircle.origin.x, b->outerCircle.origin.y, b->outerCircle.origin.z);
			return std::abs(static_cast<double>((cb - ca).Dot(axis)));
			};

		const FaceSummary * cap0 = nullptr;
		const FaceSummary * cap1 = nullptr;
		double bestH = 0.0;

		for (size_t i = 0; i < caps.size(); ++i) {
			for (size_t j = i + 1; j < caps.size(); ++j) {
				const double h = projH(caps[i], caps[j]);
				if (h > bestH && h > kLenTol) {
					bestH = h;
					cap0 = caps[i];
					cap1 = caps[j];
				}
			}
		}
		if (!cap0 || !cap1) {
			continue;
		}

		// 요청된 출력 순서로 내보내기
		// 1. 현재 열려 있는 primitive가 있다면 닫기 → "END"
		//   ※ 템플릿을 닫지 않도록, 일반적으로 바로 직전 primitive를 닫는 타이밍에 호출해야 함
		//(void) m_writer.End();

		// 2. 바깥 실린더 (ORI 없음)
		(void) m_writer.WritePlainCylinder(cy.origin.x,
			cy.origin.y,
			cy.origin.z,
			cy.radius * 2.0,      // DIAM
			bestH);               // HEIG

		// 3. 구멍 실린더 (ORI + PRODHT 'Hole')
		const H3DF::Vector axisVec = cy.Axis();
		(void) m_writer.WriteHoleCylinder(cy.origin.x, cy.origin.y, cy.origin.z,
			axisVec.x, axisVec.y, axisVec.z, cy.radius, bestH, 1.0);

		// 지금 열려있는 NEW CYLINDER만 닫기 → 첫 번째 END
		(void) m_writer.End();

		wroteHole = true;
		// 동일 쉘 내 여러 개 hole도 지원하려면 continue; (여기선 계속 검사)
		continue;
	}

	// 나머지 페이스들 처리 (원하면 생략 가능)
	for (A3DUns32 i = 0; i < shellData.m_uiFaceSize; ++i) {
		ExportTopoFace(shellData.m_ppFaces[i]);
	}

	return wroteHole || true;

// 	const auto & shellData = dataGuard.Data();
// 	for (A3DUns32 i = 0; i < shellData.m_uiFaceSize; ++i) {
// 		ExportTopoFace(shellData.m_ppFaces[i]);
// 	}

	return true;
}

bool AM::DatalConverterImpl::ExportTopoFace(const A3DTopoFace * face) noexcept
{
	if (nullptr == face) {
		return false;
	}

	H3DX::TopoFaceDataGuard dataGuard(face);
	if (!dataGuard.IsValid()) {
		return false;
	}

	const auto & faceData = dataGuard.Data();
	// faceData.m_pSurface; // A3DSurfBase* - Surface 정보
	// faceData.m_uiLoopSize; // 루프 수

	if (nullptr == faceData.m_pSurface) {
		DEBUG_STOP;
		return false;
	}

	// Surface Type check
	A3DEEntityType eType = kA3DTypeUnknown;
	if (A3D_SUCCESS != A3DEntityGetType(faceData.m_pSurface, &eType)) {
		return false;
	}

	switch (eType)
	{
		case kA3DTypeSurfPlane:

			break;

		case kA3DTypeSurfCylinder:

			break;
	}

// 	for (A3DUns32 i = 0; i < faceData.m_uiLoopSize; ++i) {
// 		ExportTopoLoop(faceData.m_ppLoops[i]);
// 	}

	return true;
}

bool AM::DatalConverterImpl::AnalyzeFaceSurface(
	const A3DTopoFace * face,
	FaceSummary & out,
	double /*lenTol*/,
	double /*angTol*/) noexcept
{
	out = FaceSummary{};
	out.face = face;

	if (nullptr == face) {
		DEBUG_STOP;
		return false;
	}

	TopoFaceDataGuard faceDataGuard(face);
	if (false == faceDataGuard.IsValid()) {
		DEBUG_STOP;
		return false;
	}
	const auto & faceData = faceDataGuard.Data();

	out.loopCount = faceData.m_uiLoopSize;
	out.hasInnerLoops = (faceData.m_uiLoopSize > 1);

	if (nullptr == faceData.m_pSurface) {
		DEBUG_STOP;
		return false;
	}

	A3DEEntityType type = kA3DTypeUnknown;
	if (A3D_SUCCESS != A3DEntityGetType(faceData.m_pSurface, &type)) {
		DEBUG_STOP;
		return false;
	}

	switch (type)
	{
		case kA3DTypeSurfPlane:
		{
			SurfPlaneDataGuard planeDataGuard(faceData.m_pSurface);
			if (false == planeDataGuard.IsValid()) {
				DEBUG_STOP;
				return false;
			}
			const auto & planeData = planeDataGuard.Data();

			out.kind = SurfKind::Plane;

			out.planeOrigin = Dmi3dx::GetPoint(planeData.m_sTrsf.m_sOrigin) * EXCHAGE_SCALE;

			DVector xAxis = Dmi3dx::GetVector(planeData.m_sTrsf.m_sXVector);
			DVector yAxis = Dmi3dx::GetVector(planeData.m_sTrsf.m_sYVector);

			DVector normal = SafeNormalized(xAxis.Cross(yAxis));
			(void) normal; // TODO: FaceSummary에 plane normal 필드가 생기면 대입

			// 캡 후보를 위해 외곽 원 탐색
			CircleParam circleParam{};
			if (true == ExtractOuterCircle(face, circleParam)) {
				out.outerHasCircle = true;
				out.outerCircle = circleParam;
			}
			// TODO: 외곽 원이 없을 때 NURBS 원 판정 보완
		}
		break;

		case kA3DTypeSurfCylinder:
		{
			SurfCylinderDataGuard cylinderDataGuard(faceData.m_pSurface);
			if (false == cylinderDataGuard.IsValid()) {
				DEBUG_STOP;
				return false;
			}
			const auto & cylinderData = cylinderDataGuard.Data();

			out.kind = SurfKind::Cylinder;

			out.cyl.radius = cylinderData.m_dRadius * EXCHAGE_SCALE;
			out.cyl.origin = Dmi3dx::GetPoint(cylinderData.m_sTrsf.m_sOrigin) * EXCHAGE_SCALE;

			DVector xAxis = Dmi3dx::GetVector(cylinderData.m_sTrsf.m_sXVector);
			DVector yAxis = Dmi3dx::GetVector(cylinderData.m_sTrsf.m_sYVector);

			DVector axis = SafeNormalized(xAxis.Cross(yAxis)); // 원통 축 방향
			BuildXYFromAxis(axis, out.cyl.xAxis, out.cyl.yAxis);
			out.cyl.Orthonormalize();
		}
		break;

		case kA3DTypeSurfNurbs:
		{
			SurfNurbsDataGuard nurbsDataGuard(faceData.m_pSurface);
			if (false == nurbsDataGuard.IsValid()) {
				DEBUG_STOP;
				return false;
			}
			const auto & nurbsData = nurbsDataGuard.Data();

			switch (nurbsData.m_eSurfaceForm)
			{
				case kA3DBSplineSurfaceFormPlane:
					out.kind = SurfKind::NurbsPlane;
					break;

				case kA3DBSplineSurfaceFormCylindrical:
					out.kind = SurfKind::NurbsCylinder; // TODO: 축/반지름 추정 로직 추가
					break;

				default:
					out.kind = SurfKind::Other;
					break;
			}
		}
		break;

		default:
			out.kind = SurfKind::Other;
			break;
	}

	return true;
}


//== 보조 유틸 정의 ==================================================================================

// 두 원(혹은 두 원통)이 같은 축(coaxial) 을 공유하고, 같은 반지름 을 가지는지 판별.
bool AM::DatalConverterImpl::IsCoaxialSameRadius(const CylinderParam & a, const CylinderParam & b, Scalar lenTol, Scalar angTolRad) noexcept
{
	// 기본 가드
	if (lenTol < static_cast<Scalar>(0) || angTolRad < static_cast<Scalar>(0)) {
		DEBUG_STOP;
		return false;
	}

	// 축 정규화 후 평행성(역방향 포함) 검사
	const DVector za = SafeNormalized(a.Axis());
	const DVector zb = SafeNormalized(b.Axis());
	if (false == ParallelWithinTol(za, zb, angTolRad)) {
		return false;
	}

	// 두 축(같은 직선) 사이의 최소 거리 검사
	const DVector d(static_cast<Scalar>(a.origin.x - b.origin.x), static_cast<Scalar>(a.origin.y - b.origin.y), static_cast<Scalar>(a.origin.z - b.origin.z));

	if (PerpDistance(d, za) > lenTol) {
		return false;
	}

	// 반지름 동일성 검사
	if (false == NearlyEqual(a.radius, b.radius, lenTol)) {
		return false;
	}

	return true;
}


// 주어진 평면(face)이 특정 원통(face)의 캡(cap) 역할을 하는지 판별.
bool AM::DatalConverterImpl::IsCapForCylinder(const CircleParam & cap, const CylinderParam & cy, Scalar lenTol, Scalar angTolRad) noexcept
{
	// 인자 가드
	if (lenTol < static_cast<Scalar>(0) || angTolRad < static_cast<Scalar>(0)) {
		DEBUG_STOP;
		return false;
	}

	// 1. 법선 // 축 (정/역방향 허용)
	const DVector n = SafeNormalized(cap.normal);
	const DVector az = SafeNormalized(cy.Axis());
	if (false == ParallelWithinTol(n, az, angTolRad)) {
		return false;
	}

	// 2. 원 중심이 축 위에 있는지: 축에 대한 수직거리 검사
	const DVector d(
		static_cast<Scalar>(cap.origin.x - cy.origin.x),
		static_cast<Scalar>(cap.origin.y - cy.origin.y),
		static_cast<Scalar>(cap.origin.z - cy.origin.z));
	if (PerpDistance(d, az) > lenTol) {
		return false;
	}

	// 3. 반지름 동일성
	if (false == NearlyEqual(static_cast<Scalar>(cap.r), cy.radius, lenTol)) {
		return false;
	}

	return true;
}

// 루프에서 원 하나 찾기 (circle curve 우선)
// 필요시 NURBS-원 판정 추가 가능.
bool AM::DatalConverterImpl::ExtractOuterCircle(const A3DTopoFace * face, CircleParam & outCircle) noexcept
{
	if (nullptr == face) {
		DEBUG_STOP;
		return false;
	}

	TopoFaceDataGuard faceDataGuard(face);
	if (!faceDataGuard.IsValid()) {
		DEBUG_STOP;
		return false;
	}

	const auto & faceData = faceDataGuard.Data();

	for (A3DUns32 i = 0; i < faceData.m_uiLoopSize; ++i) {
		TopoLoopDataGuard loopDataGuard(faceData.m_ppLoops[i]);
		if (false == loopDataGuard.IsValid()) {
			continue;
		}

		const auto & loopData = loopDataGuard.Data();

		for (A3DUns32 j = 0; j < loopData.m_uiCoEdgeSize; ++j) {
			const A3DTopoCoEdge * ce = loopData.m_ppCoEdges[j];
			if (!ce) continue;

			TopoCoEdgeDataGuard coEdgeDataGuard(ce);
			if (false == coEdgeDataGuard.IsValid()) {
				continue;
			}

			const auto & coEdgeData = coEdgeDataGuard.Data();

			const A3DTopoEdge * topoEdge = coEdgeData.m_pEdge;
			if (!topoEdge) continue;

			TopoEdgeDataGuard edgeDataGuard(topoEdge);
			if (false == edgeDataGuard.IsValid()) {
				continue;
			}

			const auto & edgeData = edgeDataGuard.Data();
			if (nullptr == edgeData.m_p3dCurve) {
				continue;
			}

			A3DEEntityType ct = kA3DTypeUnknown;
			if (A3D_SUCCESS != A3DEntityGetType(edgeData.m_p3dCurve, &ct)) continue;

			if (ct == kA3DTypeCrvCircle) {

				CrvCircleDataGuard circleDataGuard(edgeData.m_p3dCurve);
				if (false == circleDataGuard.IsValid()) {
					continue;
				}

				const auto & circleData = circleDataGuard.Data();

				outCircle.r = circleData.m_dRadius * EXCHAGE_SCALE;
				outCircle.origin = Dmi3dx::GetPoint(circleData.m_sTrsf.m_sOrigin) * EXCHAGE_SCALE;

				DVector xAxis = Dmi3dx::GetVector(circleData.m_sTrsf.m_sXVector);
				DVector yAxis = Dmi3dx::GetVector(circleData.m_sTrsf.m_sYVector);

				DVector normal = xAxis.Cross(yAxis);
				outCircle.normal = SafeNormalized(normal);

				return true;
			}
			// TODO: NURBS 원 판정 추가 가능
		}
	}
	return false;
}

//== 쉘에서 Solid Cylinder 찾기 ======================================================================
std::optional<DatalConverterImpl::SolidCylinderResult> AM::DatalConverterImpl::DetectSolidCylinder(const A3DTopoShell * shell, double lenTol, double angTol) noexcept
{
	if (nullptr == shell) {
		return std::nullopt;
	}

	TopoShellDataGuard shellDataGuard(shell);
	if (false == shellDataGuard.IsValid()) {
		return std::nullopt;
	}

	const auto & shellData = shellDataGuard.Data();

	// 1. Face 요약 수집
	std::vector<FaceSummary> planes;
	std::vector<FaceSummary> cylinders;
	planes.reserve(shellData.m_uiFaceSize);
	cylinders.reserve(shellData.m_uiFaceSize);

	for (A3DUns32 nIndex = 0; nIndex < shellData.m_uiFaceSize; ++nIndex) {
		const A3DTopoFace * face = shellData.m_ppFaces[nIndex];
		
		FaceSummary faceSummary{};
		if (false == AnalyzeFaceSurface(face, faceSummary, lenTol, angTol)) {
			continue;
		}

		if (faceSummary.kind == SurfKind::Plane || faceSummary.kind == SurfKind::NurbsPlane) {
			planes.push_back(std::move(faceSummary));
		}
		else if (faceSummary.kind == SurfKind::Cylinder /*|| fs.kind == SurfKind::NurbsCylinder*/) { // NURBS Cyl은 후속 확장
			cylinders.push_back(std::move(faceSummary));
		}
	}

	if (cylinders.empty() || planes.size() < 2) {
		return std::nullopt;
	}

	// 2. 옆면 군(동축·동반지름)으로 묶기
	struct Group { 
		CylinderParam ref; 
		std::vector<const A3DTopoFace *> members; 
	};

	std::vector<Group> groups;

	for (auto & cylinder : cylinders) {
		
		// 현재 cylinder가 어떤 그룹에 들어갔는지 표시
		bool placed = false;

		// 1. 기존 그룹들 중에서 동일 축 + 동일 반지름(공차 내)인 그룹이 있는지 탐색.
		for (auto & group : groups) {
			// group.ref : 그룹의 대표(첫 원소) 실린더 파라미터
			// cylinder.cyl : 지금 배치하려는 실린더 파라미터
			if (IsCoaxialSameRadius(group.ref, cylinder.cyl, static_cast<Scalar>(lenTol), static_cast<Scalar>(angTol))) {
				// 같은 그룹 판정, 해당 그룹의 멤버로 face를 추가
				group.members.push_back(cylinder.face);
				placed = true; 
				break;
			}
		}

		// 2. 어떤 그룹에도 들어가지 못했다면, 새 그룹을 생성.
		if (false == placed) {
			Group newGroup{ cylinder.cyl, { cylinder.face } };
			groups.push_back(std::move(newGroup));
		}
	}

	if (groups.empty()) {
		return std::nullopt;
	}

	// 같은 축(coaxial) 여부만 판단(반지름 무시)하는 헬퍼
	auto sameAxisWithinTol = [&](const CylinderParam & A, const CylinderParam & B) -> bool {
		const H3DF::DVector za = SafeNormalized(A.Axis());
		const H3DF::DVector zb = SafeNormalized(B.Axis());
		if (!ParallelWithinTol(za, zb, static_cast<Scalar>(angTol))) {
			return false;
		}
		const H3DF::DVector d(
			static_cast<Scalar>(A.origin.x - B.origin.x),
			static_cast<Scalar>(A.origin.y - B.origin.y),
			static_cast<Scalar>(A.origin.z - B.origin.z)
		);
		return PerpDistance(d, za) <= static_cast<Scalar>(lenTol);
	};

	// 3. 각 그룹에 대해 평면 캡 2개 찾기
	for (auto & group : groups) {
		// 캡 후보 리스트 구성(원 루프 가지고, 축/반지름 매칭되는 평면)
		std::vector<const FaceSummary *> caps;
		caps.reserve(4);

		for (auto & planeFace : planes) {
			// 필요시 플레인에서 원 추출 시도(AnalyzeFaceSurface에서 이미 시도함)
			if (true == planeFace.outerHasCircle) {
				CircleParam c{};
				if (ExtractOuterCircle(planeFace.face, c)) {
					const_cast<FaceSummary &>(planeFace).outerHasCircle = true;
					const_cast<FaceSummary &>(planeFace).outerCircle = c;
				}
			}

			if (!planeFace.outerHasCircle) {
				continue;
			}

			if (IsCapForCylinder(planeFace.outerCircle, group.ref, static_cast<Scalar>(lenTol), static_cast<Scalar>(angTol))) {
				caps.push_back(&planeFace);
			}
		}

		if (caps.size() < 2) {
			continue;
		}

		// 서로 다른 높이의 두 캡 선택(축 방향 거리 최대 페어)
		const H3DF::DVector axis = group.ref.Axis();
		const auto dotAxis = [&](const FaceSummary * a, const FaceSummary * b)->double {
			H3DF::DVector ca(a->outerCircle.origin.x, a->outerCircle.origin.y, a->outerCircle.origin.z);
			H3DF::DVector cb(b->outerCircle.origin.x, b->outerCircle.origin.y, b->outerCircle.origin.z);
			H3DF::DVector d(cb.x - ca.x, cb.y - ca.y, cb.z - ca.z);
			return std::abs(static_cast<double>(d.Dot(axis)));
		};

		const FaceSummary * cap0 = nullptr;
		const FaceSummary * cap1 = nullptr;
		double bestH = 0.0;

		for (size_t i = 0; i < caps.size(); ++i) {
			for (size_t j = i + 1; j < caps.size(); ++j) {
				double h = dotAxis(caps[i], caps[j]);
				if (h > bestH && h > lenTol) {
					bestH = h; cap0 = caps[i]; cap1 = caps[j];
				}
			}
		}

		if (!cap0 || !cap1) {
			continue;
		}

		// 동축 상에 '다른 반지름'이 존재하면 솔리드가 아닌 hollow로 간주 → 솔리드 반환 금지
		bool hasOtherRadiusOnAxis = false;
		for (const auto & fs : cylinders) {
			if (!sameAxisWithinTol(group.ref, fs.cyl)) {
				continue;
			}
			const double rdiff = std::abs(static_cast<double>(group.ref.radius) -
				static_cast<double>(fs.cyl.radius));
			if (rdiff > lenTol) {
				hasOtherRadiusOnAxis = true;
				break;
			}
		}
		if (hasOtherRadiusOnAxis) {
			continue; // 솔리드 확정하지 않고 다음 그룹 탐색 → 상위에서 hole 경로로 처리
		}

		// 솔리드 확정
		// (선택) 옆면 둘레 2π 검증은 생략(분할 옆면이 많을 수 있음)
		SolidCylinderResult res{};
		res.isSolidCylinder = true;
		res.cyl = group.ref;
		res.height = bestH;
		res.caps[0] = cap0->face;
		res.caps[1] = cap1->face;
		res.lateral = group.members;

		return res;
	}

	return std::nullopt;
}

//== AM Datal writer ===============================================================================

bool DatalConverterImpl::ExportToDatal(const std::filesystem::path & outPathUTF8)
{
	using W = AmWriter;
	if (auto st = m_writer.Open(outPathUTF8, /*BOM*/true); st != W::Status::Ok) return false;
	{
		auto tmpl = m_writer.BeginTemplate("SEPARATED OIL TANK HEATER-2661", "EQUI", 310.0);
		W::Orientation o; 
		o.mapY = { W::Axis::Z, -1 }; 
		o.mapZ = { W::Axis::Y, +1 };
		auto box = m_writer.BeginBox(0.0, 274.3, -205.0, o, 50, 10, 100);
		(void) box; // RAII
	}
	m_writer.Close();
	return true;
}

// BlockGuard
AmWriter::BlockGuard::BlockGuard(AmWriter * w, std::string_view name)
	: m_writer(w), m_name(name), m_closed(false) {
}

AmWriter::BlockGuard::BlockGuard(BlockGuard && o) noexcept { 
	*this = std::move(o); 
}

auto AmWriter::BlockGuard::operator=(BlockGuard && o) noexcept -> BlockGuard & 
{
	if (this != &o) {
		if (!m_closed && m_writer) {
			End();
		}

		m_writer = o.m_writer; 
		o.m_writer = nullptr;
		m_name = std::move(o.m_name);
		m_closed = o.m_closed; 
		o.m_closed = true;
	}
	return *this;
}

AmWriter::BlockGuard::~BlockGuard() 
{
	if (!m_closed && m_writer) {
		End();
	}
}

void AmWriter::BlockGuard::End() 
{
	if (m_writer) {
		(void) m_writer->endBlock(m_name);
	}

	m_closed = true; 
	m_writer = nullptr;
}

// lifecycle
AmWriter::~AmWriter() 
{ 
	Close(); 
}

AmWriter::AmWriter(AmWriter && o) noexcept 
{ 
	*this = std::move(o); 
}

auto AmWriter::operator=(AmWriter && o) noexcept -> AmWriter & {
	if (this != &o) {
		m_out = std::move(o.m_out);
		o.m_lastError = std::move(o.m_lastError);
		m_stack = std::move(o.m_stack);
	}
	return *this;
}

static void H3DX_writeBOM(std::ofstream & out) {
	const unsigned char bom[3] = { 0xEF, 0xBB, 0xBF };
	out.write(reinterpret_cast<const char *>(bom), 3);
}

// file
auto AmWriter::Open(const std::filesystem::path & path_utf8, bool writeBOM) noexcept -> Status 
{
	if (IsOpen()) { 
		setError(Status::AlreadyOpen, "file already open"); 
		return Status::AlreadyOpen; 
	}

	std::error_code ec;
	if (!path_utf8.empty()) {
		std::filesystem::create_directories(path_utf8.parent_path(), ec);
	}

	m_out.open(path_utf8, std::ios::binary | std::ios::out | std::ios::trunc);

	if (!m_out) { 
		setError(Status::FileOpenFailed, "failed to open file"); 
		return Status::FileOpenFailed; 
	}

	if (writeBOM) {
		H3DX_writeBOM(m_out);
	}

	m_stack.clear(); 
	m_lastError = {};

	return Status::Ok;
}

void AmWriter::Close() noexcept 
{
	if (false == IsOpen()) {
		return;
	}
	
	while (!m_stack.empty())  { 
		(void) endBlock(m_stack.back()); 
	}

	m_out.flush();
	m_out.close();
}

bool AmWriter::IsOpen() const noexcept 
{ 
	return m_out.is_open(); 
}

void AmWriter::Flush() noexcept 
{
	if (false == IsOpen()) {
		return;
	}

	m_out.flush();
}

// input / equipment
auto AmWriter::BeginInput() -> Status { return EmitLine("INPUT BEGIN"); }

auto AmWriter::EndInput() -> Status {
	if (!IsOpen()) {
		setError(Status::NotOpen, "file not open");
		return Status::NotOpen;
	}
	// Two spaces before "EQUIPMENT" per requested format
	if (m_currentEquipment.empty()) {
		return EmitLine("INPUT END");
	}
	return EmitLine(std::string("INPUT END  EQUIPMENT ") + m_currentEquipment);
}

auto AmWriter::FinishInput() -> Status { return EmitLine("INPUT FINISH"); }

auto AmWriter::WriteEquipmentHeader(std::string_view equipmentPath, bool buil, std::string_view dsco, std::string_view ptsp, std::string_view insc) -> Status 
{
	if (!IsOpen()) {
		setError(Status::NotOpen, "file not open");
		return Status::NotOpen;
	}

	m_currentEquipment = std::string(equipmentPath);
	
	Status s;
	s = EmitLine("NEW EQUIPMENT " + std::string(equipmentPath));	if (s != Status::Ok) return s;
	s = EmitRawKV("BUIL", buil ? "true" : "false");					if (s != Status::Ok) return s;
	s = EmitRawKV("DSCO", dsco);									if (s != Status::Ok) return s;
	s = EmitRawKV("PTSP", ptsp);									if (s != Status::Ok) return s;
	s = EmitRawKV("INSC", insc);									if (s != Status::Ok) return s;

	return Status::Ok;
}

auto AmWriter::StartWithTemplate(std::string_view equipmentPath,
	std::string_view desc,
	std::string_view purp,
	std::optional<double> userWeightKg,
	bool buil,
	std::string_view dsco,
	std::string_view ptsp,
	std::string_view insc) -> Status 
{
	// Begin input and equipment header
	Status s = BeginInput();
	if (s != Status::Ok) {
		return s;
	}

	s = WriteEquipmentHeader(equipmentPath, buil, dsco, ptsp, insc);
	if (s != Status::Ok) {
		return s;
	}

	EmitLine(""); // NEW TMPLATE 앞에 공백 라인

	// Begin template WITHOUT RAII guard (avoid immediate END if caller ignores return)
	(void) beginBlock("NEW TMPLATE");
	EmitKV("DESC", std::string(desc));
	EmitRawKV("PURP", std::string(purp));
	if (userWeightKg.has_value()) {
		EmitLine("USRWEI ( " + formatNumber(*userWeightKg, 3) + "kg )");
	}
	return Status::Ok;
}

// template / primitives
auto AmWriter::BeginTemplate(std::string_view desc,
	std::string_view purp,
	std::optional<double> userWeightKg) -> BlockGuard {

	EmitLine(""); // NEW TMPLATE 앞에 공백 라인

	(void) beginBlock("NEW TMPLATE");
	EmitKV("DESC", std::string(desc));
	EmitKV("PURP", std::string(purp));
	if (userWeightKg.has_value()) EmitLine("USRWEI ( " + formatNumber(*userWeightKg, 3) + "kg )");
	return BlockGuard(this, "NEW TMPLATE");
}

auto AmWriter::End() -> Status 
{
	EmitLine(""); // NEW TMPLATE End 앞에 공백 라인

	return endBlock(m_stack.empty() ? std::string_view{} : m_stack.back());
}

auto AmWriter::BeginBox(double px, double py, double pz,
	const Orientation & ori,
	double xlen, double ylen, double zlen) -> BlockGuard {
	(void) beginBlock("NEW BOX");
	EmitLine("POS X " + std::string(formatNumber(px, 3)) +
		"mm Y " + std::string(formatNumber(py, 3)) +
		"mm Z " + std::string(formatNumber(pz, 3)) + "mm");
	
	EmitLine("ORI Y is " + axisMapStr(ori.mapY) + " and Z is " + axisMapStr(ori.mapZ));

	EmitKV("XLEN", xlen, "mm");
	EmitKV("YLEN", ylen, "mm");
	EmitKV("ZLEN", zlen, "mm");

	return BlockGuard(this, "NEW BOX");
}

auto AmWriter::BeginCylinder(double cx, double cy, double cz, double radius, double height, Axis axis) -> BlockGuard 
{
	EmitLine(""); // NEW CYLINDER 앞에 공백 라인
	
	(void) beginBlock("NEW CYLINDER");
	EmitLine("POS X " + std::string(formatNumber(cx, 3)) +
		"mm Y " + std::string(formatNumber(cy, 3)) +
		"mm Z " + std::string(formatNumber(cz, 3)) + "mm");

	// ORI derived from axis with zero yaw (no angle numbers)
	EmitLine(formatOriFromAxisAndYaw(axis, 1, 0.0, 0.0));

	EmitKV("RADIUS", radius, "mm");
	EmitKV("HEIGHT", height, "mm");

	return BlockGuard(this, "NEW CYLINDER");
}

// Overload: axis + yaw angle (deg) with optional quantization step
auto AmWriter::BeginCylinder(double cx, double cy, double cz, double radius, double height, Axis axis, double yawDeg, double quantStepDeg) -> BlockGuard 
{
	EmitLine(""); // NEW CYLINDER 앞에 공백 라인

	(void) beginBlock("NEW CYLINDER");

	EmitLine("POS X " + std::string(formatNumber(cx, 3)) +
		"mm Y " + std::string(formatNumber(cy, 3)) +
		"mm Z " + std::string(formatNumber(cz, 3)) + "mm");

	EmitLine(formatOriFromAxisAndYaw(axis, 1, yawDeg, quantStepDeg));

	EmitKV("RADIUS", radius, "mm");
	EmitKV("HEIGHT", height, "mm");
	return BlockGuard(this, "NEW CYLINDER");
};

// New overload: explicit ORI + DIAM/HEIG
auto AmWriter::BeginCylinder(double cx, double cy, double cz, double diam, double heig, const Orientation & ori) -> BlockGuard 
{
	EmitLine("");

	(void) beginBlock("NEW CYLINDER");

	EmitLine("POS X " + std::string(formatNumber(cx, 3)) +
		"mm Y " + std::string(formatNumber(cy, 3)) +
		"mm Z " + std::string(formatNumber(cz, 3)) + "mm");

	EmitLine("ORI Y is " + axisMapStr(ori.mapY) + " and Z is " + axisMapStr(ori.mapZ));

	EmitKV("DIAM", diam, "mm");
	EmitKV("HEIG", heig, "mm");

	return BlockGuard(this, "NEW CYLINDER");
}

auto AmWriter::WriteSolidCylinder(double cx, double cy, double cz, const Orientation & ori, double diam, double heig) -> Status 
{
	if (!IsOpen()) { 
		setError(Status::NotOpen, "file not open"); 
		return Status::NotOpen; 
	}
	// Delegate to RAII cylinder opener (prints POS/ORI/DIAM/HEIG) and let guard close the block.
	auto guard = BeginCylinder(cx, cy, cz, diam, heig, ori);

	(void) guard; // RAII: END on scope exit

	return Status::Ok;
}

// solid cylinder (detected geometry)
auto AmWriter::WriteSolidCylinder(double cx, double cy, double cz,double nx, double ny, double nz, double radius, double height) -> Status 
{
	if (!IsOpen()) { 
		setError(Status::NotOpen, "file not open"); 
		return Status::NotOpen; 
	}

	// Z 축 정규화
	const double nlen = std::sqrt(nx * nx + ny * ny + nz * nz);
	if (!(nlen > 0.0)) {
		setError(Status::InvalidArgument, "axis vector is zero");
		return Status::InvalidArgument;
	}

	const double zx = nx / nlen;
	const double zy = ny / nlen;
	const double zz = nz / nlen;

	// 월드 힌트(+Y, 거의 평행이면 +X)를 Z에 수직으로 투영 → Y 벡터
	const double dotUp = zy; // dot((0,1,0), Z)
	double hx = 0.0, hy = 1.0, hz = 0.0;
	if (std::abs(dotUp) > 0.985) {
		hx = 1.0; hy = 0.0; hz = 0.0;
	}

	const double hdotz = hx * zx + hy * zy + hz * zz;
	double yx = hx - hdotz * zx;
	double yy = hy - hdotz * zy;
	double yz = hz - hdotz * zz;
	const double ylen = std::sqrt(yx * yx + yy * yy + yz * yz);

	if (ylen > 0.0) {
		yx /= ylen; yy /= ylen; yz /= ylen;
	}
	else {
		// 병행/퇴화 시 대체 직교 벡터 선택
		if (std::abs(zx) <= std::abs(zy) && std::abs(zx) <= std::abs(zz)) { yx = 0.0; yy = -zz; yz = +zy; }
		else if (std::abs(zy) <= std::abs(zx) && std::abs(zy) <= std::abs(zz)) { yx = +zz; yy = 0.0; yz = -zx; }
		else { yx = -zy; yy = +zx; yz = 0.0; }
		const double ylen2 = std::sqrt(yx * yx + yy * yy + yz * yz);
		if (ylen2 > 0.0) { yx /= ylen2; yy /= ylen2; yz /= ylen2; }
	}

	const double diam = radius * 2.0;

	EmitLine("");
	(void) beginBlock("NEW CYLINDER");

	EmitLine("POS X " + std::string(formatNumber(cx, 3)) +
		"mm Y " + std::string(formatNumber(cy, 3)) +
		"mm Z " + std::string(formatNumber(cz, 3)) + "mm");

	// 정확 각도(스냅 없음)로 Y와 Z 모두를 혼합 각도로 표기
	EmitLine(formatOriYZFromVectors(yx, yy, yz, zx, zy, zz, 0.0));

	EmitKV("DIAM", diam, "mm");
	EmitKV("HEIG", height, "mm");
	return endBlock("NEW CYLINDER");
}

auto AmWriter::WritePlainCylinder(double cx, double cy, double cz, double diam, double heig) -> Status
{
	if (!IsOpen()) {
		setError(Status::NotOpen, "file not open");
		return Status::NotOpen;
	}

	EmitLine("");
	(void) beginBlock("NEW CYLINDER");

	EmitLine("POS X " + std::string(formatNumber(cx, 3)) +
		"mm Y " + std::string(formatNumber(cy, 3)) +
		"mm Z " + std::string(formatNumber(cz, 3)) + "mm");

	// ORI 출력 없음 (요청 포맷 준수)
	EmitKV("DIAM", diam, "mm");
	EmitKV("HEIG", heig, "mm");

	//return endBlock("NEW CYLINDER");
	return Status::Ok;
}

auto AmWriter::WriteHoleCylinder(double cx, double cy, double cz, double nx, double ny, double nz, double radius, double height, double stepDeg) -> Status
{
	if (!IsOpen()) {
		setError(Status::NotOpen, "file not open");
		return Status::NotOpen;
	}

	// Z 축 정규화
	const double nlen = std::sqrt(nx * nx + ny * ny + nz * nz);
	if (!(nlen > 0.0)) {
		setError(Status::InvalidArgument, "axis vector is zero");
		return Status::InvalidArgument;
	}
	const double zx = nx / nlen;
	const double zy = ny / nlen;
	const double zz = nz / nlen;

	// 월드 +Y(거의 평행이면 +X) 투영으로 Y 벡터 구축
	const double dotUp = zy; // dot((0,1,0), Z)
	double hx = 0.0, hy = 1.0, hz = 0.0;
	if (std::abs(dotUp) > 0.985) {
		hx = 1.0; hy = 0.0; hz = 0.0;
	}

	const double hdotz = hx * zx + hy * zy + hz * zz;
	double yx = hx - hdotz * zx;
	double yy = hy - hdotz * zy;
	double yz = hz - hdotz * zz;
	const double ylen = std::sqrt(yx * yx + yy * yy + yz * yz);
	if (ylen > 0.0) {
		yx /= ylen; yy /= ylen; yz /= ylen;
	}
	else {
		// 병행/퇴화 대비 대체 직교 벡터
		if (std::abs(zx) <= std::abs(zy) && std::abs(zx) <= std::abs(zz)) { yx = 0.0; yy = -zz; yz = +zy; }
		else if (std::abs(zy) <= std::abs(zx) && std::abs(zy) <= std::abs(zz)) { yx = +zz; yy = 0.0; yz = -zx; }
		else { yx = -zy; yy = +zx; yz = 0.0; }
		const double ylen2 = std::sqrt(yx * yx + yy * yy + yz * yz);
		if (ylen2 > 0.0) { yx /= ylen2; yy /= ylen2; yz /= ylen2; }
	}

	const double diam = radius * 2.0;

	EmitLine("");
	(void) beginBlock("NEW NCYLINDER");

	EmitLine("POS X " + std::string(formatNumber(cx, 3)) +
		"mm Y " + std::string(formatNumber(cy, 3)) +
		"mm Z " + std::string(formatNumber(cz, 3)) + "mm");

	// Y, Z 모두 각도 혼합으로 ORI 출력 (기본 11.25° 스냅 → 샘플과 동일)
	EmitLine(formatOriYZFromVectors(yx, yy, yz, zx, zy, zz, stepDeg));

	EmitKV("DIAM", diam, "mm");
	EmitKV("HEIG", height, "mm");
	EmitLine("PRODHT 'Hole'");

	return endBlock("NEW NCYLINDER");
}

// emit
auto AmWriter::EmitLine(std::string_view line) -> Status 
{
	if (!IsOpen()) {
		setError(Status::NotOpen, "file not open");
		return Status::NotOpen;
	}

	try {
		m_out.write(line.data(), static_cast<std::streamsize>(line.size()));
		m_out.put('\n');

		if (!m_out.good()) { 
			setError(Status::WriteFailed, "stream write failed"); 
			return Status::WriteFailed; 
		}
	}
	catch (const std::exception & ex) {
		setError(Status::WriteFailed, ex.what());
		return Status::WriteFailed;
	}
	return Status::Ok;
}

auto AmWriter::EmitKV(std::string_view key, std::string_view value) -> Status {
	return EmitLine(std::string(key) + " '" + std::string(value) + "'");
}

auto AmWriter::EmitRawKV(std::string_view key, std::string_view value) -> Status {
	return EmitLine(std::string(key) + " " + std::string(value));
}

auto AmWriter::EmitKV(std::string_view key, double value, std::string_view unit) -> Status {
	std::string v = formatNumber(value, 3);
	if (!unit.empty()) v += std::string(unit);
	return EmitLine(std::string(key) + " " + v);
}

const AmWriter::Error & AmWriter::LastError() const noexcept { return m_lastError; }

// internals
auto AmWriter::beginBlock(std::string_view name) -> Status 
{
	if (!IsOpen()) { 
		setError(Status::NotOpen, "file not open"); 
		return Status::NotOpen; 
	}

	EmitLine(std::string(name));
	m_stack.emplace_back(name);

	return Status::Ok;
}

auto AmWriter::endBlock(std::string_view name) -> Status 
{
	if (!IsOpen()) { 
		setError(Status::NotOpen, "file not open"); 
		return Status::NotOpen; 
	}

	if (m_stack.empty()) { 
		setError(Status::InvalidArgument, "no open block to end"); 
		return Status::InvalidArgument; 
	}

	if (!name.empty() && m_stack.back() != name) {
		setError(Status::InvalidArgument, "block mismatch: end '" + std::string(name) + "' while '" + m_stack.back() + "' open");
		return Status::InvalidArgument;
	}

	EmitLine("END"); m_stack.pop_back(); return Status::Ok;
}

void AmWriter::setError(Status c, std::string msg) { m_lastError.code = c; m_lastError.message = std::move(msg); }

std::string AmWriter::axisToStr(Axis a) 
{
	switch (a) {
		case Axis::X:
			return "X";
		case Axis::Y:
			return "Y";
		default:
			return "Z";
	}
}

std::string AmWriter::axisMapStr(const AxisMap & m) 
{ 
	std::string s; 
	if (m.sign < 0) s += "-"; 
	s += axisToStr(m.to); 
	return s; 
}

double AmWriter::normalizeDeg(double deg) noexcept {
	double d = std::fmod(deg, 360.0);
	if (d < 0.0) d += 360.0;
	return d;
}

double AmWriter::quantizeDeg(double deg, double step) noexcept {
	if (step <= 0.0) return deg;
	return std::round(deg / step) * step;
}

std::string AmWriter::formatOriFromAxisAndYaw(Axis axis, int zSign, double yawDeg, double stepDeg) 
{
	// Normalize and quantize yaw (roll around axis)
	double yaw = normalizeDeg(yawDeg);
	yaw = normalizeDeg(quantizeDeg(yaw, stepDeg));

	// Determine sector [0..3] and local angle within [0,90)
	int sector = static_cast<int>(std::floor(yaw / 90.0)) & 3;
	double a = yaw - static_cast<double>(sector) * 90.0;

	AxisMap start{}, toward{};
	const AxisMap zlab{ axis, (zSign >= 0 ? +1 : -1) };

	auto mk = [](Axis ax, int s) { return AxisMap{ ax, s }; };

	switch (axis) {
		case Axis::Z:
			switch (sector) {
				case 0: start = mk(Axis::Y, +1); toward = mk(Axis::X, -1); break; // Y -> -X
				case 1: start = mk(Axis::X, -1); toward = mk(Axis::Y, -1); break; // -X -> -Y
				case 2: start = mk(Axis::Y, -1); toward = mk(Axis::X, +1); break; // -Y -> X
				default: start = mk(Axis::X, +1); toward = mk(Axis::Y, +1); break; // X -> Y
			}
			break;
		case Axis::X:
			switch (sector) {
				case 0: start = mk(Axis::Y, +1); toward = mk(Axis::Z, +1); break; // Y -> Z
				case 1: start = mk(Axis::Z, +1); toward = mk(Axis::Y, -1); break; // Z -> -Y
				case 2: start = mk(Axis::Y, -1); toward = mk(Axis::Z, -1); break; // -Y -> -Z
				default: start = mk(Axis::Z, -1); toward = mk(Axis::Y, +1); break; // -Z -> Y
			}
			break;
		case Axis::Y:
			switch (sector) {
				case 0: start = mk(Axis::X, +1); toward = mk(Axis::Z, +1); break; // X -> Z
				case 1: start = mk(Axis::Z, +1); toward = mk(Axis::X, -1); break; // Z -> -X
				case 2: start = mk(Axis::X, -1); toward = mk(Axis::Z, -1); break; // -X -> -Z
				default: start = mk(Axis::Z, -1); toward = mk(Axis::X, +1); break; // -Z -> X
			}
			break;
	}

	std::string line = "ORI Y is " + axisMapStr(start);
	
	if (a > 0.0) {
		line += " " + formatNumber(a, 3) + " " + axisMapStr(toward);
	}

	line += " and Z is " + axisMapStr(zlab);
	return line;
}

std::string AmWriter::formatOriFromVector(double nx, double ny, double nz, double stepDeg)
{
	// 두 개의 가장 큰 성분으로 평면 결정 (tie-breaker: Z > Y > X)
	const double ax = std::abs(nx), ay = std::abs(ny), az = std::abs(nz);

	struct AxisComp { 
		Axis ax; 
		double val; 
		double abs; 
	} comps[3] = {
		{ Axis::X, nx, ax }, 
		{ Axis::Y, ny, ay }, 
		{ Axis::Z, nz, az }
	};

	auto prio = [](Axis a) { 
		return (a == Axis::Z ? 3 : (a == Axis::Y ? 2 : 1)); 
	};

	std::sort(std::begin(comps), std::end(comps), [&](const AxisComp & A, const AxisComp & B) {
		if (A.abs != B.abs) return A.abs > B.abs;      // 큰 값 우선
		return prio(A.ax) > prio(B.ax);               // 동률: Z > Y > X
		});

	const AxisComp & sA = comps[0]; // Z 라인 시작축(큰 성분)
	const AxisComp & tA = comps[1]; // Z 라인 향하는 축(작은 성분)
	const AxisComp & oA = comps[2]; // Y 라인(직교축 = 최소 성분)

	double ang = std::atan2(std::abs(tA.val), std::abs(sA.val)) * 180.0 / M_PI; // [0,90]
	if (stepDeg > 0.0) ang = normalizeDeg(quantizeDeg(ang, stepDeg));

	// "ORI Y is <orth> and Z is <start> [ang] <toward>"
	std::string line = "ORI Y is " + axisMapStr(AxisMap{ oA.ax, +1 });
	line += " and Z is " + axisMapStr(AxisMap{ sA.ax, (sA.val >= 0.0 ? +1 : -1) });
	if (ang > 0.0) {
		line += " " + formatNumber(ang, 3) + " "
			+ axisMapStr(AxisMap{ tA.ax, (tA.val >= 0.0 ? +1 : -1) });
	}

	return line;
}

std::string AmWriter::formatOriYZFromVectors(double yx, double yy, double yz, double zx, double zy, double zz, double stepDeg)
{
	auto mixLine = [&](char which,double vx,double vy, double vz) -> std::string
	{
		struct AxisComp { Axis ax; double val; double abs; };
		AxisComp comps[3] = {
			{ Axis::X, vx, std::abs(vx) },
			{ Axis::Y, vy, std::abs(vy) },
			{ Axis::Z, vz, std::abs(vz) }
		};

		auto prio = [](Axis a) { return (a == Axis::Z ? 3 : (a == Axis::Y ? 2 : 1)); };

		std::sort(std::begin(comps), std::end(comps),
			[&](const AxisComp & A, const AxisComp & B) {
				if (A.abs != B.abs) return A.abs > B.abs;   // 큰 값 우선
				return prio(A.ax) > prio(B.ax);             // 동률: Z > Y > X
			});

		const AxisComp & sA = comps[0]; // 시작축
		const AxisComp & tA = comps[1]; // 향하는 축

		double ang = std::atan2(std::abs(tA.val), std::abs(sA.val)) * 180.0 / M_PI; // [0,90]
		if (stepDeg > 0.0) {
			ang = normalizeDeg(quantizeDeg(ang, stepDeg));
		}

		std::string line;
		line.reserve(64);
		line += (which == 'Y' ? "ORI Y is " : "Z is ");
		line += axisMapStr(AxisMap{ sA.ax, (sA.val >= 0.0 ? +1 : -1) });
		if (ang > 0.0) {
			line += " ";
			line += formatNumber(ang, 3);
			line += " ";
			line += axisMapStr(AxisMap{ tA.ax, (tA.val >= 0.0 ? +1 : -1) });
		}
		return line;
	};

	// "ORI Y is ... and Z is ..." 형태로 결합
	std::string left = mixLine('Y', yx, yy, yz);
	std::string right = mixLine('Z', zx, zy, zz);

	return left + " and " + right;
}

double AmWriter::autoYawFromAxisDeg(double nx, double ny, double nz, Axis a) noexcept 
{
	// normalize axis
	const double len = std::sqrt(nx * nx + ny * ny + nz * nz);
	if (!(len > 0.0)) {
		return 0.0;
	}

	const double vx = nx / len, vy = ny / len, vz = nz / len;

	// choose hint: world +Y, fallback +X if nearly parallel
	const double dotUp = vy; // dot((0,1,0), v)
	double hx = 0.0, hy = 1.0, hz = 0.0;
	if (std::abs(dotUp) > 0.985) 
	{ 
		hx = 1.0; 
		hy = 0.0;
		hz = 0.0; 
	}

	// project hint onto plane perpendicular to v: h_proj = h - (h·v) v
	const double hdotv = hx * vx + hy * vy + hz * vz;
	double yx = hx - hdotv * vx;
	double yy = hy - hdotv * vy;
	double yz = hz - hdotv * vz;
	const double ylen = std::sqrt(yx * yx + yy * yy + yz * yz);
	if (!(ylen > 0.0)) {
		// pathological: pick another fallback
		yx = (a == Axis::Z ? 1.0 : 0.0);
		yy = (a == Axis::Z ? 0.0 : 1.0);
		yz = 0.0;
	}
	else {
		yx /= ylen; yy /= ylen; yz /= ylen;
	}

	double deg = 0.0;

	switch (a) {
		case Axis::Z:  // XY-plane: 0° at +Y, +90° at -X
			deg = std::atan2(-yx, yy) * 180.0 / M_PI; 
			break;

		case Axis::X: // YZ-plane: 0° at +Y, +90° at +Z
			deg = std::atan2(yz, yy) * 180.0 / M_PI;
			break; 

		case Axis::Y: // XZ-plane: 0° at +X, +90° at +Z
			deg = std::atan2(yz, yx) * 180.0 / M_PI;
			break; 
	}

	// normalize to [0,360)
	while (deg < 0.0) {
		deg += 360.0;
	}

	while (deg >= 360.0) {
		deg -= 360.0;
	}

	return deg;
}

std::string AmWriter::formatOrientation(const Orientation & ori) 
{ 
	return axisMapStr(ori.mapY) + " and Z is " + axisMapStr(ori.mapZ); 
}

std::string AmWriter::formatNumber(double v, int maxDecimals) 
{
	double rounded = std::round(v);
	if (std::fabs(v - rounded) < 1e-9) {
		std::ostringstream oss; oss.setf(std::ios::fixed); oss << std::setprecision(0) << rounded; return oss.str();
	}
	std::ostringstream oss; oss.setf(std::ios::fixed); oss << std::setprecision(maxDecimals) << v; std::string s = oss.str();
	if (auto pos = s.find('.'); pos != std::string::npos) { while (!s.empty() && s.back() == '0') s.pop_back(); if (!s.empty() && s.back() == '.') s.pop_back(); }
	return s;
}

AmWriter::Axis AmWriter::pickDominantAxis(double nx, double ny, double nz) noexcept {
	const double ax = std::abs(nx), ay = std::abs(ny), az = std::abs(nz);
	if (ax >= ay && ax >= az) return Axis::X; if (ay >= ax && ay >= az) return Axis::Y; return Axis::Z;
}

int AmWriter::pickAxisSign(Axis a, double nx, double ny, double nz) noexcept {
        switch (a) { case Axis::X: return nx >= 0.0 ? +1 : -1; case Axis::Y: return ny >= 0.0 ? +1 : -1; default: return nz >= 0.0 ? +1 : -1; }
}

AmWriter::Orientation AmWriter::orientationFromAxis(double nx, double ny, double nz) noexcept {
	Orientation ori;
	Axis a = pickDominantAxis(nx, ny, nz);
	int  s = pickAxisSign(a, nx, ny, nz);
	ori.mapZ = { a, s };
	// Z와 다른 안정적인 Y축 선택
	ori.mapY = { (a != Axis::Y ? Axis::Y : Axis::X), +1 };
	return ori;
}
