#pragma once

#include <Json.h>

#include "../Kernel.h"

#include "../../Dmi3dfInterface/Sprocket/3DF.Application.h"

#include "../Kernel.Session.h"

#include <map>

namespace KERNEL
{
	class ApplicationImpl : public Impl
	{
	public:
		std::map<int, Session *> m_mapDocView;
		H3DF::Application m_cApplication;
	};
};