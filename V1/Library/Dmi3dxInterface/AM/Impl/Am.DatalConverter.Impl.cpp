#include "StdAfx.h"

#include "Am.DatalConverter.Impl.h"

#include <Common_Define.h>

#include "../Am.Equipment.h"
#include "../Am.Template.h"

#include "../../Dmi3dx.h"
#include "../../3DX.DataGuard.h"

#include <cassert>
#include <iomanip>
#include <algorithm>
#include <cmath>
#include <queue>

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

// Connex에서 Shell 단위로 순회하면서 Template을 생성하고, Shell 내에서 Primitive를 감지하여 추가함.
bool AM::DatalConverterImpl::ProcessTopoConnex(const A3DTopoConnex * connex, AM::Equipment & equipment) noexcept
{
	if(nullptr == connex) {
		return false;
	}

	H3DX::TopoConnexDataGuard dataGuard(connex);
	if (!dataGuard.IsValid()) {
		return false;
	}

	const auto & connexData = dataGuard.Data();

	// Template에 사용할 이름을 추출
	std::string_view connexName;
	Dmi3dx::GetName(connex, connexName);

	AM::TemplateKit templateKit;

	// 이름이 있는 경우 Description으로 활용
	if (false == connexName.empty()) {
		templateKit.setDescription(connexName);
	}
	else {
		templateKit.setDescription("TopoConnex");
	}

	// TemplateKit을 사용해서 template를 생성, Equipment에서 Kit을 이용해서, Template를 생성     
	AM::Template & templ = equipment.insertTemplate(templateKit);

	// Connex 내의 Shell 순회
	for (A3DUns32 i = 0; i < connexData.m_uiShellSize; ++i) {
		ProcessTopoShell(connexData.m_ppShells[i], templ);
	}

	return true;
}

// Shell에서 Primitive를 감지하여 Template에 추가해야 함.
// Primitive가 여러개 존재할 수 있음.
bool AM::DatalConverterImpl::ProcessTopoShell(const A3DTopoShell * shell, AM::Template & amTemplate) noexcept
{
	if (nullptr == shell) {
		return false;
	}

	H3DX::TopoShellDataGuard dataGuard(shell);
	if (!dataGuard.IsValid()) {
		return false;
	}

	const auto & shellData = dataGuard.Data();

	// 1. Edge 연결성을 기반으로 Outer Face Boundary를 찾습니다.
	//std::set<const A3DTopoFace *> outerFaces = processOuterFaceBoundary(allFaces);

	// 3. 사용 여부 추적을 위한 Set 초기화 (faceMap 역할 대체)
   //    outerFaces에 속한 면 중에서 사용된 면을 추적합니다.
	std::set<const A3DTopoFace *> consumedFaces;

	bool primitiveInserted = false;

	// 1. Face를 순회하며 처리.
	for (A3DUns32 i = 0; i < shellData.m_uiFaceSize; ++i) {
		auto face = shellData.m_ppFaces[i];

		// 이미 소비된 면은 건너뜁니다.
		if (consumedFaces.count(face)) {
			continue;
		}

		// AssemblePrimitive는 이 Face를 시드로 Primitive를 조립합니다.
		// NOTE: AssemblePrimitive 함수 시그니처에서 faceMap을 제거해야 합니다.
		if (AssemblePrimitive(face, amTemplate, consumedFaces)) { // <-- consumedFaces, outerFaces 전달
			primitiveInserted = true;
		}
	}

	// 5. 최종 결과 반환
	return primitiveInserted || true;
}

std::set<const A3DTopoFace *> AM::DatalConverterImpl::processOuterFaceBoundary(
	const std::vector<const A3DTopoFace *> & allShellFaces) noexcept
{
	std::set<const A3DTopoFace *> outerFaces;
	if (allShellFaces.empty()) {
		return outerFaces;
	}

	// 1. 순회 시작점(Seed Face) 선택 및 Queue 초기화
	const A3DTopoFace * seedFace = allShellFaces[0];
	std::set<const A3DTopoFace *> visited;
	std::queue<const A3DTopoFace *> faceQueue;

	faceQueue.push(seedFace);
	visited.insert(seedFace);

	// 2. BFS (Breadth-First Search) 순회 시작
	while (!faceQueue.empty()) {
		const A3DTopoFace * currentFace = faceQueue.front();
		faceQueue.pop();

		outerFaces.insert(currentFace);

		H3DX::TopoFaceDataGuard faceDataGuard(currentFace);
		if (!faceDataGuard.IsValid()) {
			continue;
		}
		const auto & faceData = faceDataGuard.Data();

		// 3. Outter loop를 순회하면서 이웃 Face 탐색
		// 3.1. 인덱스 유효성 검사
		if (faceData.m_uiOuterLoopIndex == A3D_LOOP_UNKNOWN_OUTER_INDEX ||
			faceData.m_uiOuterLoopIndex >= faceData.m_uiLoopSize) {
			// 외부 루프 정보가 없거나 유효하지 않으므로, 이 면은 여기서 처리를 종료합니다.
			continue;
		}

		// 3.2. Outer Loop만 이웃 Face 탐색
		H3DX::TopoLoopDataGuard loopDataGuard(faceData.m_ppLoops[faceData.m_uiOuterLoopIndex]);
		if (!loopDataGuard.IsValid()) {
			continue;
		}

		const auto & loopData = loopDataGuard.Data();

		// 4. CoEdge 순회: 이웃 Face (Twin Face) 찾기
		for (A3DUns32 j = 0; j < loopData.m_uiCoEdgeSize; ++j) {
			const A3DTopoCoEdge * ce = loopData.m_ppCoEdges[j];
			if (!ce) continue;

			// *************************************************************************
			// 핵심 로직: Edge 연결성을 따라 다음 Face를 찾습니다.
			// *************************************************************************

			// 4.1. 트윈 CoEdge가 속한 Face를 역추적
			const A3DTopoFace * nextFace = FindNeighborFace(ce, allShellFaces); // <-- 헬퍼 함수 호출

			// *************************************************************************

			if (!nextFace) continue;

			// 5. 다음 면이 아직 방문되지 않았고, Shell의 멤버라면 Queue에 추가
			if (visited.find(nextFace) == visited.end()) {
				// NOTE: nextFace가 allShellFaces에 속하는지 확인하는 추가 검사가 필요할 수 있으나, 
				//       B-Rep 가정상 쉘 내부의 면이라고 가정합니다.
				visited.insert(nextFace);
				faceQueue.push(nextFace);
			}
		}
	}

	return outerFaces;
}

