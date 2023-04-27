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

#ifdef _3DF_EXPORT
#	define API_3DF __declspec (dllexport)
#	define DLLEXPORT_TEMPLATE
#else
#	define API_3DF __declspec (dllimport)
#	define DLLEXPORT_TEMPLATE extern
#endif

#define OPEN_3DF_NAMESPACE namespace TDF {
#define CLOSE_3DF_NAMESPACE }

#define USING_3DF_NAMESPACE using namespace TDF;

#include <vector>

OPEN_3DF_NAMESPACE

#define PATTERN_BUFFER_SIZE		1024
#define STYLE_BUFFER_SIZE		128

//== Forward Declarations ==========================================================================
class Object;
class Canvas;
class WindowKey;
class Model;
class Key;
class SegmentKey;
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

class MatrixKit;
class CameraKit;

class WindowPoint;
class WorldPoint;
class PixelPoint;

class NamedStyleDefinition;
class StyleKey;

class SelectabilityControl;
class VisibilityControl;
class MarkerAttributeControl;


//==================================================================================================

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
	PolygonKey								= 0x10100011,
	ShellKey								= 0x10100012,
	SphereKey								= 0x10100013,
	TextKey									= 0x10100014,

	Control									= 0x50000000,
	SelectionControl						= 0x50000017,
	HighlightControl						= 0x50000018,
};

enum class UserDataIndex : uint32_t
{
	None									= 0x00000000,

	// Item 관련 User Data Index
	Type									= 0x10000000,
	Name									= 0x10000001,
};


//== Allocator Template Specializations ============================================================

#ifndef TDF_UNREFERENCED
#	define TDF_UNREFERENCED(param) ((void)(param))
#endif

class API_3DF Memory
{
public:
	static void * Allocate(size_t nInBytes, bool bInClearMemory = true);
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
	struct rebind
	{
		typedef Allocator<U> other;
	};


	pointer address(reference x) const { return &x; }
	const_pointer address(const_reference x) const { return &x; }

	pointer  allocate(size_type n, void * v = 0) { TDF_UNREFERENCED(v); return static_cast<pointer>(Memory::Allocate(n * sizeof(T))); }
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
bool operator==(const Allocator<T> &, const Allocator<U> &) { return true; }

template <typename T, typename U>
bool operator!=(const Allocator<T> &, const Allocator<U> &) { return false; }


//== Type Definitions ==============================================================================
using SegmentKeyArray = std::vector<SegmentKey, Allocator<SegmentKey>>;

using LineArray = std::vector<LineKit, Allocator<LineKit>>;
using PolylineArray = LineArray;
using Polyline = LineKit;

using WindowPointArray = std::vector<WindowPoint, Allocator<WindowPoint>>;
using WorldPointArray = std::vector<WorldPoint, Allocator<WorldPoint>>;
using PixelPointArray = std::vector<PixelPoint, Allocator<PixelPoint>>;

using Polygon = PolygonKit;
using PolygonArray = std::vector<PolygonKit, Allocator<PolygonKit>>;

using StringArray = std::vector<CString, Allocator<CString>>;

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

	TDF::Type Type() const;
	void SetType(TDF::Type eType);

	void SetImpl(Object * pcObject, PrivateImpl * pcImpl);

protected:
	TDF::Type m_eType = TDF::Type::None;
};

CLOSE_3DF_NAMESPACE