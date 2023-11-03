#pragma once

//== HOOPS 관련 Define ==============================================================================
#ifndef HC_POINTER_SIZED_INT
#if defined(_M_IA64) || defined(_M_AMD64) || defined(WIN64) || defined(_WIN64) || defined(_M_X64)
#   define  HC_POINTER_SIZED_INT    __int64
#else
#   define  HC_POINTER_SIZED_INT    long
#endif
#endif

#ifndef HC_KEY
#   define  HC_KEY      HC_POINTER_SIZED_INT
#endif

#ifndef INVALID_KEY
#	define INVALID_KEY (-1L)
#endif

#ifndef _3DF_INLINE
#	define TDF_INLINE __forceinline
#endif

#ifndef _3DF_UNREFERENCED
#	define _3DF_UNREFERENCED(param) ((void)(param))
#endif

#ifdef H3DF_EXPORT
#	define API_3DF __declspec (dllexport)
#else
#	define API_3DF __declspec (dllimport)
#endif

#define OPEN_3DF_NAMESPACE namespace H3DF {
#define CLOSE_3DF_NAMESPACE }

#define USING_3DF_NAMESPACE using namespace H3DF;

#include <vector>

namespace H3DF
{
#	define PATTERN_BUFFER_SIZE		1024
#	define STYLE_BUFFER_SIZE		128

	//== Forward Declarations ======================================================================
	class Object;
	class Canvas;
	class View;
	class WindowKey;
	class Model;
	class Key;
	class SegmentKey;
	class IncludeKey;
	class MaterialMappingKit;
	class PortfolioKey;
	class ShellKey;
	class CircleKit;
	class CircleKey;
	class LineKit;
	class LineKey;
	class PolygonKit;
	class PolygonKey;
	class BoundingKit;
	class SelectabilityKit;

	class LineAttributeKit;
	class LineAttributeControl;

	class MatrixKit;
	class CameraKit;

	class WorldPoint;
	class InnerWindowPoint;
	class WindowPoint;
	class PixelPoint;

	class NamedStyleDefinition;
	class StyleKey;

	class SelectabilityControl;
	class SelectionOptionsControl;

	class VisibilityControl;
	class VisualEffectsControl;

	class MarkerAttributeControl;
	class MaterialMappingControl;

	class ApplicationWindowOptionsKit;

	//==============================================================================================

	// The Type class is a concept class that contains type information for all classes.
	enum class Type : uint32_t
	{
		None									= 0x00000000,
		GenericMask								= 0xffffff00,

		SelectionResults						= 0x0000000a,
		SelectionItem							= 0x0000000b,

		SelectionResultsIterator				= 0x01000003,

		Kit										= 0x01000000,
		CircleKit								= 0x01000017,
		LineKit									= 0x01000020,
		ShellKit								= 0x01000027,
		MaterialKit								= 0x01000029,
		CameraKit								= 0x01000031,
		BoundingKit								= 0x01000032,
		CullingKit								= 0x01000033,
		CurveAttributeKit						= 0x01000034,
		CylinderAttributeKit					= 0x01000035,
		EdgeAttributeKit						= 0x01000036,
		LightingAttributeKit					= 0x01000037,
		LineAttributeKit						= 0x01000038,
		MarkerAttributeKit						= 0x01000039,
		MaterialMappingKit						= 0x0100003a,
		ApplicationWindowOptionsKit				= 0x01000042,

		Key										= 0x10000000,
		IncludeKey								= 0x10000001,
		PortfolioKey							= 0x10000002,
		StyleKey								= 0x10000003,

		SegmentKey								= 0x10200000,
		WindowKey								= 0x10600000,

		GeometryKey								= 0x10100000,
		ReferenceKey							= 0x10100001,
		CircleKey								= 0x10100002,
		CircularArcKey							= 0x10100003,
		CircularWedgeKey						= 0x10100004,
		CuttingSectionKey						= 0x10100005,
		CylinderKey								= 0x10100006,
		EllipseKey								= 0x10100007,
		EllipticalArcKey						= 0x10100008,
		InfiniteLineKey							= 0x10100009,
		LineKey									= 0x1010000a,
		DistantLightKey							= 0x1010000b,
		SpotlightKey							= 0x1010000c,
		MarkerKey								= 0x1010000d,
		MeshKey									= 0x1010000e,
		NURBSCurveKey							= 0x1010000f,
		NURBSSurfaceKey							= 0x10100010,
		PolygonKey								= 0x10100011,
		ShellKey								= 0x10100012,
		SphereKey								= 0x10100013,
		TextKey									= 0x10100014,
		GridKey									= 0x10100015,