// allShellFaces는 ProcessOuterFaceBoundary에서 사용된 쉘의 전체 Face 목록.
const A3DTopoFace * AM::DatalConverterImpl::FindNeighborFace(const A3DTopoCoEdge * coEdge, const std::vector<const A3DTopoFace *> & allShellFaces) noexcept
{
	if (nullptr == coEdge) {
		return nullptr;
	}

	H3DX::TopoCoEdgeDataGuard coEdgeDataGuard(coEdge);
	if (!coEdgeDataGuard.IsValid()) {
		return nullptr;
	}
	const auto & coEdgeData = coEdgeDataGuard.Data(); // A3DTopoCoEdgeData 획득

	// 1. Twin CoEdge (m_pNeighbor) 객체 획득
	const A3DTopoCoEdge * neighborCoEdge = coEdgeData.m_pNeighbor;
	if (nullptr == neighborCoEdge) {
		return nullptr; // 트윈 CoEdge 없음
	}

	// 2. Twin CoEdge를 포함하는 Face를 Shell 전체 목록에서 검색
	for (const A3DTopoFace * face : allShellFaces) {
		H3DX::TopoFaceDataGuard faceDataGuard(face);
		if (!faceDataGuard.IsValid()) continue;
		const auto & faceData = faceDataGuard.Data(); // A3DTopoFaceData

		// Face의 모든 Loop를 순회
		for (A3DUns32 i = 0; i < faceData.m_uiLoopSize; ++i) {
			H3DX::TopoLoopDataGuard loopDataGuard(faceData.m_ppLoops[i]);
			if (!loopDataGuard.IsValid()) continue;
			const auto & loopData = loopDataGuard.Data(); // A3DTopoLoopData

			// Loop의 모든 CoEdge를 순회
			for (A3DUns32 j = 0; j < loopData.m_uiCoEdgeSize; ++j) {
				// 현재 순회 중인 CoEdge가 우리가 찾는 Twin CoEdge와 일치하는지 확인
				if (loopData.m_ppCoEdges[j] == neighborCoEdge) {
					return face; // Twin CoEdge를 포함하는 Face를 찾음.
				}
			}
		}
	}

	return nullptr; // Twin CoEdge를 포함하는 Face를 찾지 못함
}

const A3DTopoFace * AM::DatalConverterImpl::FindNeighborFace(const A3DTopoCoEdge * coEdge, const std::set<const A3DTopoFace *> & faces) noexcept
{
	if (nullptr == coEdge) {
		return nullptr;
	}

	H3DX::TopoCoEdgeDataGuard coEdgeDataGuard(coEdge);
	if (!coEdgeDataGuard.IsValid()) {
		return nullptr;
	}
	const auto & coEdgeData = coEdgeDataGuard.Data(); // A3DTopoCoEdgeData 획득

	// 1. Neighbor CoEdge (m_pNeighbor) 객체 획득
	const A3DTopoCoEdge * neighborCoEdge = coEdgeData.m_pNeighbor;
	if (nullptr == neighborCoEdge) {
		return nullptr; // 이웃 CoEdge 없음
	}

	// 2. Neighbor CoEdge를 포함하는 Face를 주어진 Face 목록에서 검색
	//    Set을 순회하는 것은 Vector를 순회하는 것과 구조는 동일하나, Set에 없는 Face는 제외됨.
	for (const A3DTopoFace * face : faces) { // <-- Set 순회
		H3DX::TopoFaceDataGuard faceDataGuard(face);
		if (!faceDataGuard.IsValid()) continue;
		const auto & faceData = faceDataGuard.Data(); // A3DTopoFaceData

		// Face의 모든 Loop를 순회
		for (A3DUns32 i = 0; i < faceData.m_uiLoopSize; ++i) {
			H3DX::TopoLoopDataGuard loopDataGuard(faceData.m_ppLoops[i]);
			if (!loopDataGuard.IsValid()) continue;
			const auto & loopData = loopDataGuard.Data(); // A3DTopoLoopData

			// Loop의 모든 CoEdge를 순회
			for (A3DUns32 j = 0; j < loopData.m_uiCoEdgeSize; ++j) {
				// 현재 순회 중인 CoEdge가 우리가 찾는 Neighbor CoEdge와 일치하는지 확인
				if (loopData.m_ppCoEdges[j] == neighborCoEdge) {
					return face; // 이웃 CoEdge를 포함하는 Face를 찾음.
				}
			}
		}
	}

	return nullptr; // 이웃 Face를 찾지 못함
}

bool AM::DatalConverterImpl::AssemblePrimitive(const A3DTopoFace * face, AM::Template & amTemplate, std::set<const A3DTopoFace *> & consumedFaces) noexcept
{
	if (nullptr == face) {
		return false;
	}

	H3DX::TopoFaceDataGuard dataGuard(face);
	if (!dataGuard.IsValid()) {
		return false;
	}

	const auto & faceData = dataGuard.Data();

	if (nullptr == faceData.m_pSurface) {
		DEBUG_STOP;
		return false;
	}

	// NOTE: kLenTol, kAngTol 등의 공차는 필요 시 정의되거나 전달됩니다.
	constexpr double kLenTol = 1e-4;
	constexpr double kAngTol = 1e-5;

	// 1. Face 분석 (FaceAnalysisResult 생성)
	FaceAnalysisResult fs{};
	if (!AnalyzeFaceSurface(face, fs, kLenTol, kAngTol)) {
		return false; // 분석 실패
	}

	// Surface Type check
	A3DEEntityType eType = kA3DTypeUnknown;
	if (A3D_SUCCESS != A3DEntityGetType(faceData.m_pSurface, &eType)) {
		return false;
	}

	// 2. Primitive 타입에 따른 분기 및 조립 시도
	switch (eType)
	{
		case kA3DTypeSurfPlane:
			// Plane 처리: Box 조립 또는 Extrusion 처리 시작
			// 이 Face가 BOX를 구성하는 면이라면, Box 조립 함수를 호출합니다.

			// NOTE: Box 조립은 6면 전체를 대상으로 하므로, Box 조립 함수가 
			//       Shell의 모든 Outer Face를 대상으로 시도하는 것이 더 적절합니다.

			// (여기서는 추후 Box 조립 로직이 이 분기에 추가된다고 가정합니다.)

			break;

		case kA3DTypeSurfCylinder:
			// Cylinder Lateral Face 판정: 이 면을 시드로 Cylinder 완성 시도
			// assembleCylinder 함수가 양쪽 캡을 찾고 객체를 생성합니다.
			if (true == assembleCylinder(face, amTemplate, consumedFaces, kLenTol, kAngTol)) {
				// Cylinder 생성 및 객체 삽입 성공
				return true;
			}
			break;

		case kA3DTypeSurfSphere:
			//if (true == assembleSphere(face, amTemplate, consumedFaces, outerFaces, kLenTol, kAngTol)) 
			{
				// Cylinder 생성 및 객체 삽입 성공
				return true;
			}
			break;

			// TODO: Torus, Sphere 등 Surface Type 분기 추가
	}

	return false; // 프리미티브를 찾지 못함
}

