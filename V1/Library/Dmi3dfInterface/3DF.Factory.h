#pragma once

#include "3DF/3DF.h"

#include "Sprocket.h"

#include "3DF/ApplicationWindowOption.h"

namespace H3DF
{
	class Canvas;
	class View;

	class API_3DF Factory : public Sprocket
	{
	public:
		static Canvas * CreateCanvas(H3DF::WindowHandle nInWindowHandle, char const * chInName = nullptr, H3DF::ApplicationWindowOptionsKit const & cInOptions = H3DF::ApplicationWindowOptionsKit());

		static View * CreateView(CStringA strInName);

	private:
		Factory() {}
	};
}