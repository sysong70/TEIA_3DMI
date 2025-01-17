#include "StdAfx.h"
#include "3DX.FileOptions.h"
#include <Path.h>
#include <WStr.h>

#include "LogManager.h"

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



bool H3DX::JsonWrapper::Set(Json::Object& value)
{
	m_root = value;
	return m_root.IsEmpty() == false;
}

#pragma endregion //:REGION

//--------------------------------------------------------------------------------------------------

#pragma region ImportOptions Class

H3DX::ImportOptions::ImportOptions()
{
}



H3DX::ImportOptions::~ImportOptions()
{
}



bool H3DX::ImportOptions::Set(Json::Object& value)
{
	const CStringA category = "Import";

	if (value.FindValue(category) == nullptr) {
		RETURN_FALSE;
	}

	bool success = JsonWrapper::Set(value.GetAt(category));
	ASSERT(success);

	return success;
}



bool H3DX::ImportOptions::SetReference(Json::Object& value)
{
	ASSERT(value.IsEmpty() == false && m_reference.IsEmpty());
	m_reference = value;

	return true;
}



bool H3DX::ImportOptions::Get(CString& filePath, A3DRWParamsLoadData& param)
{
	//:CHECK
	A3D_INITIALIZE_DATA(A3DRWParamsLoadData, param);

	A3DEModellerType eModellerType;
	A3DStatus nResult = A3DGetFileFormat(ToHoopsString(filePath), &eModellerType);

	CStringA fileType = (CStringA)GetFileTypeName(filePath);
	if (fileType.IsEmpty()) {
		return false;
	}

	LogManager::Log(2, "File Type: %s", fileType);

	Json::Object& data = m_root.GetAt(fileType);
	bool success = true;

	success &= GetGeneral(data, param);
	success &= GetTessellation(data, param);
	success &= GetSearch(data, param);
	success &= GetPMI(data, param);
	success &= GetSpecial(fileType, data, param);
	ASSERT(success);

	return success;
}

/*
{"type":"group", "name":"General", "title":"General|일반", "items":[
	{"type":"drop", "name":"ReadingMode", "title":"Reading Mode|__#__", "desc":"__#__|__#__", "value":0, "items":[
		"BRep & Tessellation|__#__",
		"Tessellation Only|__#__"
	]},
	{"type":"check", "name":"Solids", "title":"Solid|__#__", "desc":"__#__|__#__"},
	{"type":"check", "name":"Surfaces", "title":"Surfaces|__#__", "desc":"__#__|__#__"},
	{"type":"check", "name":"Wireframes", "title":"Wireframes|__#__", "desc":"__#__|__#__"},
	{"type":"check", "name":"Attributes", "title":"Attributes|__#__", "desc":"__#__|__#__"},
	{"type":"check", "name":"HiddenObjects", "title":"Hidden Objects|__#__", "desc":"__#__|__#__"},
	{"type":"check", "name":"ConstructionAndReferences", "title":"Construction and References|__#__", "desc":"__#__|__#__"},
	{"type":"check", "name":"ActiveFilter", "title":"Active Filter|__#__", "desc":"__#__|__#__"},
	{"type":"group", "name":"SewModel", "title":"Sew Model|__#__", "desc":"__#__|__#__", "hasCheck":true, "items":[
		{"type":"edit", "name":"SewingTolerance", "title":"Sewing Tolerance|__#__", "desc":"__#__|__#__", "value":"0.001"},
		{"type":"check", "name":"ShellOrientation", "title":"Compute Non-Solid Shell Orientation|__#__", "desc":"__#__|__#__"}
	]},
	{"type":"drop", "name":"LoadingMode", "title":"Loading Mode|__#__", "desc":"__#__|__#__", "value":0, "items":[
		"Complete|__#__",
		"Incremental|__#__"
	]},
	{"type":"drop", "name":"GeneralUnits", "title":"Units|__#__", "desc":"__#__|__#__", "value":0, "items":[
		"Unknown|__#__",
		"Point(pt)|__#__",
		"Inch(in)|__#__",
		"Millimeter(mm)|__#__",
		"Centimeter(cm)|__#__",
		"Picas(pica)|__#__",
		"Foot(ft)|__#__",
		"Yard(yd)|__#__",
		"Meter(m)|__#__",
		"Kilometer(km)|__#__",
		"Mile(mi)|__#__"
	]},
	{"type":"check", "name":"TrueColorMode", "title":"True Color Mode|__#__", "desc":"__#__|__#__"},
	{"type":"group", "name":"LargeTansformation", "title":"Extract Large Transformation|", "hasCheck":true, "items":[
		{"type":"edit", "name":"Cutoff", "title":"Cutoff Value|__#__", "desc":"__#__|__#__", "value":"12000"}
	]}
]}
*/