bool AM::DatalConverterImpl::AssemblePrimitive(const A3DTopoFace * face, AM::Template & amTemplate, std::set<const A3DTopoFace *> & consumedFaces,	const std::set<const A3DTopoFace *> & outerFaces) noexcept
{
	if (nullptr == face) {
		return false;
	}

	H3DX::TopoFaceDataGuard dataGuard(face);
	if (!dataGuard.IsValid()) {
		return false;
	}

	const auto & faceData = dataGuard.Data();

	if (nullptr == faceData.m_pSurface) {
		DEBUG_STOP;
		return false;
	}

	// NOTE: kLenTol, kAngTol 등의 공차는 필요 시 정의되거나 전달됩니다.
	constexpr double kLenTol = 1e-4;
	constexpr double kAngTol = 1e-5;

	// 1. Face 분석 (FaceAnalysisResult 생성)
	FaceAnalysisResult fs{};
	if (!AnalyzeFaceSurface(face, fs, kLenTol, kAngTol)) {
		return false; // 분석 실패
	}

	// Surface Type check
	A3DEEntityType eType = kA3DTypeUnknown;
	if (A3D_SUCCESS != A3DEntityGetType(faceData.m_pSurface, &eType)) {
		return false;
	}

	// 2. Primitive 타입에 따른 분기 및 조립 시도
	switch (eType)
	{
		case kA3DTypeSurfPlane:
			// Plane 처리: Box 조립 또는 Extrusion 처리 시작
			// 이 Face가 BOX를 구성하는 면이라면, Box 조립 함수를 호출합니다.

			// NOTE: Box 조립은 6면 전체를 대상으로 하므로, Box 조립 함수가 
			//       Shell의 모든 Outer Face를 대상으로 시도하는 것이 더 적절합니다.

			// (여기서는 추후 Box 조립 로직이 이 분기에 추가된다고 가정합니다.)

			break;

		case kA3DTypeSurfCylinder:
			// Cylinder Lateral Face 판정: 이 면을 시드로 Cylinder 완성 시도
			// assembleCylinder 함수가 양쪽 캡을 찾고 객체를 생성합니다.
			if (true == assembleCylinder(face, amTemplate, consumedFaces, kLenTol, kAngTol)) {
				// Cylinder 생성 및 객체 삽입 성공
				return true;
			}
			break;

		case kA3DTypeSurfSphere:
			if (true == assembleSphere(face, amTemplate, consumedFaces, outerFaces, kLenTol, kAngTol)) {
				// Cylinder 생성 및 객체 삽입 성공
				return true;
			}
			break;

			// TODO: Torus, Sphere 등 Surface Type 분기 추가
	}

	return false; // 프리미티브를 찾지 못함
}

// Assembly Cylinder
bool AM::DatalConverterImpl::assemblePlane(const A3DTopoFace * face, AM::Template & amTemplate, std::unordered_map<const A3DTopoFace *, bool> & faceMap, double lenTol, double angTol) noexcept
{
	if (nullptr == face) {
		return false;
	}

	// 1. Face의 정보 분석 및 유효성 검사
	FaceAnalysisResult surfaceAnalyzeResult;
	if (!AnalyzeFaceSurface(face, surfaceAnalyzeResult, lenTol, angTol)) {
		return false;
	}

	// 2. Face가 원통형(Cylinder) Surface인지 최종 확인
	if (surfaceAnalyzeResult.kind != SurfKind::Cylinder) {
		return false;
	}

	return true;
}

