#pragma once

#include "3DF/3DF.h"

#include "Sprocket.h"

#include "../Signal/Signal.h"

namespace H3DF
{
	class API_3DF Application : public Sprocket
	{
	public:
		void InitInstance();
		void ExitInstance();
	};
};