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
#	define _3DF_INLINE __forceinline
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

#include <atlcoll.h>

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

	Kit										= 0x01000000,
	CircleKit								= 0x01000017,
	LineKit									= 0x01000020,
	ShellKit								= 0x01000027,
	CameraKit								= 0x01000031,

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

using SegmentKeyArray = CAtlArray<SegmentKey>;

using LineArray = CAtlArray<LineKit>;
using PolylineArray = LineArray;
using Polyline = LineKit;

using WindowPointArray = CAtlArray<WindowPoint>;
using WorldPointArray = CAtlArray<WorldPoint>;
using PixelPointArray = CAtlArray<PixelPoint>;

using Polygon = PolygonKit;
using PolygonArray = CAtlArray<PolygonKit>;

using StringArray = CAtlArray<CString>;

namespace PMI {
	class TextAttributes;
	using TextAttributesArray = CAtlArray<TextAttributes>;
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

	virtual TDF::Type Type() const;

	void SetImpl(Object * pcObject, PrivateImpl * pcImpl);
	void SetObject(Object * pcObject);
};

CLOSE_3DF_NAMESPACE