// Assembly Cylinder
bool AM::DatalConverterImpl::assembleCylinder(const A3DTopoFace * face, AM::Template & amTemplate, std::set<const A3DTopoFace *> & consumedFaces, double lenTol, double angTol) noexcept
{
	if (nullptr == face) {
		return false; // 면 포인터가 null이면 실패 반환
	}

	// 1. Lateral Face의 정보 분석 및 유효성 검사
	FaceAnalysisResult fsLateral; // Face에 대한 정보 수집
	if (false == AnalyzeFaceSurface(face, fsLateral, lenTol, angTol)) {
		return false; // 분석 실패 시 반환
	}

	// Lateral Face가 원통형(Cylinder) Surface인지 확인
	if (fsLateral.kind != SurfKind::Cylinder) {
		return false; // Cylinder Surface가 아니면 실패
	}

	const H3DF::DVector cylinderAxis = fsLateral.cylinder.Axis(); // Cylinder 축 벡터 획득
	const Scalar cylinderRadius = fsLateral.cylinder.m_radius; // Cylinder 반지름 획득

	// 2. 캡(Cap) 후보 찾기 및 Edge 순회 준비
	std::vector<const A3DTopoFace *> capFaceCandidates; // 캡 Face 후보 리스트
	std::vector<CircleParameter> capCircles; // 캡 Circle 파라미터 리스트

	H3DX::TopoFaceDataGuard lateralFaceDataGuard(face);
	if (!lateralFaceDataGuard.IsValid()) return false; // Face 데이터 유효성 확인
	const auto & lateralFaceData = lateralFaceDataGuard.Data();


	std::vector<A3DCrvBase *> circleCurves; // Lateral Face의 Edge에서 찾은 Circle 커브 리스트

	// 2. Lateral Face의 Edge를 Circle이 있는지 확인

	// 2.1 Circle 2개로 구성된 Cylinder를 찾는다.
	for (A3DUns32 loopIndex = 0; loopIndex < lateralFaceData.m_uiLoopSize; ++loopIndex) {
		H3DX::TopoLoopDataGuard loopDataGuard(lateralFaceData.m_ppLoops[loopIndex]);

		if (false == loopDataGuard.IsValid()) {
			continue; // 루프 데이터 유효성 확인
		}

		const auto & loopData = loopDataGuard.Data();

		// 루프의 모든 CoEdge를 순회하면서 Circle을 찾는다.
		for (A3DUns32 coEdgeIndex = 0; coEdgeIndex < loopData.m_uiCoEdgeSize; ++coEdgeIndex) {
			const A3DTopoCoEdge * coEdge = loopData.m_ppCoEdges[coEdgeIndex];
			if (!coEdge) {
				continue;
			}

			H3DX::TopoCoEdgeDataGuard coEdgeDataGuard(coEdge);
			if (!coEdgeDataGuard.IsValid()) { // CoEdge 데이터 유효성 확인
				continue;
			}
			const auto & coEdgeData = coEdgeDataGuard.Data();

			const A3DTopoEdge * edge = coEdgeData.m_pEdge;
			if (nullptr == edge) {
				continue; // Edge 포인터 확인
			}

			H3DX::TopoEdgeDataGuard edgeDataGuard(edge);
			if (!edgeDataGuard.IsValid()) {
				continue; // Edge 데이터 유효성 확인
			}

			const auto & edgeData = edgeDataGuard.Data(); // A3DTopoEdgeData 획득
			if (nullptr == edgeData.m_p3dCurve) {
				continue; // 3D 커브가 없는 경우 건너뜀
			}

			// Edge Curve 타입 검사: 원형 커브인지 확인
			A3DEEntityType curveType = kA3DTypeUnknown;
			if (A3D_SUCCESS != A3DEntityGetType(edgeData.m_p3dCurve, &curveType)) continue;

			if (curveType == kA3DTypeCrvCircle) {

/*
				// A. Circle 커브 데이터 추출
				CircleParameter currentCircle;
				if (ExtractCircleParameter(edgeData.m_p3dCurve, currentCircle) != Result::Ok()) continue;

				// B. 이웃 Face 찾기
				const A3DTopoFace * nextFace = FindNeighborFace(coEdge, outerFaces); // outerFaces에서 이웃 Face 찾음
				if (!nextFace) continue;

				// C. Cap 유효성 검사 (평면, 반지름, 축 일치 여부)
				FaceAnalysisResult fsCap;
				if (!AnalyzeFaceSurface(nextFace, fsCap, lenTol, angTol)) continue;
				if (fsCap.kind != SurfKind::Plane && fsCap.kind != SurfKind::NurbsPlane) continue;

				// D. 기하학적 매칭 검사
				if (IsCapForCylinder(currentCircle, fsLateral.cylinder, static_cast<Scalar>(lenTol), static_cast<Scalar>(angTol))) {
					capFaceCandidates.push_back(nextFace);
					capCircles.push_back(currentCircle);
				}
*/

				CircleParameter currentCircle;
				if (ExtractCircleParameter(edgeData.m_p3dCurve, currentCircle) != Result::Ok()) continue;

				capCircles.push_back(currentCircle);				// Circle 커브를 리스트에 추가
				circleCurves.push_back(edgeData.m_p3dCurve);
			}
		}
	}

	/*
	// 2.1. Lateral Face의 모든 루프(Edge)를 순회합니다.
	for (A3DUns32 i = 0; i < lateralFaceData.m_uiLoopSize; ++i) {
		H3DX::TopoLoopDataGuard loopDataGuard(lateralFaceData.m_ppLoops[i]);
		if (!loopDataGuard.IsValid()) continue; // 루프 데이터 유효성 확인
		const auto & loopData = loopDataGuard.Data();

		for (A3DUns32 j = 0; j < loopData.m_uiCoEdgeSize; ++j) {
			const A3DTopoCoEdge * ce = loopData.m_ppCoEdges[j];
			if (!ce) continue; // CoEdge 포인터 유효성 확인

			H3DX::TopoCoEdgeDataGuard coEdgeDataGuard(ce);
			if (!coEdgeDataGuard.IsValid()) continue; // CoEdge 데이터 유효성 확인
			const auto & coEdgeData = coEdgeDataGuard.Data();

			// 2.2. Neighbor CoEdge (다음 면 후보) 찾기
			const A3DTopoCoEdge * neighborCoEdge = coEdgeData.m_pNeighbor; // m_pNeighbor 사용
			if (!neighborCoEdge) continue; // 이웃 CoEdge 없음

			// A3D SDK 토폴로지 구조를 기반으로 Neighbor CoEdge가 속한 다음 면을 추출합니다.
			const A3DTopoFace * nextFace = FindNeighborFace(neighborCoEdge, outerFaces);// A3DTopoCoEdge::m_pFace 가정
			if (!nextFace) continue; // 다음 Face 포인터 유효성 확인

			// 이미 사용된 면은 건너뛰어 중복 처리 방지
			if (consumedFaces.count(nextFace)) continue; // 이미 소비되었으면 건너뜀

			// 다음 면이 Outer Face 집합에 포함되는지 확인 (Inner Face 무시)
			if (outerFaces.count(nextFace) == 0) continue; // Outer Face가 아니면 건너뜀


			// 2.3. 캡(Cap) 유효성 검사
			FaceAnalysisResult fsCap;
			if (!AnalyzeFaceSurface(nextFace, fsCap, lenTol, angTol)) continue; // 캡 후보 분석

			// 캡은 평면이어야 함
			if (fsCap.kind != SurfKind::Plane && fsCap.kind != SurfKind::NurbsPlane) continue; // 평면이 아니면 건너뜀

			// Cap Face에서 Cylinder Axis와 동일한 반지름의 원(Circle)을 찾음
			for (const auto & circle : fsCap.allCircles) {
				// IsCapForCylinder는 Cap과 Lateral Cylinder의 기하학적 매칭을 검사합니다.
				if (IsCapForCylinder(circle, fsLateral.cylinder, static_cast<Scalar>(lenTol), static_cast<Scalar>(angTol))) {

					capFaceCandidates.push_back(nextFace); // 유효한 캡 Face 추가
					capCircles.push_back(circle); // 유효한 Circle Parameter 추가
					goto next_cap_search; // 이 Cap Face에 대한 Edge 검사 중단
				}
			}
		}
	next_cap_search:;
	}*/

	// 3. Cylinder 완성 조건 검사: 캡이 정확히 2개인지 확인
	if (capCircles.size() != 2) return false; // 캡 개수 불일치 시 조립 실패 반환


	// 4. 높이 계산 및 최종 Solid Cylinder 조립

	const CircleParameter & cap0_circle = capCircles[0]; // 첫 번째 캡 원점
	const CircleParameter & cap1_circle = capCircles[1]; // 두 번째 캡 원점

	// Cap 사이의 벡터 획득 및 높이 계산
	const H3DF::DVector vecBetweenCaps = cap1_circle.m_origin - cap0_circle.m_origin;
	double height = std::abs(static_cast<double>(vecBetweenCaps.Dot(cylinderAxis))); // 높이 계산

	// 높이가 공차 이상인지 확인
	if (height <= lenTol) return false; // 높이 불충분 시 조립 실패 반환


	// 5. Cylinder 객체 생성 및 FaceMap 업데이트

	// Midpoint 계산 (POS)
	H3DF::DPoint midpoint;
	midpoint.x = (cap0_circle.m_origin.x + cap1_circle.m_origin.x) / 2.0;
	midpoint.y = (cap0_circle.m_origin.y + cap1_circle.m_origin.y) / 2.0;
	midpoint.z = (cap0_circle.m_origin.z + cap1_circle.m_origin.z) / 2.0;

	AM::CylinderKit cylinderKit; // Cylinder Kit 생성
	cylinderKit.setPosition(midpoint); // POS 설정
	cylinderKit.setOrientation(cylinderAxis); // ORI 설정
	cylinderKit.setDiameter(cylinderRadius * 2.0); // Diameter 설정
	cylinderKit.setHeight(height); // Height 설정

	amTemplate.insertCylinder(cylinderKit); // Template에 Cylinder 객체 삽입

	// 6. FaceMap 업데이트: 이 Cylinder를 구성하는 면을 사용됨으로 표시
	consumedFaces.insert(face); // Lateral Face 소비
	consumedFaces.insert(capFaceCandidates[0]); // 첫 번째 캡 Face 소비
	consumedFaces.insert(capFaceCandidates[1]); // 두 번째 캡 Face 소비


	return true; // Cylinder 조립 성공 반환
}