// + "ReadingMode"
// + "Solids"
// + "Surfaces"
// + "Wireframes"
// + "Attributes"
// + "HiddenObjects"
// + "ConstructionAndReferences"
// + "ActiveFilter"
// - "SewModel"
// - "SewingTolerance"
// - "ShellOrientation"
// - "LoadingMode"
// - "GeneralUnits"
// - "TrueColorMode"
// - "LargeTansformation"
// - "Cutoff"

bool H3DX::ImportOptions::GetGeneral(Json::Object& source, A3DRWParamsLoadData& target)
{
	const CStringA category = "General";

	//:WARNING - essential
	if (source.FindValue(category) == nullptr) {
		RETURN_FALSE;
	}

	Json::Object& data = source.GetAt(category);
	A3DRWParamsGeneralData& param = target.m_sGeneral;

	int readingMode = data.GetInteger("ReadingMode");
	int unit = data.GetInteger("GeneralUnits");

	//param.m_usStructSize;
	param.m_bReadSolids = data.GetBoolean("Solids");
	param.m_bReadSurfaces = data.GetBoolean("Surfaces");
	param.m_bReadWireframes = data.GetBoolean("Wireframes");
	//:WARNING - process in GetPMI()
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

/*
{"type":"group", "name":"Tessellation", "title":"Tessellation", "items":[
	{"type":"drop", "name":"TessLevel", "title":"Predefined Tessellation Level|__#__", "value":2, "items":[
		"Extra Low|__#__",
		"Low|__#__",
		"Medium|__#__",
		"High|__#__",
		"Extra High|__#__",
		"Custom|__#__"
	]},
	{"type":"group", "name":"CustomTessLevel", "title":"Custom Tessellation Level", "enable":false, "items":[
		{"type":"drop", "name":"ChordLimitType", "title":"Chord Limit|__#__", "desc":"__#__|__#__", "value":0, "items":[
			"Ratio|__#__",
			"Height|__#__"
		]},
		{"type":"slider", "name":"ChordLimit", "title":"Limit|__#__", "desc":"__#__|__#__", "min":50, "max":10000, "value":2000},
		{"type":"slider", "name":"AngleTolerance", "title":"Angle Tolerance|__#__", "desc":"__#__|__#__", "min":10, "max":40, "value":40}
	]},
	{"type":"check", "name":"PreserveUV", "title":"Preserve UV Points|__#__", "desc":"__#__|__#__"},
	{"type":"edit", "name":"MaxEdgeLength", "title":"Maximum Edge Length|__#__", "desc":"__#__|__#__", "value":"0"},
	{"type":"group", "name":"AccurateTess", "title":"Accurate Tessellation|__#__", "desc":"__#__|__#__", "hasCheck":true, "items":[
		{"type":"group", "name":"GridAlignedTess", "title":"Grid Aligned Tessellation|__#__", "desc":"__#__|__#__", "hasCheck":true, "items":[
			{"type":"edit", "name":"MaxStitchLength", "title":"Maximum Stitch Length|__#__", "desc":"__#__|__#__", "value":"0.001"},
			{"type":"check", "name":"SurfaceCurvatures", "title":"Adapt to Surface Curvatures|__#__", "desc":"__#__|__#__"}
		]}
	]}
]}
*/

// + "TessLevel"
// + "CustomTessLevel"
// + "ChordLimitType"
// + "ChordLimit"
// + "AngleTolerance"
// + "PreserveUV"
// + "MaxEdgeLength"
// + "AccurateTess"
// + "GridAlignedTess"
// + "MaxStitchLength"
// + "SurfaceCurvatures"

bool H3DX::ImportOptions::GetTessellation(Json::Object& source, A3DRWParamsLoadData& target)
{
	const CStringA category = "Tessellation";

	if (source.FindValue(category) != nullptr) {
		Json::Object& data = source.GetAt(category);
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
		//param.m_bDoNotComputeNormalsInAccurateTessellation;
		param.m_bKeepUVPoints = data.GetBoolean("PreserveUV");
		param.m_bUseHeightInsteadOfRatio = custom.GetInteger("ChordLimitType") == 1; //:CHECK
		param.m_dMaximalTriangleEdgeLength = (double)data.GetInteger("TessMaxEdgeLength"); //:CHECK
	}
	else {
		//:TODO - default?
	}

	return true;
}

/*
{"type":"group", "name":"Search", "title":"Search|__#__", "items":[
	{"type":"folder", "name":"Folders", "title":"Folders|__#__", "desc":"Select search folders for assemblies and textures|__#__"},
	{"type":"check", "name":"RecursiveSearch", "title":"Recursive Search|__#__", "desc":"__#__|__#__"},
	{"type":"edit", "name":"SearchMaxEdgeLength", "title":"Maximum Edge Length|__#__", "desc":"__#__|__#__", "value":"0"}
]}
*/

// + "Folders"
// + "RecursiveSearch"
// - "SearchMaxEdgeLength"

bool H3DX::ImportOptions::GetSearch(Json::Object& source, A3DRWParamsLoadData& target)
{
	const CStringA category = "Search";

	//:WARNING - essential
	if (source.FindValue(category) == nullptr) {
		RETURN_FALSE;
	}

	Json::Object& data = source.GetAt(category);
	A3DRWParamsAssemblyData& param = target.m_sAssembly;

	bool recursive = data.GetBoolean("RecursiveSearch");

	//param.m_usStructSize;
	param.m_bUseRootDirectory = recursive; //:CHECK
	param.m_bRootDirRecursive = recursive;

	WStringArray strings;
	if (WStr::Split(data.GetString("Folders").GetBuffer(), L';', strings) && strings.size() > 0) {
		int length = strings.size();
		param.m_uiSearchDirectoriesSize = length;

		A3DRWParamsSearchDirData** pBuffer
			= param.m_ppcSearchDirectories
			= new A3DRWParamsSearchDirData*[length];
		DEBUG_VALID(pBuffer);
		::ZeroMemory(pBuffer, sizeof(A3DRWParamsSearchDirData*) * length);

		for (int i = 0; i < strings.size(); i++) {
			CString dir = strings[i];
			dir.Replace(L'/', L'\\');

			A3DRWParamsSearchDirData* pDir
				= pBuffer[i]
				= new A3DRWParamsSearchDirData;
			DEBUG_VALID(pDir);
			::ZeroMemory(pDir, sizeof(A3DRWParamsSearchDirData));

			//pDir->m_usStructSize;
			pDir->m_pcPhysicalPath = ToHoopsString(dir);
			//pDir->m_pcLogicalName
			pDir->m_bRecursive = recursive; //:CHECK
		}
	}

	//param.m_uiPathDefinitionsSize;
	//param.m_ppcPathDefinitions;

	return true;
}

/*
{"type":"group", "name":"PMI", "title":"PMI|__#__", "hasCheck":true, "items":[
	{"type":"check", "name":"AutomaticOrientation", "title":"Automatic Orientation|__#__", "desc":"__#__|__#__"},
	{"type":"drop", "name":"PMIUnits", "title":"Units|__#__", "desc":"__#__|__#__", "value":0, "items":[
		"Unknown|__#__",
		"Point(pt)|__#__",
		"Inch(in)|__#__",
		"Millimeter(mm)|__#__",
		"Centimeter(cm)|__#__",
		"Picas(pica)|__#__",
		"Foot(ft)|__#__",
		"Yard(yd)|__#__",
		"Meter(m)|__#__",
		"Kilometer(km)|__#__",
		"Mile(mi)|__#__"
	]},
	{"type":"color", "name":"PMIColor", "title":"Color|__#__", "desc":"__#__|__#__", "value":"0x000000"},
	{"type":"check", "name":"SubstituteFont", "title":"Always Substitute Font|__#__", "desc":"__#__|__#__"},
	{"type":"font", "name":"PMIFont", "title":"Substitution Font|__#__", "desc":"__#__|__#__", "value":"Myraid CAD"}
]}
*/

// - "AutomaticOrientation"
// + "PMIUnits"
// + "PMIColor"
// + "SubstituteFont"
// + "PMIFont"

bool H3DX::ImportOptions::GetPMI(Json::Object& source, A3DRWParamsLoadData& target)
{
	const CStringA category = "PMI";

	if (source.FindValue(category) != nullptr) {
		Json::Object& data = source.GetAt(category);
		if (data.GetBoolean("checked") == false) {
			return true;
		}

		//:CHECK
		target.m_sGeneral.m_bReadPmis = true;

		A3DRWParamsPmiData& param = target.m_sPmi;

		int unit = data.GetInteger("TessUnits");

		//param.m_usStructSize;
		param.m_bAlwaysSubstituteFont = data.GetBoolean("SubstituteFont");
		param.m_pcSubstitutionFont = ToHoopsString("Myriad CAD"); //ToHoopsString(data.GetString("PMIFont"));
		//param.m_iNumberOfDigitsAfterDot;
		param.m_eDefaultUnit = unit == 0 ? kA3DUnitUnknown : (A3DEUnits)(unit - 1);
		//param.m_uiProprietaryFontDirectoriesSize;
		//param.m_ppcProprietaryFontDirectories;
		param.m_sDefaultColor = ToHoopsColor(data.GetString("PMIColor"));
		//param.m_bAlwaysUseDefaultColor;
	}
	else {
		//:TODO - default?
	}

	return true;
}



bool H3DX::ImportOptions::GetSpecial(CStringA fileType, Json::Object& source, A3DRWParamsLoadData& target)
{
	const CStringA category = "Special";

	if (source.FindValue(category) == nullptr) {
		return true;
	}

	//target.m_usStructSize;
	//   A3DRWParamsCatiaV4Data m_sCatiaV4;
	// + A3DRWParamsCatiaV5Data m_sCatiaV5;
	// + A3DRWParamsUnigraphicsData m_sUnigraphics;
	// + A3DRWParamsProEData m_sProE;
	//   A3DRWParamsStepData m_sStep;
	//   A3DRWParamsIGESData m_sIGES;
	//   A3DRWParamsIFCData m_sIFC; // V5.2
	// + A3DRWParamsJTData m_sJT; // V8.0
	//   A3DRWParamsParasolidData m_sParasolid; // V8.1
	// + A3DRWParamsSolidworksData m_sSolidworks; // V8.2
	//   A3DRWParamsInventorData m_sInventor; // V9.0
	//   A3DRWParamsRhinoData m_sRhino; // V11.2
	//   A3DRWParamsRevitData m_sRevit; // V13.1
	//   A3DRWParamsSolidEdgeData m_sSolidEdge; // V23.1

	Json::Object& data = source.GetAt(category);
	A3DRWParamsSpecificLoadData& special = target.m_sSpecifics;

	//:TODO

	if (fileType == "") {
		return true;
	}
	else if (fileType == "ACAD") {
		//:TODO
		// - "AsDrawing"
	}
	else if (fileType == "Inventor") {
		// + "EmbededTess"

		A3DRWParamsInventorData& param = special.m_sInventor;

		//param.m_usStructSize;
		param.m_bUseTessForFile = data.GetBoolean("EmbededTess");
	}
	else if (fileType == "CATIA5") {
		// + "DiskCache"
		// + "CacheLocation"
		// + "MaterialRendering"

		A3DRWParamsCatiaV5Data& param = special.m_sCatiaV5;

		//param.m_usStructSize;
		param.m_bCacheActivation = data.GetAt("DiskCache").GetBoolean("checked");
		param.m_pcCachePath = ToHoopsString(data.GetString("CacheLocation"));
		param.m_bUseMaterialRendering = data.GetBoolean("MaterialRendering");
	}
	else if (fileType == "ProE") {
		// + "DimensionTolerance"
		// + "MissingBoolean"
		// + "MissingFlexible"
		// + "SubPartPMI"
		// + "Datum"
		// + "Skeletons"
		// + "Construction"
		// + "FamilyTable"
		// + "CodePageName"
		// + "SessionColor"

		A3DRWParamsProEData& param = special.m_sProE;

		Json::Object& display = data.GetAt("Display");
		Json::Object& import = data.GetAt("Import");
		int sessionColor = data.GetInteger("SessionColor");

		//param.m_usStructSize;
		param.m_pcCodePageName = ToHoopsString(data.GetString("CodePageName"));
		param.m_bDisplayTolerance = display.GetBoolean("DimensionTolerance");
		param.m_bDisplaySubpartAnnotations = import.GetBoolean("SubPartPMI");

		switch (sessionColor) {
		case 0: param.m_eSessionColorType = kA3DHExchangeSessionColor; break;
		case 1: param.m_eSessionColorType = kA3DLastCreoVersionSessionColor; break;
		case 2: param.m_eSessionColorType = kA3DFileVersionSessionColor; break; //:CHECK

		default:
			DEBUG_STOP;
			break;
		}

		param.m_eFamilyTables = (A3DProEFamilyTables)data.GetInteger("FamilyTable");
		param.m_bBoolOpUseGenericIfNoTess = display.GetBoolean("MissingBoolean");
		param.m_bFlexCompUseGenericIfNoTess = display.GetBoolean("MissingFlexible");
		param.m_bHideSkeletons = !import.GetBoolean("Skeletons"); //:WARNING - reverse value
		//param.m_bReadExplodeStateAsView;
		param.m_bDisplayVisibleDatum = import.GetBoolean("Datum");
		param.m_eReadConstructEntities = import.GetBoolean("Construction")
			? A3DProEReadConstructEntities_Yes : A3DProEReadConstructEntities_No; //:CHECK
		//param.m_bComputeHomeView;
		//param.m_bHandlePMIScreenLocation;
		//param.m_bIsometricDefaultView;
	}
	else if (fileType == "JT") {
		// + "LevelOfDetail"

		A3DRWParamsJTData& param = special.m_sJT;

		//param.m_usStructSize;
		param.m_eReadTessellationLevelOfDetail = (A3DEJTReadTessellationLevelOfDetail)data.GetInteger("LevelOfDetail");
	}
	else if (fileType == "NX") {
		// + "RecomputeCameras"

		A3DRWParamsUnigraphicsData& param = special.m_sUnigraphics;

		//param.m_usStructSize;
		//param.m_bApplyToAllLevels;
		//param.m_uiPreferredReferenceSetsSize;
		//param.m_ppcPreferredReferenceSets;
		param.m_bFitAllToUpdateViewCameras = data.GetBoolean("RecomputeCameras");
	}
	else if (fileType == "SolidWorks") {
		// + "VisibleDatum"

		A3DRWParamsSolidworksData& param = special.m_sSolidworks;

		//param.m_usStructSize;
		//param.m_bLoadAllConfigsData;
		param.m_usDisplayVisibleDatum = data.GetBoolean("VisibleDatum") ? 1 : 0;

	}
		
	return true;
}

/*
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

CString H3DX::ImportOptions::GetFileTypeName(CString filePath)
{
	CString extension = Path::GetExtension(filePath.GetBuffer());
	extension.MakeUpper();

	if (WStr::IsDigit(extension)) {
		return "ProE";
	}

	for (auto item : m_reference.GetArray("items").GetBuffer()) {
		Json::Object& import = item->AsObject();
		if (import.GetBoolean("visible", true) == false) {
			continue;
		}

		for (auto ext : import.GetArray("ext").GetBuffer()) {
			if (ext->AsString() == extension) {
				return import.GetString("name");
			}
		}
	}

	DEBUG_STOP;
	return L"";
}



A3DGraphRgbColorData H3DX::ImportOptions::ToHoopsColor(CString value)
{
	A3DGraphRgbColorData data;
	A3D_INITIALIZE_DATA(A3DGraphRgbColorData, data);

	COLORREF color = Json::Helper::ToColor(value);
	data.m_dRed = GetRValue(color) / 255.0;
	data.m_dGreen = GetGValue(color) / 255.0;
	data.m_dBlue = GetBValue(color) / 255.0;

	return data;
}



A3DUTF8Char* H3DX::ImportOptions::ToHoopsString(CString value)
{
	A3DUTF8Char* pBuffer = nullptr;
	if (value.IsEmpty()) {
		return NULL;
	}

	int length = value.GetLength() * sizeof(WCHAR);
	pBuffer = new A3DUTF8Char[length];
	DEBUG_VALID(pBuffer);
	::ZeroMemory(pBuffer, length);

	A3DStatus eStatus = A3DMiscUnicodeToUTF8((A3DUTF8Char*)(LPCTSTR)value, pBuffer);
	if (A3D_SUCCESS != eStatus) {
		REMOVE_ARRAY(pBuffer);
		DEBUG_STOP;
	}

	return pBuffer;
}



A3DUTF8Char** H3DX::ImportOptions::ToHoopsStrings(CString value)
{
	WStringArray buffer;
	if (WStr::Split(value.GetBuffer(), L';', buffer) == false || buffer.size() == 0) {
		return NULL;
	}

	A3DUTF8Char** pBuffer = new A3DUTF8Char*[buffer.size()];
	DEBUG_VALID(pBuffer);
	::ZeroMemory(pBuffer, sizeof(A3DUTF8Char*) * buffer.size());

	for (int i = 0; i < buffer.size(); i++) {
		pBuffer[i] = ToHoopsString(buffer[i]);
		DEBUG_VALID(pBuffer[i]);
	}

	return pBuffer;
}

#pragma endregion //:REGION

//--------------------------------------------------------------------------------------------------

#pragma region ExportOptions Class

H3DX::ExportOptions::ExportOptions()
{
}



H3DX::ExportOptions::~ExportOptions()
{
}



bool H3DX::ExportOptions::Set(Json::Object& value)
{
	const CStringA category = "Export";

	if (value.FindValue(category) == nullptr) {
		RETURN_FALSE;
	}

	JsonWrapper::Set(value.GetAt(category));

	return true;
}



bool H3DX::ExportOptions::SetReference(Json::Object& value)
{
	ASSERT(value.IsEmpty() == false && m_reference.IsEmpty());
	m_reference = value;

	RETURN_TRUE;
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



bool H3DX::FileOptions::Set(Json::Object& value)
{
	//:TODO - version check
	ASSERT(value.GetString("version") == L"240126.1");

	bool success = true;
	success &= Import.Set(value.GetAt("Import"));
	//:TODO
	//success &= Export.Set(value.GetAt("Export"));

	return success;
}



bool H3DX::FileOptions::SetReference(Json::Object& value)
{
	bool success = true;
	success &= Import.SetReference(value.GetAt("Import"));
	//:TODO
	//success &= Export.SetReference(value.GetAt("Export"));
	
	return success;
}

#pragma endregion //:REGION
