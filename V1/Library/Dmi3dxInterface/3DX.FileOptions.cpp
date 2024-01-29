#include "StdAfx.h"
#include "3DX.h"
#include "3DX.FileOptions.h"

//--------------------------------------------------------------------------------------------------

H3DX::FileOptions TheFileOptions;

//--------------------------------------------------------------------------------------------------

#pragma region JsonWrapper Class

H3DX::JsonWrapper::JsonWrapper()
{
}



H3DX::JsonWrapper::~JsonWrapper()
{
}



bool H3DX::JsonWrapper::SetRootObject(const wchar_t* pSource)
{
	wchar_t* pStream = (wchar_t*)pSource;
	bool success = Json::Reader::ReadObject(pStream, m_root);
	//:TODO - version check
	//ASSERT(m_root.GetString("version") == L"240126.1");

	return success;
}

#pragma endregion //:REGION

//--------------------------------------------------------------------------------------------------

#pragma region FileOptions Class

H3DX::FileOptions::FileOptions()
{
}



H3DX::FileOptions::~FileOptions()
{
}



bool H3DX::FileOptions::GetImport(CStringA fileTypeName, A3DRWParamsLoadData& param)
{
	if (IsValidFileTypeName(fileTypeName) == false) {
		return false;
	}

	// set category for sub functions
	m_activeType = fileTypeName;

	Json::Object& data = m_root.GetAt("Import").GetAt(fileTypeName);
	bool success = true;

	success &= GetGeneral(data, param);
	success &= GetTessellation(data, param);
	success &= GetSearch(data, param);
	success &= GetPMI(data, param);
	success &= GetSpecial(data, param);
	ASSERT(success);

	// reset cateogry
	m_activeType.Empty();

	return success;
}



bool H3DX::FileOptions::GetGeneral(Json::Object& source, A3DRWParamsLoadData& target)
{
	//:WARNING - essential
	if (source.FindValue("General") == nullptr) {
		RETURN_FALSE;
	}

	Json::Object& data = source.GetAt("General");
	A3DRWParamsGeneralData& param = target.m_sGeneral;

	int readingMode = data.GetInteger("ReadingMode");
	int unit = data.GetInteger("GeneralUnits");

	//param.m_usStructSize;
	param.m_bReadSolids = data.GetBoolean("Solids");
	param.m_bReadSurfaces = data.GetBoolean("Surfaces");
	param.m_bReadWireframes = data.GetBoolean("Wireframes");
	//param.m_bReadPmis;
	param.m_bReadAttributes = data.GetBoolean("Attributes");
	param.m_bReadHiddenObjects = data.GetBoolean("HiddenObjects");
	param.m_bReadConstructionAndReferences = data.GetBoolean("ConstructionAndReferences");
	param.m_bReadActiveFilter = data.GetBoolean("ActiveFilter");
	param.m_eReadingMode2D3D = kA3DRead_3D; //:CHECK
	param.m_eReadGeomTessMode = (A3DEReadGeomTessMode)(readingMode + 1) ; //:CHECK - non Feature
	param.m_eDefaultUnit = unit == 0 ? kA3DUnitUnknown : (A3DEUnits)(unit - 1);
	//param.m_bReadFeature;
	//param.m_bReadConstraints;
	//param.m_iNbMultiProcess;
	//param.m_uiSearchTextureDirectoriesSize;
	//param.m_ppcSearchTextureDirectories;

	return true;
}