// Face에 포함된 Surface의 정보를 분석하여 FaceAnalysisResult에 채워넣음.
bool AM::DatalConverterImpl::AnalyzeFaceSurface(const A3DTopoFace * face, FaceAnalysisResult & out,	double lenTol, double angTol) noexcept
{
	out = FaceAnalysisResult {}; // <-- FaceAnalysisResult 초기화
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
		return false;
	}

	// 1. 표면 타입 분석 및 파라미터 추출
	switch (type) {
		case kA3DTypeSurfPlane: {
			H3DX::SurfPlaneDataGuard planeDataGuard(faceData.m_pSurface);
			if (false == planeDataGuard.IsValid()) {
				DEBUG_STOP;
				return false;
			}
			const auto & planeData = planeDataGuard.Data();

			out.kind = SurfKind::Plane;

			// Plane 파라미터 추출
			out.planeOrigin = Dmi3dx::GetPoint(planeData.m_sTrsf.m_sOrigin) * EXCHAGE_SCALE;

			DVector xAxis = Dmi3dx::GetVector(planeData.m_sTrsf.m_sXVector);
			DVector yAxis = Dmi3dx::GetVector(planeData.m_sTrsf.m_sYVector);

			out.planeNormal = xAxis.Cross(yAxis); // 법선 저장
			out.planeNormal.Normalize();

			// TODO: FaceAnalysisResult에 plane normal 필드가 생기면 out.planeNormal에 대입

			// NOTE: 이제 모든 원 추출 로직은 ExtractOuterCircle이 아닌 
			//       새로운 ExtractAllCircles(face, out.allCircles)가 담당해야 합니다.
		}  break;

		case kA3DTypeSurfCylinder: {
			H3DX::SurfCylinderDataGuard cylinderDataGuard(faceData.m_pSurface);
			if (false == cylinderDataGuard.IsValid()) {
				DEBUG_STOP;
				return false;
			}
			const auto & cylinderData = cylinderDataGuard.Data();

			out.kind = SurfKind::Cylinder;

			// Cylinder 파라미터 추출
			out.cylinder.m_radius = cylinderData.m_dRadius * EXCHAGE_SCALE;
			out.cylinder.m_origin = Dmi3dx::GetPoint(cylinderData.m_sTrsf.m_sOrigin) * EXCHAGE_SCALE;

			DVector xAxis = Dmi3dx::GetVector(cylinderData.m_sTrsf.m_sXVector);
			DVector yAxis = Dmi3dx::GetVector(cylinderData.m_sTrsf.m_sYVector);

			DVector axis = xAxis.Cross(yAxis); // 원통 축 방향
			axis.Normalize();

			BuildXYFromAxis(axis, out.cylinder.m_xAxis, out.cylinder.m_yAxis);
			out.cylinder.Orthonormalize();
		} break;

		case kA3DTypeSurfNurbs: {
			H3DX::SurfNurbsDataGuard nurbsDataGuard(faceData.m_pSurface);
			if (false == nurbsDataGuard.IsValid()) {
				DEBUG_STOP;
				return false;
			}
			const auto & nurbsData = nurbsDataGuard.Data();

			switch (nurbsData.m_eSurfaceForm) {
				case kA3DBSplineSurfaceFormPlane:
					out.kind = SurfKind::NurbsPlane;
					break;

				case kA3DBSplineSurfaceFormCylindrical:
					out.kind = SurfKind::NurbsCylinder;
					break;

				default:
					out.kind = SurfKind::Other;
					break;
			}
		} break;

		default:
			out.kind = SurfKind::Other;
			break;
	}

	return true;
}

