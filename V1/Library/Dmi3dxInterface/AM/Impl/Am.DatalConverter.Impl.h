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

	class AmWriter {
	public:
		enum class Status : uint8_t {
			Ok = 0,
			FileOpenFailed,
			AlreadyOpen,
			NotOpen,
			WriteFailed,
			InvalidArgument
		};

		struct Error {
			Status code{ Status::Ok };
			std::string message{};
			explicit operator bool() const noexcept { return code != Status::Ok; }
		};

		enum class Axis : uint8_t  { 
			X, 
			Y, 
			Z 
		};

		struct AxisMap { 
			Axis to{}; 
			int sign{ +1 }; 
		};

		struct Orientation { 
			AxisMap mapY{ Axis::Y, +1 }; 
			AxisMap mapZ{ Axis::Z, +1 }; 
		};

		class BlockGuard {
		public:
			BlockGuard() = default;
			BlockGuard(AmWriter * w, std::string_view name);
			BlockGuard(BlockGuard &&) noexcept;
			BlockGuard & operator=(BlockGuard &&) noexcept;
			~BlockGuard();

			void End();
			bool active() const noexcept { return m_writer != nullptr; }
		private:
			AmWriter * m_writer{ nullptr };
			std::string m_name{};
			bool m_closed{ true };
		};

	public:
		// lifecycle
		AmWriter() = default;
		~AmWriter();
		AmWriter(const AmWriter &) = delete;
		AmWriter & operator=(const AmWriter &) = delete;
		AmWriter(AmWriter &&) noexcept;
		AmWriter & operator=(AmWriter &&) noexcept;

		// file
		Status Open(const std::filesystem::path & path_utf8, bool writeBOM = true) noexcept;
		void Close() noexcept;
		bool IsOpen() const noexcept;

		void Flush() noexcept;

		// input / equipment helpers
		Status BeginInput();
		Status EndInput();            // prints: "INPUT END  EQUIPMENT <name>" if equipment set, else "INPUT END"
		Status FinishInput();         // prints: "INPUT FINISH"
		Status WriteEquipmentHeader(std::string_view equipmentPath,
			bool buil,
			std::string_view dsco = "unset",
			std::string_view ptsp = "unset",
			std::string_view insc = "unset");

		Status StartWithTemplate(
			std::string_view equipmentPath,
			std::string_view desc,
			std::string_view purp = "EQUI",
			std::optional<double> userWeightKg = std::nullopt,
			bool buil = false,
			std::string_view dsco = "unset",
			std::string_view ptsp = "unset",
			std::string_view insc = "unset");

		// template / primitives
		BlockGuard BeginTemplate(std::string_view desc,
			std::string_view purp = "EQUI",
			std::optional<double> userWeightKg = std::nullopt);
		Status End();

		BlockGuard BeginBox(double px, double py, double pz,
			const Orientation & ori,
			double xlen, double ylen, double zlen);

		auto BeginCylinder(double cx, double cy, double cz, double radius, double height, Axis axis = Axis::Z) -> BlockGuard;

		// Overload that applies a yaw (roll) angle around the cylinder axis and optional quantization step (deg)
		auto BeginCylinder(double cx, double cy, double cz, double radius, double height, Axis axis, double yawDeg, double quantStepDeg) -> BlockGuard;

		// Overload: cylinder with explicit ORI and DIAM/HEIG keys
		auto BeginCylinder(double cx, double cy, double cz, double diam, double heig, const Orientation & ori) -> BlockGuard;

		// Oriented solid cylinder (no RAII)
		Status WriteSolidCylinder(double cx, double cy, double cz, const Orientation & ori, double diam, double heig);
		Status WriteSolidCylinder(double cx, double cy, double cz, double nx, double ny, double nz, double radius, double height);

		Status WritePlainCylinder(double cx, double cy, double cz, double diam, double heig);

		// Hole(내경) 실린더 출력용 선언 추가:
		Status WriteHoleCylinder(double cx, double cy, double cz, double nx, double ny, double nz, double radius, double height, double stepDeg = 11.25);

		// emit
		Status EmitLine(std::string_view line);
		Status EmitKV(std::string_view key, std::string_view value);
		Status EmitRawKV(std::string_view key, std::string_view value);
		Status EmitKV(std::string_view key, double value, std::string_view unit = "");

		const Error & LastError() const noexcept;

	private:
		friend class BlockGuard;
		Status beginBlock(std::string_view name);
		Status endBlock(std::string_view name);
		void setError(Status c, std::string msg);
		static std::string axisToStr(Axis a);
		static std::string axisMapStr(const AxisMap & m);
		static std::string formatOriFromAxisAndYaw(Axis axis, int zSign, double yawDeg, double stepDeg);
		static std::string formatOriFromVector(double nx, double ny, double nz, double stepDeg);
		static std::string formatOriYZFromVectors(double yx, double yy, double yz, double zx, double zy, double zz, double stepDeg);
		static double autoYawFromAxisDeg(double nx, double ny, double nz, Axis a) noexcept;
		static double normalizeDeg(double deg) noexcept;
		static double quantizeDeg(double deg, double step) noexcept;
		static std::string formatOrientation(const Orientation & ori);
		static std::string formatNumber(double v, int maxDecimals = 3);
		static Axis pickDominantAxis(double nx, double ny, double nz) noexcept;
		static int pickAxisSign(Axis a, double nx, double ny, double nz) noexcept;
		static Orientation orientationFromAxis(double nx, double ny, double nz) noexcept;

	private:
		std::ofstream m_out{};
		Error m_lastError{};
		std::vector<std::string> m_stack{};
		std::string m_currentEquipment{};
		int m_fd;
	};

	class DatalConverterImpl
	{
	public:
		DatalConverterImpl();

	protected:
		bool ExportTopoConnex(const A3DTopoConnex * connex, AM::Equipment & equipment) noexcept;
		bool ExportTopoShell(const A3DTopoShell * shell, AM::Template & templ) noexcept;
		bool ExportTopoFace(const A3DTopoFace * face) noexcept;

		//== Solid 분석 =============================================================================

		//----- 공통 유틸: 어디서나 호출 가능하도록 static -----
		static H3DF_INLINE H3DF::DVector SafeNormalized(H3DF::DVector v, Scalar eps = Scalar(1e-12f))
		{
			if (v.Length() <= eps) {
				return H3DF::DVector::Zero();
			}

			v.Normalize();
			return v;
		}

		// 축(Z)으로부터 안정적인 X/Y 프레임 생성 (Exchange에서 XDir 미제공 시 사용)
		static H3DF_INLINE void BuildXYFromAxis(H3DF::DVector zAxis, H3DF::DVector & xOut, H3DF::DVector & yOut)
		{
			auto z = SafeNormalized(zAxis);
			H3DF::DVector t = (std::fabs(z.z) < 0.9f) ? H3DF::DVector(0, 0, 1) : H3DF::DVector(1, 0, 0);
			xOut = SafeNormalized(t.Cross(z));
			yOut = SafeNormalized(z.Cross(xOut)); // 오른손 좌표계
		}

		//----- Solid 분석 기초 타입들 -----
		struct CircleParam {
			H3DF::DPoint  origin;   // (cx,cy,cz) → DPoint
			H3DF::DVector normal;   // (nx,ny,nz) → DVector (normalized)
			double r = 0;        // 반경(그대로 double 유지)
		};

		enum class SurfKind : uint8_t {
			Plane,
			Cylinder,
			NurbsPlane,
			NurbsCylinder,
			Other
		};

		// X/Y 프레임 기반 원통 파라미터 (축 Z = X × Y)
		struct CylinderParam {
			H3DF::DVector origin, xAxis, yAxis;
			Scalar radius{};
			bool uClosed{ true };

			H3DF_INLINE H3DF::DVector Axis() const {
				return DatalConverterImpl::SafeNormalized(xAxis.Cross(yAxis));
			}

			H3DF_INLINE void Orthonormalize() {
				xAxis = DatalConverterImpl::SafeNormalized(xAxis);
				const Scalar d = static_cast<Scalar>(yAxis.Dot(xAxis));
				yAxis = H3DF::DVector(
					static_cast<Scalar>(yAxis.x - d * xAxis.x),
					static_cast<Scalar>(yAxis.y - d * xAxis.y),
					static_cast<Scalar>(yAxis.z - d * xAxis.z)
				);
				yAxis = DatalConverterImpl::SafeNormalized(yAxis);
				H3DF::DVector z = Axis();
				yAxis = DatalConverterImpl::SafeNormalized(z.Cross(xAxis));
			}
		};

		struct FaceAnalysisResult {
			const A3DTopoFace * face;
			SurfKind kind{ SurfKind::Other };

			H3DF::DPoint  planeOrigin;
			H3DF::DVector planeNormal;

			CylinderParam cyl;

			bool        outerHasCircle{ false };
			CircleParam outerCircle;

			uint32_t loopCount;
			bool     hasInnerLoops;
		};

		bool AnalyzeFaceSurface(const A3DTopoFace * face, FaceAnalysisResult & out,
			double lenTol, double angTol) noexcept;

		// ---- Solid Cylinder 결과 & 유틸 ----
		struct SolidCylinderResult {
			bool isSolidCylinder{ false };
			CylinderParam cyl;
			double height;
			std::vector<const A3DTopoFace *> lateral;
			const A3DTopoFace * caps[2];
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
			bDir = SafeNormalized(bDir);
			const Scalar proj = static_cast<Scalar>(a.Dot(bDir));
			H3DF::DVector s(
				static_cast<Scalar>(a.x - proj * bDir.x),
				static_cast<Scalar>(a.y - proj * bDir.y),
				static_cast<Scalar>(a.z - proj * bDir.z)
			);
			return static_cast<Scalar>(s.Length());
		}

		// 두 원기둥이 같은 축(coaxial)이며 반지름 또한 동일한지, 공차 내에서 판단.
		bool IsCoaxialSameRadius(const CylinderParam & a, const CylinderParam & b,
			Scalar lenTol, Scalar angTolRad) noexcept;

		// 원(cap)이 주어진 원기둥의 엔드캡(끝면)인지 판정.
		bool IsCapForCylinder(const CircleParam & cap, const CylinderParam & cy,
			Scalar lenTol, Scalar angTolRad) noexcept;

		// 주어진 Face 로부터 바깥 경계(외곽) 원을 추출.
		bool ExtractOuterCircle(const A3DTopoFace * f, CircleParam & outCircle) noexcept;

		bool IsHollowCandidate(const DatalConverterImpl::CylinderParam & refCyl,
			const std::vector<DatalConverterImpl::FaceAnalysisResult> & allCylinders,
			double lenTol, double angTol) noexcept;

		std::optional<SolidCylinderResult> DetectSolidCylinder(const A3DTopoShell * shell, double lenTol = 1e-4, double angTol = 1e-5) noexcept;

	protected:
		//== AM Datal writer =======================================================================
		bool ExportToDatal(const std::filesystem::path & outPathUTF8);
		AmWriter & Writer() noexcept { return m_writer; }
		const AmWriter & Writer() const noexcept { return m_writer; }

	private:
		AmWriter m_writer; // owns the writer instance
	};
}

