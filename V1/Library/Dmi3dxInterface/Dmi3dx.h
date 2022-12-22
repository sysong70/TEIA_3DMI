#pragma once

#ifndef INITIALIZE_A3D_API
	#include <A3DSDKIncludes.h>
#endif

#include <templ_p_array.h>
#include <templ_s_array_rw.h>
#include <mb_cart_point3d.h>
#include <mb_cart_point.h>

namespace Dmi3dx
{
	// == Text 관련 함수 =============================================================================

	// Char text를 Unicode CString으로 변환하여 돌려줌
	// Windows에서 Unicode 인코딩은 UTF-16을 사용한다. 그러니까 유니코드로 변환은 고정적으로 UTF-16으로 인코딩한다는 것으로 보면 된다. 
	// 한국어를 사용하는 윈도우에서는 멀티바이트일때 CP949를 디폴트로 사용한다. 
	// 따라서 WideCharToMultiByte 사용시 CodePage 인자로 UTF-8을 넣는게 아니라 CP_ACP를 넣으면 CP949로 인코딩된다.
	bool CharToCString(char * pchText, CString & strText, UINT nCodePage = CP_UTF8);
	//bool CStringToChar(char * pchText, CString & strText, UINT nCodePage = CP_ACP);

	bool CStringToChar(CString strText, char *& pchText);
	// Windows에서 Unicode 인코딩은 UTF-16을 사용한다. 그러니까 유니코드로 변환은 고정적으로 UTF-16으로 인코딩한다는 것으로 보면 된다. 
	// 한국어를 사용하는 윈도우에서는 멀티바이트일때 CP949를 디폴트로 사용한다. 
	// 따라서 WideCharToMultiByte 사용시 CodePage 인자로 UTF-8을 넣는게 아니라 CP_ACP를 넣으면 CP949로 인코딩된다.
	bool CStringToChar(CString strText, char *& pchText, int & nTextSize, UINT nCodePage = CP_UTF8);

	// == Directory 관련 함수 ========================================================================

	// 실행 파일 경로 (끝에 '\' 붙어서 나옴)
	CString GetExecuteDirectory();
	// 다중 디렉토리도 생성함.
	bool CreateFolder(CString strPath);

	// == C3D 변환 관련 함수 ==========================================================================
	MbCartPoint GetMbCartPoint(A3DVector3dData cVector, double dScale);
	MbCartPoint GetMbCartPoint(A3DVector3dData cVector);

	MbCartPoint3D GetMbCartPoint3D(A3DVector3dData cVector, double dScale = 1.0);
	MbCartPoint3D GetMbCartPoint3D(A3DVector3dData cVector, A3DVector3dData cScaleVector);

	MbVector3D GetMbVector3D(A3DVector3dData cVector);
	MbVector GetMbVector(A3DVector3dData cVector);

	bool GetPlacement3d(const A3DMiscCartesianTransformationData & cTransformationData, double dParentScale, MbPlacement3D & cPlacement);

	// == A3D 관련 함수 ==============================================================================
	std::wstring GetA3dEntityTypeString(A3DEEntityType eType);
};