bool AM::DatalConverterImpl::assembleSphere(const A3DTopoFace * face, AM::Template & amTemplate, std::set<const A3DTopoFace *> & consumedFaces,  const std::set<const A3DTopoFace *> & outerFaces,  double lenTol, double angTol) noexcept
{
	if (nullptr == face) {
		return false; // 면 포인터가 null이면 실패 반환
	}
/*

	// 1. Face 정보 분석 및 유효성 검사
	FaceAnalysisResult fsSphere;
	if (!AnalyzeFaceSurface(face, fsSphere, lenTol, angTol)) return false; // 분석 실패 시 반환

	// Surface가 Sphere인지 최종 확인
	if (fsSphere.kind != SurfKind::Sphere) return false; // Sphere Surface가 아니면 실패

	// NOTE: FaceAnalysisResult 구조체에 Sphere 파라미터가 저장되어 있다고 가정하고,
	//       여기서는 CylinderParameters 구조를 활용합니다. 

	// 2. Sphere 파라미터 추출
	//    Sphere는 중심(Origin)과 반지름(Radius)만 있으면 됩니다.
	const H3DF::DPoint sphereOrigin = fsSphere.sphere.m_origin; // Sphere 중심 가정
	const Scalar sphereRadius = fsSphere.sphere.m_radius; // Sphere 반지름 가정

	// 3. 닫힌 Solid Sphere 검사 (선택적)
	//    Sphere는 단일 면이므로, 이 면이 쉘 전체를 대표하는지 확인합니다.
	//    만약 이 Face에 Edge가 없다면 (Hole이 없는 단일 구) 닫힌 Sphere입니다.

	H3DX::TopoFaceDataGuard faceDataGuard(face);
	if (!faceDataGuard.IsValid()) return false;
	const auto & faceData = faceDataGuard.Data();

	// 4. 프리미티브 조립 및 객체 삽입

	// CylinderKit을 Box처럼 재사용하거나, 별도의 SphereKit을 사용해야 합니다.
	// 여기서는 SphereKit이 있다고 가정하고 객체를 삽입합니다.

	AM::SphereKit sphereKit; // Sphere Kit 생성 가정
	sphereKit.setPosition(sphereOrigin); // POS 설정
	sphereKit.setDiameter(sphereRadius * 2.0); // Diameter 설정

	amTemplate.insertSphere(sphereKit); // Template에 Sphere 객체 삽입 가정

	// 5. FaceMap 업데이트: 이 Sphere를 구성하는 면을 사용됨으로 표시
	consumedFaces.insert(face); // Sphere Face 소비

	// NOTE: 만약 Sphere가 단일 면으로 된 닫힌 솔리드가 아니라면, 
	//       Loop를 순회하여 Edge를 따라 다른 면(DISH 등)을 찾아야 합니다.
*/

	return true; // Sphere 조립 성공 반환
}

//== 보조 유틸 정의 ==================================================================================

// 두 원(혹은 두 원통)이 같은 축(coaxial) 을 공유하고, 같은 반지름 을 가지는지 판별.
bool AM::DatalConverterImpl::IsCoaxialSameRadius(const CylinderParameters & a, const CylinderParameters & b, Scalar lenTol, Scalar angTolRad) noexcept
{
	// 기본 가드
	if (lenTol < static_cast<Scalar>(0) || angTolRad < static_cast<Scalar>(0)) {
		DEBUG_STOP;
		return false;
	}

	// 축 정규화 후 평행성(역방향 포함) 검사
	const DVector za = a.Axis();
	const DVector zb = b.Axis();

	if (false == ParallelWithinTol(za, zb, angTolRad)) {
		return false;
	}

	// 두 축(같은 직선) 사이의 최소 거리 검사
	const DVector d(static_cast<Scalar>(a.m_origin.x - b.m_origin.x), static_cast<Scalar>(a.m_origin.y - b.m_origin.y), static_cast<Scalar>(a.m_origin.z - b.m_origin.z));

	if (PerpDistance(d, za) > lenTol) {
		return false;
	}

	// 반지름 동일성 검사
	if (false == NearlyEqual(a.m_radius, b.m_radius, lenTol)) {
		return false;
	}

	return true;
}


// 주어진 평면(face)이 특정 원통(face)의 캡(cap) 역할을 하는지 판별.
bool AM::DatalConverterImpl::IsCapForCylinder(const CircleParameter & cap, const CylinderParameters & cy, Scalar lenTol, Scalar angTolRad) noexcept
{
	// 인자 가드
	if (lenTol < static_cast<Scalar>(0) || angTolRad < static_cast<Scalar>(0)) {
		DEBUG_STOP;
		return false;
	}

	// 1. 법선 // 축 (정/역방향 허용)
	const DVector n = cap.m_normal;
	const DVector az = cy.Axis();

	if (false == ParallelWithinTol(n, az, angTolRad)) {
		return false;
	}

	// 2. 원 중심이 축 위에 있는지: 축에 대한 수직거리 검사
	const DVector d(
		static_cast<Scalar>(cap.m_origin.x - cy.m_origin.x),
		static_cast<Scalar>(cap.m_origin.y - cy.m_origin.y),
		static_cast<Scalar>(cap.m_origin.z - cy.m_origin.z));
	if (PerpDistance(d, az) > lenTol) {
		return false;
	}

	// 3. 반지름 동일성
	if (false == NearlyEqual(static_cast<Scalar>(cap.m_radius), cy.m_radius, lenTol)) {
		return false;
	}

	return true;
}

bool AM::DatalConverterImpl::extractAllCircles(const A3DTopoFace * face, std::vector<CircleParameter> & outCircles) noexcept
{
	if (nullptr == face) {
		DEBUG_STOP;
		return false;
	}

	H3DX::TopoFaceDataGuard faceDataGuard(face);
	if (!faceDataGuard.IsValid()) {
		DEBUG_STOP;
		return false;
	}

	const auto & faceData = faceDataGuard.Data();
	bool circleFound = false;

	// 1. 모든 루프(Loop) 순회 (Outer Loop + Inner Loops)
	for (A3DUns32 i = 0; i < faceData.m_uiLoopSize; ++i) {
		H3DX::TopoLoopDataGuard loopDataGuard(faceData.m_ppLoops[i]);
		if (false == loopDataGuard.IsValid()) {
			continue;
		}

		const auto & loopData = loopDataGuard.Data();

		// 2. 루프 내의 CoEdge(모서리) 순회
		for (A3DUns32 j = 0; j < loopData.m_uiCoEdgeSize; ++j) {
			const A3DTopoCoEdge * ce = loopData.m_ppCoEdges[j];
			if (!ce) continue;

			H3DX::TopoCoEdgeDataGuard coEdgeDataGuard(ce);
			if (false == coEdgeDataGuard.IsValid()) {
				continue;
			}

			const auto & coEdgeData = coEdgeDataGuard.Data();

			const A3DTopoEdge * topoEdge = coEdgeData.m_pEdge;
			if (!topoEdge) continue;

			H3DX::TopoEdgeDataGuard edgeDataGuard(topoEdge);
			if (false == edgeDataGuard.IsValid()) {
				continue;
			}

			const auto & edgeData = edgeDataGuard.Data();
			if (nullptr == edgeData.m_p3dCurve) {
				continue;
			}

			A3DEEntityType ct = kA3DTypeUnknown;
			if (A3D_SUCCESS != A3DEntityGetType(edgeData.m_p3dCurve, &ct)) continue;

			// 3. 커브 타입 확인: 원(Circle)인 경우
			if (ct == kA3DTypeCrvCircle) {

				H3DX::CrvCircleDataGuard circleDataGuard(edgeData.m_p3dCurve);
				if (false == circleDataGuard.IsValid()) {
					continue;
				}

				const auto & circleData = circleDataGuard.Data();

				CircleParameter outCircle {};

				// 파라미터 추출
				outCircle.m_radius = circleData.m_dRadius * EXCHAGE_SCALE;
				outCircle.m_origin = Dmi3dx::GetPoint(circleData.m_sTrsf.m_sOrigin) * EXCHAGE_SCALE;

				H3DF::DVector xAxis = Dmi3dx::GetVector(circleData.m_sTrsf.m_sXVector);
				H3DF::DVector yAxis = Dmi3dx::GetVector(circleData.m_sTrsf.m_sYVector);

				H3DF::DVector normal = xAxis.Cross(yAxis);
				normal.Normalize();
				outCircle.m_normal = normal;

				// 4. 추출된 원을 결과 벡터에 추가
				outCircles.push_back(std::move(outCircle));
				circleFound = true;

				// NOTE: 원은 루프의 경계를 대표하는 단일 커브이므로, 
				//       루프 내에서 원을 찾았다면 해당 루프의 나머지 CoEdge는 건너뛰어도 됩니다.
				break;
			}
			// TODO: NURBS 원 판정 로직 추가 가능
		}
	}
	return circleFound;
}

