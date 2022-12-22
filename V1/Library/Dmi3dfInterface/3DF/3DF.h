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
#else
#	define API_3DF __declspec (dllimport)
#endif

#define OPEN_3DF_NAMESPACE namespace _3DF {
#define CLOSE_3DF_NAMESPACE }

#define USING_3DF_NAMESPACE using namespace _3DF;

OPEN_3DF_NAMESPACE

//== Forward Declarations ==========================================================================
class Model;
class View;
class Key;
class SegmentKey;
class PortfolioKey;
class ShellKey;
class LineKey;
class BoundingKit;
class NamedStyleDefinition;
class StyleKey;
//==================================================================================================

enum class ModelHandedness
{
	Left,
	Right,
	None,
	NotSet
};

CLOSE_3DF_NAMESPACE