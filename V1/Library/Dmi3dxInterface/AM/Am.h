#pragma once

namespace AM
{
	class Object;
	
	class Kit;
	class TemplateKit;

	class Equipment;
	class Template;

	enum class Type : uint32_t {
		None = 0x00000000,
		GenericMask			= 0xffffff00,

		Kit					= 0x01000000,
		EquipmentKit		= 0x01000001,
		TemplateKit			= 0x01000002,

		Object				= 0x10000000,
		Equipment			= 0x10000001,
		Template			= 0x10000002,
	};


}