		Control									= 0x50000000,
		CameraControl							= 0x50000001,
		SelectabilityControl					= 0x50000002,
		MarkerAttributeControl					= 0x50000003,
		SphereAttributeControl					= 0x50000004,
		LightingAttributeControl				= 0x50000005,
		CylinderAttributeControl				= 0x50000006,
		TextAttributeControl					= 0x50000007,
		LineAttributeControl					= 0x50000008,
		EdgeAttributeControl					= 0x50000009,
		CurveAttributeControl					= 0x5000000a,
		ModellingMatrixControl					= 0x5000000b,
		TextureMatrixControl					= 0x5000000c,
		CullingControl							= 0x5000000d,
		TransparencyControl						= 0x5000000e,
		MaterialMappingControl					= 0x5000000f,
		NURBSSurfaceAttributeControl			= 0x50000010,
		PostProcessEffectsControl				= 0x50000011,
		BoundingControl							= 0x50000012,
		VisualEffectsControl					= 0x50000013,
		SelectionOptionsControl					= 0x50000014,
		HighlightOptionsControl					= 0x50000015,
		DefinitionControl						= 0x50000016,
		SelectionControl						= 0x50000017,
		HighlightControl						= 0x50000018,
		StandAloneWindowOptionsControl			= 0x50600019, //Is a Window Key
		OffScreenWindowOptionsControl			= 0x5060001a, //Is a Window Key
		ApplicationWindowOptionsControl			= 0x5060001b, //Is a Window Key
		VisibilityControl						= 0x5000001c,
		SubwindowControl						= 0x5000001d,
		PerformanceControl						= 0x5000001e,
		HiddenLineAttributeControl				= 0x5000001f,
		DrawingAttributeControl					= 0x50000020,
		DebuggingControl						= 0x50000021,
		ContourLineControl						= 0x50000022,
		StyleControl							= 0x50000023,
		ConditionControl						= 0x50000024,
		PortfolioControl						= 0x50000025,
		WindowInfoControl						= 0x50000026,
		AttributeLockControl					= 0x50000027,
		TransformMaskControl					= 0x50000028,
		ColorInterpolationControl				= 0x50000029,
		UpdateOptionsControl					= 0x50600030, //Is a Window Key
		CuttingSectionAttributeControl			= 0x50000031,

		Sprocket								= 0x80000000,
		Canvas									= 0x80000001,
		Layout									= 0x80000002,
		View									= 0x80000003,
		Model									= 0x80000004,
		Operator								= 0x80000005,
		SprocketPath							= 0x80000007,

		SprocketControl							= 0xD0000000,
		OperatorControl							= 0xD0000008,
		NavigationCubeControl					= 0xD0000009,
		AxisTriadControl						= 0xD000000A,
		AnimationControl						= 0xD000000B,

		SprocketKit								= 0x81000000,

	};

	enum class UserDataIndex : uint32_t
	{
		None = 0x00000000,

		// Item 관련 User Data Index
		Type = 0x10000000,
		Name = 0x10000001,
	};

	class API_3DF ViewDirection
	{
	public:
		enum class Mode
		{
			top,
			bottom,
			front,
			back,
			left,
			right,

			py_nz,
			py_pz,
			ny_pz,
			ny_nz,

			nx_nz,
			nx_pz,
			px_pz,
			px_nz,

			nx_py,
			px_py,
			px_ny,
			nx_ny,

			nx_py_nz,
			nx_py_pz,
			nx_ny_pz,
			nx_ny_nz,

			px_py_pz, // ISO
			px_py_nz,
			px_ny_nz,
			px_ny_pz,

			Count,
			Unknown
		};
	private:
		ViewDirection() {}
	};

	class API_3DF ViewControl
	{
	public:
		enum class Mode
		{
			Multi,
			Pan,
			Orbit,
			Zoom,
			ZoomBox,
		};
	};

	class API_3DF Rendering
	{
	public:
		enum class Mode
		{
			Gouraud,
			GouraudWithLines,
			Flat,
			FlatWithLines,
			Phong,
			PhongWithLines,
			HiddenLine,
			FastHiddenLine,
			Wireframe,
			Tessellated,
			Default = Phong
		};