//== Utility 함수 ===================================================================================

// 1. Circle 커브에서 파라미터 추출
Result AM::DatalConverterImpl::ExtractCircleParameter(const A3DCrvBase * curve, CircleParameter & circleParameter) noexcept
{
	if (nullptr == curve) {
		return Result::Fail(Error::InvalidArgument, "Curve is null."); // 커브 포인터가 null이면 실패 반환
	}

	A3DEEntityType curveType = kA3DTypeUnknown;
	if (A3D_SUCCESS != A3DEntityGetType(curve, &curveType)) {
		return Result::Fail(Error::AnalysisFailed, "Failed to get curve type."); // 커브 타입 획득 실패 반환
	}

	if (curveType != kA3DTypeCrvCircle) {
		return Result::Fail(Error::InvalidArgument, "Curve is not a Circle."); // Circle 타입이 아니면 실패 반환
	}

	H3DX::CrvCircleDataGuard circleDataGuard(curve);
	if (false == circleDataGuard.IsValid()) {
		return Result::Fail(Error::AnalysisFailed, "Failed to get circle data."); // Circle 데이터 획득 실패 반환
	}

	const auto & circleData = circleDataGuard.Data(); // A3DCrvCircleData 획득

	// 1. 파라미터 추출 및 스케일 적용 (EXCHAGE_SCALE은 1000이라고 가정)
	circleParameter.m_radius = circleData.m_dRadius * EXCHAGE_SCALE;
	circleParameter.m_origin = Dmi3dx::GetPoint(circleData.m_sTrsf.m_sOrigin) * EXCHAGE_SCALE;

	// 2. 법선 벡터 계산 및 정규화
	H3DF::DVector xAxis = Dmi3dx::GetVector(circleData.m_sTrsf.m_sXVector);
	H3DF::DVector yAxis = Dmi3dx::GetVector(circleData.m_sTrsf.m_sYVector);

	H3DF::DVector normal = xAxis.Cross(yAxis);

	// Normalize()를 사용하여 정규화
	normal.Normalize();
	circleParameter.m_normal = normal;

	return Result::Ok(); // 추출 성공 반환
}

// 루프에서 원 하나 찾기 (circle curve 우선)
// 필요시 NURBS-원 판정 추가 가능.
bool AM::DatalConverterImpl::ExtractOuterCircle(const A3DTopoFace * face, CircleParameter & outCircle) noexcept
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

				outCircle.m_radius = circleData.m_dRadius * EXCHAGE_SCALE;
				outCircle.m_origin = Dmi3dx::GetPoint(circleData.m_sTrsf.m_sOrigin) * EXCHAGE_SCALE;

				DVector xAxis = Dmi3dx::GetVector(circleData.m_sTrsf.m_sXVector);
				DVector yAxis = Dmi3dx::GetVector(circleData.m_sTrsf.m_sYVector);

				DVector normal = xAxis.Cross(yAxis);
				normal.Normalize();
				outCircle.m_normal = normal;

				return true;
			}
			// TODO: NURBS 원 판정 추가 가능
		}
	}
	return false;
}

//== 쉘에서 Solid Cylinder 찾기 ======================================================================

// 보조 함수: 동일 축선 상에 다른 반지름의 실린더가 있는지 확인 (Hollow 필터링 로직)
bool AM::DatalConverterImpl::IsHollowCandidate(const DatalConverterImpl::CylinderParameters & refCyl,
	const std::vector<DatalConverterImpl::FaceAnalysisResult> & allCylinders,
	double lenTol, double angTol) noexcept
{
	// 같은 축(coaxial) 여부만 판단(반지름 무시)하는 헬퍼를 재사용
	// (여기서는 sameAxisWithinTol 함수가 DatalConverterImpl의 protected 멤버라고 가정)
	auto sameAxisWithinTol = [&](const DatalConverterImpl::CylinderParameters & A, const DatalConverterImpl::CylinderParameters & B) -> bool {
		const H3DF::DVector za = A.Axis();
		const H3DF::DVector zb = B.Axis();
		if (!DatalConverterImpl::ParallelWithinTol(za, zb, static_cast<Scalar>(angTol))) {
			return false;
		}
		const H3DF::DVector d(
			static_cast<Scalar>(A.m_origin.x - B.m_origin.x),
			static_cast<Scalar>(A.m_origin.y - B.m_origin.y),
			static_cast<Scalar>(A.m_origin.z - B.m_origin.z)
		);
		return DatalConverterImpl::PerpDistance(d, za) <= static_cast<Scalar>(lenTol);
	};

	for (const auto & fs : allCylinders) {
		if (!sameAxisWithinTol(refCyl, fs.cylinder)) {
			continue;
		}

		// 반지름이 다르면 Hollow 후보로 간주
		const double rdiff = std::abs(static_cast<double>(refCyl.m_radius) -
			static_cast<double>(fs.cylinder.m_radius));
		if (rdiff > lenTol) {
			return true;
		}
	}
	return false;
}

