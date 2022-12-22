#pragma once

#include "Facility.Base.h"



namespace Facility
{
	class Preference : public Base
	{
	public:

		Preference();

		~Preference() override;

	public:

		Json::Object* Get() override;

		bool Set(Json::Object* pData) override;
	};
}
