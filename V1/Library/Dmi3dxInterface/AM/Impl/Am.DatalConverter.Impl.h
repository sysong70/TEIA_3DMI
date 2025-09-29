#pragma once

#include <3DF/Math.h>

#include "../../3DX.h"

#include "../AM.h"

#include <memory>
#include <string>
#include <string_view>
#include <fstream>
#include <filesystem>
#include <vector>
#include <optional>
#include <cstdint>
#include <cmath>
#include <iosfwd>
#include <set>
#include <unordered_map>

namespace AM
{
	class DatalWriter
	{
	public:
		DatalWriter();
		~DatalWriter();

		DatalWriter(const DatalWriter &) = delete;
		DatalWriter & operator=(const DatalWriter &) = delete;

		// 출력 타깃 설정 (ofsteam 등 외부에서 열어 전달)
		[[nodiscard]] Result Bind(std::ostream * os) noexcept;
		[[nodiscard]] bool IsOpen() const noexcept;

		// 상위 개체 (Equipment) 블록
		[[nodiscard]] Result BeginEquipment(std::string_view equipmentName) noexcept;
		[[nodiscard]] Result EndEquipment() noexcept;

	private:
		struct Impl;
		std::unique_ptr<Impl> m_impl;
	};
}

namespace AM
{
	using Scalar = double;

	using FaceList = std::vector<const A3DTopoFace *>;
	using CombinationList = std::vector<FaceList>;


	class DatalConverterImpl
	{
	public:
		DatalConverterImpl();

	protected:
		bool ProcessTopoConnex(const A3DTopoConnex * connex, AM::Equipment & equipment) noexcept;
		bool ProcessTopoShell(const A3DTopoShell * shell, AM::Template & templ) noexcept;

		std::set<const A3DTopoFace *> processOuterFaceBoundary(const std::vector<const A3DTopoFace *> & allShellFaces) noexcept;
		const A3DTopoFace * FindNeighborFace(const A3DTopoCoEdge * ce, const std::vector<const A3DTopoFace *> & allShellFaces) noexcept;
		const A3DTopoFace * FindNeighborFace(const A3DTopoCoEdge * coEdge, const std::set<const A3DTopoFace *> & faces) noexcept;

		bool AssemblePrimitive(const A3DTopoFace * face, AM::Template & amTemplate, std::set<const A3DTopoFace *> & consumedFaces) noexcept;
		bool AssemblePrimitive(const A3DTopoFace * face, AM::Template & amTemplate, std::set<const A3DTopoFace *> & consumedFaces, const std::set<const A3DTopoFace *> & outerFaces) noexcept;

		bool assemblePlane(const A3DTopoFace * lateralFace, AM::Template & amTemplate, std::unordered_map<const A3DTopoFace *, bool> & faceMap, double lenTol, double angTol) noexcept;
		bool assembleCylinder(const A3DTopoFace * face, AM::Template & amTemplate, std::set<const A3DTopoFace *> & consumedFaces, double lenTol, double angTol) noexcept;
		bool assembleSphere(const A3DTopoFace * face, AM::Template & amTemplate, std::set<const A3DTopoFace *> & consumedFaces, const std::set<const A3DTopoFace *> & outerFaces, double lenTol, double angTol) noexcept;

		//== Solid 분석 =============================================================================

		//----- 공통 유틸: 어디서나 호출 가능하도록 static -----

		// 축(Z)으로부터 안정적인 X/Y 프레임 생성 (Exchange에서 XDir 미제공 시 사용)
		static H3DF_INLINE void BuildXYFromAxis(H3DF::DVector zAxis, H3DF::DVector & xOut, H3DF::DVector & yOut)
		{
			zAxis.Normalize();
			H3DF::DVector t = (std::fabs(zAxis.z) < 0.9f) ? H3DF::DVector(0, 0, 1) : H3DF::DVector(1, 0, 0);
			xOut = t.Cross(zAxis);
			yOut = zAxis.Cross(xOut); // 오른손 좌표계

			xOut.Normalize();
			yOut.Normalize();
		}

		//----- Solid 분석 기초 타입들 -----
		struct CircleParameter {
			H3DF::DPoint  m_origin;   // (cx,cy,cz) → DPoint
			H3DF::DVector m_normal;   // (nx,ny,nz) → DVector (normalized)
			double m_radius = 0;        // 반경(그대로 double 유지)
		};

		enum class SurfKind : uint8_t {
			Plane,
			Cylinder,
			NurbsPlane,
			NurbsCylinder,
			Sphere,
			Other
		};

		// X/Y 프레임 기반 원통 파라미터 (축 Z = X × Y)
		struct CylinderParameters {
			H3DF::DVector m_origin, m_xAxis, m_yAxis;
			Scalar m_radius{};
			bool m_closed{ true };

			H3DF_INLINE H3DF::DVector Axis() const {
				H3DF::DVector zAxis = m_xAxis.Cross(m_yAxis);
				zAxis.Normalize();
				return zAxis;
			}

			H3DF_INLINE void Orthonormalize() {
				m_xAxis.Normalize();
				const Scalar d = static_cast<Scalar>(m_yAxis.Dot(m_xAxis));
				m_yAxis = H3DF::DVector(
					static_cast<Scalar>(m_yAxis.x - d * m_xAxis.x),
					static_cast<Scalar>(m_yAxis.y - d * m_xAxis.y),
					static_cast<Scalar>(m_yAxis.z - d * m_xAxis.z)
				);

				m_yAxis.Normalize();

				H3DF::DVector z = Axis();
				m_yAxis = z.Cross(m_xAxis);
			}
		};