std::vector<DatalConverterImpl::SolidCylinderResult> AM::DatalConverterImpl::DetectSolidCylinder(const A3DTopoShell * shell, double lenTol, double angTol) noexcept
{
	std::vector<SolidCylinderResult> results;

	if (nullptr == shell) {
		return results;
	}

	H3DX::TopoShellDataGuard shellDataGuard(shell);
	if (false == shellDataGuard.IsValid()) {
		return results;
	}

	const auto & shellData = shellDataGuard.Data();

	// 1. Face 요약 수집 (FaceAnalysisResult 사용)
	std::vector<FaceAnalysisResult> planes;
	std::vector<FaceAnalysisResult> cylinders;
	planes.reserve(shellData.m_uiFaceSize);
	cylinders.reserve(shellData.m_uiFaceSize);

	for (A3DUns32 nIndex = 0; nIndex < shellData.m_uiFaceSize; ++nIndex) {
		const A3DTopoFace * face = shellData.m_ppFaces[nIndex];

		FaceAnalysisResult faceAnalysisResult {};
		if (false == AnalyzeFaceSurface(face, faceAnalysisResult, lenTol, angTol)) {
			continue;
		}

		if (faceAnalysisResult.kind == SurfKind::Plane || faceAnalysisResult.kind == SurfKind::NurbsPlane) {
			planes.push_back(std::move(faceAnalysisResult));
		}
		else if (faceAnalysisResult.kind == SurfKind::Cylinder /*|| fs.kind == SurfKind::NurbsCylinder*/) {
			cylinders.push_back(std::move(faceAnalysisResult));
		}
	}

	if (cylinders.empty() || planes.size() < 2) {
		return results;
	}

	// 2. 옆면 군(동축·동반지름)으로 묶기
	struct Group {
		CylinderParameters ref;
		std::vector<const A3DTopoFace *> members;
	};

	std::vector<Group> groups;

	for (auto & cylinder : cylinders) {

		bool placed = false;

		for (auto & group : groups) {
			// IsCoaxialSameRadius는 CylinderParam을 사용하여 동축/동반지름 여부를 검사합니다.
			if (IsCoaxialSameRadius(group.ref, cylinder.cylinder, static_cast<Scalar>(lenTol), static_cast<Scalar>(angTol))) {
				group.members.push_back(cylinder.face);
				placed = true;
				break;
			}
		}

		if (false == placed) {
			Group newGroup { cylinder.cylinder, { cylinder.face } };
			groups.push_back(std::move(newGroup));
		}
	}

	if (groups.empty()) {
		return results;
	}

	// 같은 축(coaxial) 여부만 판단(반지름 무시)하는 헬퍼 (Hollow 검사에도 사용됨)
	auto sameAxisWithinTol = [&](const CylinderParameters & A, const CylinderParameters & B) -> bool {
		const H3DF::DVector za = A.Axis();
		const H3DF::DVector zb = B.Axis();
		if (!ParallelWithinTol(za, zb, static_cast<Scalar>(angTol))) {
			return false;
		}
		const H3DF::DVector d(
			static_cast<Scalar>(A.m_origin.x - B.m_origin.x),
			static_cast<Scalar>(A.m_origin.y - B.m_origin.y),
			static_cast<Scalar>(A.m_origin.z - B.m_origin.z)
		);
		return PerpDistance(d, za) <= static_cast<Scalar>(lenTol);
	};


	// 3. 각 그룹에 대해 평면 캡 2개 찾기 (솔리드 판별 루프)
	for (auto & group : groups) {

		std::vector<const CircleParameter *> caps; // <-- 캡 후보 원 포인터 리스트
		caps.reserve(8); // 넉넉하게 예약

		for (auto & planeFace : planes) {

			// planeFace 내의 모든 원(allCircles)을 순회하며 캡인지 검사합니다.
			for (const auto & circle : planeFace.allCircles) { // <-- 모든 원 순회

				// IsCapForCylinder는 원(circle)이 그룹의 실린더와 동축/동반지름인지 검사
				if (IsCapForCylinder(circle, group.ref, static_cast<Scalar>(lenTol), static_cast<Scalar>(angTol))) {
					// CircleParameter의 주소(const CircleParameter *)를 캡 후보 리스트에 추가
					caps.push_back(&circle);
				}
			}
		}

		if (caps.size() < 2) {
			continue;
		}

		// 서로 다른 높이의 두 캡 선택(축 방향 거리 최대 페어)
		const H3DF::DVector axis = group.ref.Axis();

		// dotAxis 람다 수정: cap은 이제 CircleParameter * 타입입니다.
		const auto dotAxis = [&](const CircleParameter * a, const CircleParameter * b)->double {
			H3DF::DVector ca(a->m_origin.x, a->m_origin.y, a->m_origin.z);
			H3DF::DVector cb(b->m_origin.x, b->m_origin.y, b->m_origin.z);
			H3DF::DVector d(cb.x - ca.x, cb.y - ca.y, cb.z - ca.z);
			return std::abs(static_cast<double>(d.Dot(axis)));
		};

		const CircleParameter * cap0_circle = nullptr; // 캡 원 객체 포인터
		const CircleParameter * cap1_circle = nullptr;
		double bestH = 0.0;

		for (size_t i = 0; i < caps.size(); ++i) {
			for (size_t j = i + 1; j < caps.size(); ++j) {
				double h = dotAxis(caps[i], caps[j]); // CircleParameter*을 사용
				if (h > bestH && h > lenTol) {
					bestH = h;
					cap0_circle = caps[i];
					cap1_circle = caps[j];
				}
			}
		}

		if (!cap0_circle || !cap1_circle) {
			continue;
		}

		// 4. Cylinder 중간점 계산 (POS)
		// 두 캡의 중심을 평균하여 실린더의 중간점(Midpoint)을 계산합니다.
		H3DF::DPoint cap0_origin = cap0_circle->m_origin;
		H3DF::DPoint cap1_origin = cap1_circle->m_origin;

		H3DF::DPoint midpoint;
		midpoint.x = (cap0_origin.x + cap1_origin.x) / 2.0;
		midpoint.y = (cap0_origin.y + cap1_origin.y) / 2.0;
		midpoint.z = (cap0_origin.z + cap1_origin.z) / 2.0;

		// Hollow 케이스 필터링 로직을 분리된 함수로 대체
		// IsHollowCandidate는 동일 축선 상에 다른 반지름의 실린더가 있는지 검사합니다.
// 		if (IsHollowCandidate(group.ref, cylinders, lenTol, angTol)) {
// 			continue; // 솔리드가 아님
// 		}

		// 솔리드 확정: 결과 벡터에 추가
		SolidCylinderResult res {};
		res.isSolidCylinder = true;
		res.cyl = group.ref;
		// res.cyl.m_origin = midpoint;
		res.height = bestH;

		results.push_back(std::move(res));
	}

	return results; // 발견된 모든 솔리드 리스트 반환
}