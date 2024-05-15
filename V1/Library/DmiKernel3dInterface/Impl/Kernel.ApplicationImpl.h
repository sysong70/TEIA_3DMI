#pragma once

#include <Json.h>

#include "../Kernel.h"

#include "../../Dmi3dfInterface/Sprocket/3DF.Application.h"

#include "../Kernel.DocView.h"

#include <map>

namespace KERNEL
{
	class ApplicationImpl : public Impl
	{
	public:
		std::map<int, DocView *> m_mapDocView;
		H3DF::Application m_cApplication;
	};
};