	private:
		Rendering() {}
	};

	class API_3DF VisualEffects
	{
	public:
		enum class ShadowMode // 숫자값을 바꾸면 않됨.
		{
			None = 1,  // no shadow
			Soft = 2,  // soft shadow
			Hard = 3   // hard shadow
		};

	private:
		VisualEffects() {}
	};

	using WindowHandle = DWORD_PTR;
	using Time = double;

	//== Allocator Template Specializations ========================================================

#ifndef TDF_UNREFERENCED
#	define TDF_UNREFERENCED(param) ((void)(param))
#endif

	class API_3DF Memory
	{
	public:
		static void * Allocate(size_t nInBytes, bool bInClearMemory = false);
		static void	Free(void * pInPointer);

	private:
		//! Private default constructor to prevent instantiation.
		Memory();
	};

	template <typename T>
	class Allocator
	{
	public:
		typedef T					value_type;
		typedef value_type * pointer;
		typedef value_type const * const_pointer;
		typedef value_type & reference;
		typedef value_type const & const_reference;
		typedef size_t				size_type;
		typedef ptrdiff_t			difference_type;

		Allocator() {}
		Allocator(Allocator<T> const & in_that) { TDF_UNREFERENCED(in_that); }
		~Allocator() {}

		template <typename U> Allocator(Allocator<U> const &) {}

		template <typename U>
		struct rebind {
			typedef Allocator<U> other;
		};

		pointer address(reference x) const { return &x; }
		const_pointer address(const_reference x) const { return &x; }

		pointer allocate(size_type n, void * v = 0) { TDF_UNREFERENCED(v); return static_cast<pointer>(Memory::Allocate(n * sizeof(T))); }
		void deallocate(pointer p, size_type n) { TDF_UNREFERENCED(n); Memory::Free(p); }

#if defined(_MSC_VER) || defined (__APPLE__)
		void construct(pointer p, const_reference x) { new(p) T(x); }
		void construct(pointer p, value_type && x) { new(p) T(std::move(x)); }
#else
		template<typename U, typename... Args>
		void construct(U * p, Args&&... args) { new(p) U(std::forward<Args>(args)...); }
#endif

		void destroy(pointer p) { TDF_UNREFERENCED(p); p->~T(); }

		size_type max_size() const { return static_cast<size_type>(-1) / sizeof(T); }
	};

	template <typename T, typename U>
	bool operator == (const Allocator<T> &, const Allocator<U> &) { return true; }

	template <typename T, typename U>
	bool operator != (const Allocator<T> &, const Allocator<U> &) { return false; }


	//== Type Definitions ==========================================================================
	using SegmentKeyArray = std::vector<SegmentKey, Allocator<SegmentKey>>;
	using IncludeKeyArray = std::vector<IncludeKey, Allocator<IncludeKey>>;

	using LineArray = std::vector<LineKit, Allocator<LineKit>>;
	using PolylineArray = LineArray;
	using Polyline = LineKit;

	using WindowPointArray = std::vector<WindowPoint, Allocator<WindowPoint>>;
	using WorldPointArray = std::vector<WorldPoint, Allocator<WorldPoint>>;
	using PixelPointArray = std::vector<PixelPoint, Allocator<PixelPoint>>;

	using Polygon = PolygonKit;
	using PolygonArray = std::vector<PolygonKit, Allocator<PolygonKit>>;

	using StringArray = std::vector<CString, Allocator<CString>>;

	using IntPtrTArray = std::vector<intptr_t, Allocator<intptr_t>>;
	using ByteArray = std::vector<BYTE, Allocator<BYTE>>;
	using ByteArrayArray = std::vector<ByteArray, Allocator<ByteArray>>;

	namespace PMI {
		class TextAttributes;
		using TextAttributesArray = std::vector<TextAttributes, Allocator<TextAttributes>>;
	};

	enum class ModelHandedness
	{
		Left,
		Right,
		None,
		NotSet
	};

	class PrivateImpl
	{
	public:
		PrivateImpl() {}
		virtual ~PrivateImpl() {}

		H3DF::Type Type() const;
		void SetType(H3DF::Type eType);

		void SetImpl(Object * pcObject, PrivateImpl * pcImpl);

	protected:
		H3DF::Type m_eType = H3DF::Type::None;
	};
}