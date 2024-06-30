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
		std::map<int, Session *> m_mpSessionMap;
		H3DF::Application m_cApplication;
		double m_dDpiScale = 1.0;
	};
};