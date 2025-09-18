#include "StdAfx.h"

#include "AM.Equipment.Impl.h"

using namespace AM;

AM::EquipmentImpl::EquipmentImpl()
{
	SetType(AM::Type::Equipment);
}


std::unique_ptr<Impl> AM::EquipmentImpl::Clone() const
{
	auto impl = std::make_unique<EquipmentImpl>(*this);
	DEBUG_VALID(impl.get());

	return impl;
}
