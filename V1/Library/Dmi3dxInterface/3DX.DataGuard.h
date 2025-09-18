#pragma once
#include <A3DSDKIncludes.h>

namespace H3DX {

	// ----------------- 공통 가드 -----------------
	template <class Traits>
	class DataGuard
	{
	public:
		using data_type = typename Traits::data_type;
		using object_type = typename Traits::object_type;

		explicit DataGuard(const object_type * obj) noexcept
			: m_isValid(false)
		{
			Traits::Init(m_data);  // ← 매크로는 여기서 “구체 타입”으로만 호출
			if (obj != nullptr) {
				m_status = Traits::Get(obj, &m_data);
				m_isValid = (m_status == A3D_SUCCESS);
			}
		}

		~DataGuard() {
			(void) Traits::Get(nullptr, &m_data); // 항상 정리
		}

		// 복사 금지
		DataGuard(const DataGuard &) = delete;
		DataGuard & operator=(const DataGuard &) = delete;

		// 이동 허용
		DataGuard(DataGuard && other) noexcept
			: m_data(other.m_data), m_isValid(other.m_isValid)
		{
			Traits::Init(other.m_data);
			other.m_isValid = false;
		}

		DataGuard & operator=(DataGuard && other) noexcept {
			if (this != &other) {
				(void) Traits::Get(nullptr, &m_data);
				m_data = other.m_data;
				m_isValid = other.m_isValid;

				Traits::Init(other.m_data);
				other.m_isValid = false;
			}
			return *this;
		}

		[[nodiscard]] bool IsValid() const noexcept { return m_isValid; }
		[[nodiscard]] const data_type & Data() const noexcept { return m_data; }
		[[nodiscard]] data_type & Data()       noexcept { return m_data; }
		[[nodiscard]] A3DStatus Status()       noexcept { return m_status; }

		// 선택: 포인터/참조 연산자
		const data_type * operator->() const noexcept { return &m_data; }
		data_type * operator->()       noexcept { return &m_data; }
		const data_type & operator* () const noexcept { return m_data; }
		data_type & operator* ()       noexcept { return m_data; }

	private:
		data_type m_data{};
		bool m_isValid = false;
		A3DStatus m_status = A3D_ERROR;
	};

	// ----------------- Traits 정의 -----------------
	// 각 Traits에 Init()을 넣어 “구체 타입”으로만 매크로를 호출합니다.
	// 필요 타입만 추가해서 사용.

	struct TopoBrepTraits {
		using data_type = A3DTopoBrepDataData;
		using object_type = A3DTopoBrepData;
		static void Init(data_type & d) { A3D_INITIALIZE_DATA(A3DTopoBrepDataData, d); }
		static A3DStatus Get(const object_type * o, data_type * d) { return A3DTopoBrepDataGet(o, d); }
	};

	struct TopoConnexTraits {
		using data_type = A3DTopoConnexData;
		using object_type = A3DTopoConnex;
		static void Init(data_type & d) { A3D_INITIALIZE_DATA(A3DTopoConnexData, d); }
		static A3DStatus Get(const object_type * o, data_type * d) { return A3DTopoConnexGet(o, d); }
	};

	struct TopoFaceTraits {
		using data_type = A3DTopoFaceData;
		using object_type = A3DTopoFace;
		static void Init(data_type & d) { A3D_INITIALIZE_DATA(A3DTopoFaceData, d); }
		static A3DStatus Get(const object_type * o, data_type * d) { return A3DTopoFaceGet(o, d); }
	};

	struct TopoShellTraits {
		using data_type = A3DTopoShellData;
		using object_type = A3DTopoShell;
		static void Init(data_type & d) { A3D_INITIALIZE_DATA(A3DTopoShellData, d); }
		static A3DStatus Get(const object_type * o, data_type * d) { return A3DTopoShellGet(o, d); }
	};

