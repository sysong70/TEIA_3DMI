#pragma once

#ifdef H3DX_EXPORTS
#	define API_3DX __declspec (dllexport)
#else
#	define API_3DX __declspec (dllimport)
#endif