bool H3DX::FileOptions::GetTessellation(Json::Object& source, A3DRWParamsLoadData& target)
{
	if (source.FindValue("Tessellation")) {
		Json::Object& data = source.GetAt("Tessellation");
		A3DRWParamsTessellationData& param = target.m_sTessellation;

		Json::Object& custom = data.GetAt("CustomTessLevel");
		Json::Object& accurate = data.GetAt("AccurateTess");
		Json::Object& gridAlignedTess = accurate.GetAt("GridAlignedTess");

		//param.m_usStructSize;
		param.m_eTessellationLevelOfDetail = (A3DETessellationLevelOfDetail)data.GetInteger("TessLevel");
		param.m_dChordHeightRatio = (double)custom.GetInteger("ChordLimit"); //:CHECK
		param.m_dAngleToleranceDeg = (double)custom.GetInteger("AngleTolerance"); //:CHECK
		//param.m_dMinimalTriangleAngleDeg;
		//param.m_dMaxChordHeight;
		param.m_bAccurateTessellation = accurate.GetBoolean("checked");
		param.m_bAccurateTessellationWithGrid = gridAlignedTess.GetBoolean("checked");
		param.m_dAccurateTessellationWithGridMaximumStitchLength = gridAlignedTess.GetReal("MaxStitchLength");
		param.m_bAccurateSurfaceCurvatures = gridAlignedTess.GetReal("SurfaceCurvatures");
		param.m_bDoNotComputeNormalsInAccurateTessellation;
		param.m_bKeepUVPoints = data.GetBoolean("PreserveUV");
		param.m_bUseHeightInsteadOfRatio = custom.GetInteger("ChordLimitType") == 1; //:CHECK
		param.m_dMaximalTriangleEdgeLength = (double)data.GetInteger("TessMaxEdgeLength"); //:CHECK
	}
	else {
		//:TODO - default?
	}

	return true;
}



bool H3DX::FileOptions::GetSearch(Json::Object& source, A3DRWParamsLoadData& target)
{
	//:WARNING - essential
	if (source.FindValue("Search") == nullptr) {
		RETURN_FALSE;
	}

	//:TODO

	return true;
}



bool H3DX::FileOptions::GetPMI(Json::Object& source, A3DRWParamsLoadData& target)
{
	if (source.FindValue("PMI")) {
		Json::Object& data = source.GetAt("PMI");
		A3DRWParamsPmiData& param = target.m_sPmi;


	}
	else {
		//:TODO - default?
	}

	return true;
}



bool H3DX::FileOptions::GetSpecial(Json::Object& source, A3DRWParamsLoadData& target)
{
	return false;
}