	struct TopoLoopTraits {
		using data_type = A3DTopoLoopData;
		using object_type = A3DTopoLoop;
		static void Init(data_type & d) { A3D_INITIALIZE_DATA(A3DTopoLoopData, d); }
		static A3DStatus Get(const object_type * o, data_type * d) { return A3DTopoLoopGet(o, d); }
	};

	struct TopoCoEdgeTraits {
		using data_type = A3DTopoCoEdgeData;
		using object_type = A3DTopoCoEdge;
		static void Init(data_type & d) { A3D_INITIALIZE_DATA(A3DTopoCoEdgeData, d); }
		static A3DStatus Get(const object_type * o, data_type * d) { return A3DTopoCoEdgeGet(o, d); }
	};

	struct TopoEdgeTraits {
		using data_type = A3DTopoEdgeData;
		using object_type = A3DTopoEdge;
		static void Init(data_type & d) { A3D_INITIALIZE_DATA(A3DTopoEdgeData, d); }
		static A3DStatus Get(const object_type * o, data_type * d) { return A3DTopoEdgeGet(o, d); }
	};

// 	struct TopoVertexTraits {
// 		using data_type = A3DTopoVertexData;
// 		using object_type = A3DTopoVertex;
// 		static void Init(data_type & d) { A3D_INITIALIZE_DATA(A3DTopoVertexData, d); }
// 		static A3DStatus Get(const object_type * o, data_type * d) { return A3DTopoVertexGet(o, d); }
// 	};

	struct SurfPlaneTraits {
		using data_type = A3DSurfPlaneData;
		using object_type = A3DSurfPlane;
		static void Init(data_type & d) { A3D_INITIALIZE_DATA(A3DSurfPlaneData, d); }
		static A3DStatus Get(const object_type * o, data_type * d) { return A3DSurfPlaneGet(o, d); }
	};

	struct SurfCylinderTraits {
		using data_type = A3DSurfCylinderData;
		using object_type = A3DSurfCylinder;
		static void Init(data_type & d) { A3D_INITIALIZE_DATA(A3DSurfCylinderData, d); }
		static A3DStatus Get(const object_type * o, data_type * d) { return A3DSurfCylinderGet(o, d); }
	};

	struct SurfNurbsTraits {
		using data_type = A3DSurfNurbsData;
		using object_type = A3DSurfNurbs;
		static void Init(data_type & d) { A3D_INITIALIZE_DATA(A3DSurfNurbsData, d); }
		static A3DStatus Get(const object_type * o, data_type * d) { return A3DSurfNurbsGet(o, d); }
	};

	struct CrvCircleTraits {
		using data_type = A3DCrvCircleData;
		using object_type = A3DCrvCircle;
		static void Init(data_type & d) { A3D_INITIALIZE_DATA(A3DCrvCircleData, d); }
		static A3DStatus Get(const object_type * o, data_type * d) { return A3DCrvCircleGet(o, d); }
	};

	// ----------------- 편의 별칭 -----------------
	using TopoBrepDataGuard = DataGuard<TopoBrepTraits>;
	using TopoConnexDataGuard = DataGuard<TopoConnexTraits>;
	using TopoFaceDataGuard = DataGuard<TopoFaceTraits>;
	using TopoShellDataGuard = DataGuard<TopoShellTraits>;
	using TopoLoopDataGuard = DataGuard<TopoLoopTraits>;
	using TopoCoEdgeDataGuard = DataGuard<TopoCoEdgeTraits>;
	using TopoEdgeDataGuard = DataGuard<TopoEdgeTraits>;
	
	using SurfPlaneDataGuard = DataGuard<SurfPlaneTraits>;
	using SurfCylinderDataGuard = DataGuard<SurfCylinderTraits>;
	using SurfNurbsDataGuard = DataGuard<SurfNurbsTraits>;

	using CrvCircleDataGuard = DataGuard<CrvCircleTraits>;

	//using TopoVertexDataGuard = DataGuard<TopoVertexTraits>;

} // namespace H3DX