		struct DishParameters {
			Scalar m_diameter{};          // XY 평면에서의 Dish 직경 (Diameter)
			Scalar m_height{};            // Z축에 평행한 Dish의 높이 (Height)
			Scalar m_radius{};            // 너클 반경 (Knuckle radius, Radius)
		};

		struct FaceAnalysisResult {
			const A3DTopoFace * face{};
			SurfKind kind{ SurfKind::Other };

			H3DF::DPoint  planeOrigin;
			H3DF::DVector planeNormal;

			CylinderParameters cylinder;

			DishParameters dish;

			std::vector<CircleParameter> allCircles;

			uint32_t loopCount = 0;
			bool hasInnerLoops = false;
		};

		bool AnalyzeFaceSurface(const A3DTopoFace * face, FaceAnalysisResult & out,
			double lenTol, double angTol) noexcept;

		// ---- Solid Cylinder 결과 & 유틸 ----
		struct SolidCylinderResult {
			bool isSolidCylinder{ false };
			CylinderParameters cyl;
			double height;
			std::vector<const A3DTopoFace *> lateral;
			const A3DTopoFace * caps[2];
		};


		// X/Y/Z 축 방향 및 크기를 포함하는 Box 파라미터 구조체
		struct BoxParameters {
			H3DF::DPoint m_origin;        // POS X Y Z (Box의 중심 좌표)
			H3DF::DVector m_size;         // (XLEN, YLEN, ZLEN)
			H3DF::DVector m_xAxis;        // X 축 방향 벡터 (Normalize 됨)
			H3DF::DVector m_yAxis;        // Y 축 방향 벡터 (Normalize 됨)
			H3DF::DVector m_zAxis;        // Z 축 방향 벡터 (Normalize 됨)
		};

		struct BoxFaceInfo {
			const A3DTopoFace * face;
			H3DF::DVector normal;
			double distToOrigin;
			size_t originalIndex;
		};

		struct BoxBuilderState {
			std::set<const A3DTopoFace *> faces;        // 현재까지 선택된 면 (최종적으로 6개)
			std::vector<H3DF::DVector> axes;           // 현재까지 확정된 직교 축 (최대 3개)
			std::vector<const A3DTopoFace *> boxFaces;  // 순서대로 정렬된 6개의 면
		};

		struct SolidBoxResult {
			bool isSolidBox { false };
			BoxParameters box;

			// 6개의 면 (Face) 포인터: 순서대로 X+, X-, Y+, Y-, Z+, Z- 면을 담을 수 있음.
			const A3DTopoFace * faces[6];
		};

		// 두 스칼라가 공차(tol) 이내로 같은지 검사.
		static H3DF_INLINE bool NearlyEqual(Scalar a, Scalar b, Scalar tol) {
			return std::abs(a - b) <= tol;
		}

		// 두 벡터가 각도 공차(라디안) 이내에서 평행(또는 반평행)인지 검사.
		static H3DF_INLINE bool ParallelWithinTol(H3DF::DVector a, H3DF::DVector b, Scalar angTolRad) 
		{
			// 1. Zero 벡터 가드: 방향이 없으므로 평행 판정은 false
			const Scalar tiny = static_cast<Scalar>(1e-12); // 프로젝트 스케일에 맞춰 조정 가능
			if (a.Length() <= tiny || b.Length() <= tiny) {
				return false;
			}

			// 2. 정규화 후 코사인 임계값 비교
			a = a.Normalize();
			b = b.Normalize();

			const Scalar dot = static_cast<Scalar>(std::abs(a.Dot(b)));
			const Scalar cosTol = static_cast<Scalar>(std::cos(angTolRad));

			return dot >= cosTol;
		}

		// 벡터 a 의 성분 중, 방향 bDir 에 수직인 성분의 길이(수선 거리)를 반환.
		static H3DF_INLINE Scalar PerpDistance(H3DF::DVector a, H3DF::DVector bDir) 
		{
			bDir.Normalize();
			const Scalar proj = static_cast<Scalar>(a.Dot(bDir));
			H3DF::DVector s(
				static_cast<Scalar>(a.x - proj * bDir.x),
				static_cast<Scalar>(a.y - proj * bDir.y),
				static_cast<Scalar>(a.z - proj * bDir.z)
			);
			return static_cast<Scalar>(s.Length());
		}

		// 두 원기둥이 같은 축(coaxial)이며 반지름 또한 동일한지, 공차 내에서 판단.
		bool IsCoaxialSameRadius(const CylinderParameters & a, const CylinderParameters & b,
			Scalar lenTol, Scalar angTolRad) noexcept;

		// 원(cap)이 주어진 원기둥의 엔드캡(끝면)인지 판정.
		bool IsCapForCylinder(const CircleParameter & cap, const CylinderParameters & cy,
			Scalar lenTol, Scalar angTolRad) noexcept;

		bool extractAllCircles(const A3DTopoFace * face, std::vector<CircleParameter> & outCircles) noexcept;

		//== Utility 함수 ===================================================================================
		Result ExtractCircleParameter(const A3DCrvBase * curve, CircleParameter & outCircle) noexcept;

		// 주어진 Face 로부터 바깥 경계(외곽) 원을 추출.
		bool ExtractOuterCircle(const A3DTopoFace * f, CircleParameter & outCircle) noexcept;

		bool IsHollowCandidate(const DatalConverterImpl::CylinderParameters & refCyl,
			const std::vector<DatalConverterImpl::FaceAnalysisResult> & allCylinders,
			double lenTol, double angTol) noexcept;

		std::vector<SolidCylinderResult> DetectSolidCylinder(const A3DTopoShell * shell, double lenTol = 1e-4, double angTol = 1e-5) noexcept;

		//std::vector<SolidBoxResult> DetectSolidBox(const A3DTopoShell * shell, double lenTol = 1e-4, double angTol = 1e-5) noexcept;
	};
}

