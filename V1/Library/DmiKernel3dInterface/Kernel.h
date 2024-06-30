#pragma once

#include <Json.h>

#ifdef KERNEL_EXPORTS
#	define API_KERNEL __declspec (dllexport)
#else
#	define API_KERNEL __declspec (dllimport)
#endif

namespace KERNEL
{
	enum class Type : uint32_t
	{
		None									= 0x00000000,
		GenericMask								= 0xffffff00,

		Kernel									= 0x00000000,
		Manager									= 0x00000001,
		Object									= 0x00000002,
		Application								= 0x00000003,
		View									= 0x00000004,
		
		Command									= 0x50000000,
		CommandSet								= 0x50000001,
		CommandStep								= 0x50000002,
	};

	class OSnap
	{
	public:
		enum class Type : uint32_t
		{
			None								= 0x00000000,	
			EndPoint							= 0x00000001,
			MidPoint							= 0x00000002,
			NearPoint							= 0x00000004,
			Center								= 0x00000008,
			Intersection						= 0x00000010,
			Perpendicular						= 0x00000020,
			Quadrant							= 0x00000040,
			OnSurface							= 0x00000080,
			BoundaryCenter						= 0x00000100,
			Axis								= 0x00000200,
		};

	private:
		OSnap() = default;
	};

	class SelectionFilter
	{
	public:
		enum class Type : uint32_t
		{
			None								= 0x00000000,
			Point								= 0x00000001,
			Curve								= 0x00000002,
			Edge								= 0x00000004,
			Face								= 0x00000008,
			Solid								= 0x00000010,
			Axis								= 0x00000020,
			PMI									= 0x00000040,
		};

	private:
		SelectionFilter() = default;
	};

	class API_KERNEL Kernel
	{
	public:
		Kernel();
	};

	class Object;
	class Session;

	namespace Command
	{
		class Attribute;
		class Camera;
		class Select;
		class ModelPanel;
	}

	class Impl
	{
	public:
		Impl() {}
		virtual ~Impl() {}

		KERNEL::Type Type() const;
		void SetType(KERNEL::Type eType);

		void SetImpl(Object * pcObject, Impl * pcImpl);

	protected:
		KERNEL::Type m_eType = KERNEL::Type::None;
	};
};
