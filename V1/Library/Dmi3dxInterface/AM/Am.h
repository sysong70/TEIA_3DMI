#pragma once

namespace AM
{
	class Object;
	
	class Kit;
	class TemplateKit;
	class VertexKit;

	class Equipment;
	class Template;

	class Geometry;
	class Box;
	class Cylinder;
	class Extrusion;
	class Loop;
	class Vertex;

	enum class Type : uint32_t 
	{
		None = 0x00000000,
		GenericMask			= 0xffffff00,

		Kit					= 0x01000000,
		EquipmentKit		= 0x01000001,
		TemplateKit			= 0x01000002,
		BoxKit				= 0x01000003,
		CylinderKit			= 0x01000004,
		ExtrusionKit		= 0x01000005,
		LoopKit				= 0x01000006,
		VertexKit			= 0x01000007,

		Object				= 0x10000000,
		Equipment			= 0x10000001,
		Template			= 0x10000002,

		Geometry			= 0x20000000,
		Box					= 0x20000001,
		Cylinder			= 0x20000002,
		Extrusion			= 0x20000003,
		Loop				= 0x20000004,
		Vertex				= 0x20000005,
	};
}
