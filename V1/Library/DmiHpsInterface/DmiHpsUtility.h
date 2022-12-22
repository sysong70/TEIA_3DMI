#pragma once

#include <templ_p_array.h>
#include <templ_s_array_rw.h>
#include <mb_cart_point3d.h>
#include <mesh_float_point3d.h>

namespace DmiHps
{
	HPS::Point GetPoint(MbCartPoint3D & cOther);
	HPS::Point GetPoint(MbFloatPoint3D & cOther);

	HPS::Vector GetVector(MbFloatVector3D & cOther);

	HPS::RGBColor GetRgbColor(COLORREF cColor);

	// Char text를 Unicode CString으로 변환하여 돌려줌
	// Windows에서 Unicode 인코딩은 UTF-16을 사용한다. 그러니까 유니코드로 변환은 고정적으로 UTF-16으로 인코딩한다는 것으로 보면 된다. 
	// 한국어를 사용하는 윈도우에서는 멀티바이트일때 CP949를 디폴트로 사용한다. 
	// 따라서 WideCharToMultiByte 사용시 CodePage 인자로 UTF-8을 넣는게 아니라 CP_ACP를 넣으면 CP949로 인코딩된다.
	bool CharToCString(char * pchText, CString & strText, UINT nCodePage = CP_UTF8);
	bool CStringToChar(CString strText, char *& pchText, int & nTextSize, UINT nCodePage = CP_UTF8);

	CString ToString(const HPS::UTF8 & chText);

	bool CopySegmentProperties(HPS::SegmentKey & cSoruce, HPS::SegmentKey & cDestination);
	bool GetFrontFaceAlpha(HPS::SegmentKey cSegKey, float & cAlpha);
};