/*
	UI structures - UiMain/RES/UI.json (Dialogs/FileOptions/tree/0)

	{"name":"3MF", "title":"3MF", "ext":["3MF"],                                                            "group":[1,0,1,0,0], "General":[9,10]},
	{"name":"ACIS", "title":"ACIS", "ext":["SAT", "SAB"],                                                   "group":[1,1,1,0,0], "General":[9,10]},
	{"name":"ACAD", "title":"AutoCAD", "ext":["DWG", "DXF"],                                                "group":[1,1,1,0,1], "General":[9,10]},
	{"name":"3DS", "title":"Autodesk 3DS", "ext":["3DS"],                                                   "group":[1,0,1,0,0], "General":[0,9]},
	{"name":"DWF", "title":"Autodesk DWF", "ext":["DWF", "DWFX"],                                           "group":[1,0,1,0,0], "General":[0,9]},
	{"name":"Inventor", "title":"Autodesk Inventor", "ext":["IPT", "IAM"],                                  "group":[1,1,1,0,1], "General":[9,10]},
	{"name":"Navisworks", "title":"Autodesk Navisworks", "ext":["NWD"],                                     "group":[1,0,1,0,0], "General":[9,10]},
	{"name":"CATIA4", "title":"CATIA V4", "ext":["MODEL", "SESSION", "DLV", "EXP"],                         "group":[1,1,1,1,0], "General":[9,10]},
	{"name":"CATIA5", "title":"CATIA V5", "ext":["CATDrawing", "CATPart", "CATProduct", "CATShape", "CGR"], "group":[1,1,1,1,1], "General":[10]},
	{"name":"CATIA6", "title":"CATIA V6/3DExperience", "ext":["3DXML"],                                     "group":[1,0,1,1,0], "General":[0,9,10]},
	{"name":"COLLADA", "title":"COLLADA", "ext":["DAE"],                                                    "group":[1,0,1,0,0], "General":[0,9]},
	{"name":"ProE", "title":"Creo/Pro-E", "ext":["ASM", "NEU", "PRT", "PRT.*", "XAS", "XPR"],               "group":[1,1,1,1,1], "General":[10]},
	{"name":"FBX", "title":"FBX", "ext":["FBX"],                                                            "group":[1,0,1,0,0], "General":[0,9]},
	{"name":"GLTF", "title":"GL Transmission Format", "ext":["GLTF", "GLB"],                                "group":[1,0,1,1,0], "General":[0,9,10]},
	{"name":"Ideas", "title":"I-deas", "ext":["MF1", "ARC", "UNV", "PKG"],                                  "group":[1,1,1,1,0], "General":[9,10]},
	{"name":"IFC", "title":"IFC", "ext":["IFC", "IFCZIP"],                                                  "group":[1,1,1,0,0], "General":[0,9,10]},
	{"name":"IGES", "title":"IGES", "ext":["IGS", "IGES"],                                                  "group":[1,1,1,0,0], "General":[9,10]},
	{"name":"JT", "title":"JT", "ext":["JT"],                                                               "group":[1,1,1,1,1], "General":[9,10]},
	{"name":"NX", "title":"NX Unigraphics", "ext":["PRT"],                                                  "group":[1,1,1,1,1], "General":[10]},
	{"name":"Parasolid", "title":"Parasolid", "ext":["X_B", "X_T", "XMT", "XMT_TXT"],                       "group":[1,1,1,0,0], "General":[9,10]},
	{"name":"PDF", "title":"PDF", "ext":["PDF"],                                                            "group":[1,1,1,1,0], "General":[9,10]},
	{"name":"PRC", "title":"PRC", "ext":["PRC"],                                                            "group":[1,1,1,1,0], "General":[9,10]},
	{"name":"Revit", "title":"Revit", "ext":["RVT", "RFA"],                                                 "group":[1,0,1,0,0], "General":[9,10]},
	{"name":"Rhino3D", "title":"Rhino3D", "ext":["3DM"],                                                    "group":[1,1,1,0,0], "General":[0,9,10]},
	{"name":"SolidEdge", "title":"Solid Edge", "ext":["ASM", "PAR", "PWD", "PSM"],                          "group":[1,1,1,0,0], "General":[9,10]},
	{"name":"SolidWorks", "title":"SolidWorks", "ext":["SLDASM", "SLDPRT"],                                 "group":[1,1,1,1,1], "General":[10]},
	{"name":"STEP", "title":"STEP", "ext":["STP", "STEP", "STPZ", "STPX", "STPXZ"],                         "group":[1,1,1,1,0], "General":[9,10]},
	{"name":"STL", "title":"Stereo Lithography", "ext":["STL"],                                             "group":[1,0,1,1,0], "General":[0,9]},
	{"name":"U3D", "title":"U3D", "ext":["U3D"],                                                            "group":[1,0,1,1,0], "General":[0,9]},
	{"name":"VDA", "title":"VDA-FS", "ext":["VDA"],                                                         "group":[1,1,1,0,0], "General":[0,9,10]},
	{"name":"VRML", "title":"VRML", "ext":["WRL", "VRML"],                                                  "group":[1,0,1,1,0], "General":[0,9]},
	{"name":"OBJ", "title":"Wavefront OBJ", "ext":["OBJ"],                                                  "group":[1,0,1,1,0], "General":[9,10]},
*/

bool H3DX::FileOptions::IsValidFileTypeName(CStringA name)
{
	const CStringA categories[] = {
		"3MF", "ACIS", "ACAD", "3DS", "DWF", "Inventor", "Navisworks", "CATIA4", "CATIA5", "CATIA6",
		"COLLADA", "ProE", "FBX", "GLTF", "Ideas", "IFC", "IGES", "JT", "NX", "Parasolid", "PDF",
		"PRC", "Revit", "Rhino3D", "SolidEdge", "SolidWorks", "STEP", "STL", "U3D", "VDA", "VRML", "OBJ"
	};

	for (auto cat : categories) {
		if (cat == name) {
			return true;
		}
	}

	RETURN_FALSE;
}

#pragma endretion //:REGION
