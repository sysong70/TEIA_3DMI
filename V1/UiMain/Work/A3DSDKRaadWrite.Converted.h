typedef enum
{
	kA3DRead_3D,       /* Read 3D Only */
	kA3DRead_Drawings, /* Read Drawings only */
	kA3DRead_Both,     /* Read 3D and Drawings */
} A3DEReadingMode2D3D;

typedef enum
{
	kA3DReadGeomOnly,    /* In this mode, Exchange will avoid reading or generating tessellation on B-rep. Faceted elements from the native file will still be imported.*/
	kA3DReadGeomAndTess, /* Mixed mode: In this mode, the B-rep and faceted elements are read from the native file. Tessellation is then generated from B-rep elements. */
	kA3DReadTessOnly,    /* In this mode, all faceted elements from the native file are read. If present, the native tessellation of the B-rep is read exclusively. Native tessellation is the display list integrated in the CAD file (only available for CATIA V5, Solidworks, 3DXML, and Inventor). \n\n There are some side effects: The tessellation may be not up-to-date with the geometry, the assembly tree can be different, and hidden objects may be missed. \n\n If an element contains only B-rep, tessellation is generated from the B-rep and subsequently the B-rep is removed to save memory. */
} A3DEReadGeomTessMode;

typedef enum
{
	kA3DUnitPoint,        /* Point */
	kA3DUnitInch,         /* Inch */
	kA3DUnitMillimeter,   /* Millimeter */
	kA3DUnitCentimeter,   /* Centimeter */
	kA3DUnitPicas,        /* Picas */
	kA3DUnitFoot,         /* Foot */
	kA3DUnitYard,         /* Yard */
	kA3DUnitMeter,        /* Meter */
	kA3DUnitKilometer,    /* Kilometer */
	kA3DUnitMile,         /* Mile */
	kA3DUnitMicroInch,    /* Micro Inch */
	kA3DUnitMil,          /* Mil */
	kA3DUnitAngstrom,     /* Angstrom */
	kA3DUnitNanometer,    /* Nanometer */
	kA3DUnitMicron,       /* Micron */
	kA3DUnitDecimeter,    /* Decimeter */
	kA3DUnitDekameter,    /* Dekameter */
	kA3DUnitHectometer,   /* Hectometer */
	kA3DUnitGigameter,    /* Gigameter */
	kA3DUnitAstronomical, /* Astronomical */
	kA3DUnitLightYear,    /* Light year */
	kA3DUnitParsec,	      /* Parsec */
	kA3DUnitUSSurveyFoot, /* US Survey foot */
	kA3DUnitUSSurveyInch, /* US survey inch */
	kA3DUnitUSSurveyYard, /* US survey yard */
	kA3DUnitUSSurveyMile, /* US survey mile */
	kA3DUnitUnknown,      /* Unknown */
} A3DEUnits;

typedef enum
{
	// A3DRWParamsTessellationData::m_dChordHeightRatio = 50
	// A3DRWParamsTessellationData::m_dAngleToleranceDeg = 40
	kA3DTessLODExtraLow,            /* Extra Low level */
	// A3DRWParamsTessellationData::m_dChordHeightRatio = 600
	// A3DRWParamsTessellationData::m_dAngleToleranceDeg = 40
	kA3DTessLODLow,                 /* Low level */
	// A3DRWParamsTessellationData::m_dChordHeightRatio = 2000
	// A3DRWParamsTessellationData::m_dAngleToleranceDeg = 40
	kA3DTessLODMedium,              /* Medium level */
	// A3DRWParamsTessellationData::m_dChordHeightRatio = 5000
	// A3DRWParamsTessellationData::m_dAngleToleranceDeg = 30
	kA3DTessLODHigh,                /* High level */
	// A3DRWParamsTessellationData::m_dChordHeightRatio = 10000
	// A3DRWParamsTessellationData::m_dAngleToleranceDeg = 20
	kA3DTessLODExtraHigh,           /* Extra High level */
	// when selected, these members should be defined
	// A3DRWParamsTessellationData::m_dChordHeightRatio if A3DRWParamsTessellationData::m_bUseHeightInsteadOfRatio is set to false
	// A3DRWParamsTessellationData::m_dMaxChordHeight if A3DRWParamsTessellationData::m_bUseHeightInsteadOfRatio is set to true
	// A3DRWParamsTessellationData::m_dAngleToleranceDeg. Be very careful when using A3DRWParamsTessellationData::m_dMaxChordHeight because a too small value would generate a huge tessellation.
	kA3DTessLODUserDefined,         /* User Defined level */
	// \deprecated when selected, these members should be defined
	// A3DRWParamsTessellationData::m_dAngleToleranceDeg
	// A3DRWParamsTessellationData::m_dMaxChordHeight
	// A3DRWParamsTessellationData::m_dMinimalTriangleAngleDeg
	kA3DTessLODControlledPrecision, /* Controlled Precision level */
} A3DETessellationLevelOfDetail;

typedef enum
{
	kA3DLastCreoVersionSessionColor, /* Sets the default session color corresponding to the last version of Creo that HOOPS Exchange supports. */
	kA3DHExchangeSessionColor,       /* Uses HOOPS Exchange default color. */
	kA3DFileVersionSessionColor,     /* Lets the user define the color by using graphics parameters options (A3DRWParamsPmiData::m_sDefaultColor). */
} A3DProESessionColorType;

typedef enum
{
	A3DProEFamTabAcceleratorFileOnly, /* Only use accelerator file. If there's tessellation or a generic part, these will not be loaded, even if an accelerator file isn't available. */
	A3DProEFamTabOrUseTessellation,   /* If the accelerator file isn't present, then search for the tessellation representation: if found, then use the tessellation. If an accelerator file or tessellation isn't available, then nothing will be loaded. */
	A3DProEFamTabOrUseWireAndGeneric, /* If neither the accelerator file nor the tessellation representation is present, then use the generic or wire representation. \n \warning If using the generic or wire representation, be aware that the representation will not be what it should be! A generic part/wire is only a placeholder. */
} A3DProEFamilyTables;

typedef enum
{
 A3DProEReadConstructEntities_AsDatum, /*  Read wire according to the datum reading option*/
 A3DProEReadConstructEntities_Yes,     /* Read wire*/
 A3DProEReadConstructEntities_No,      /* Do not read wire*/
} A3DProEReadConstructEntities;

typedef enum
{
	kA3DStepNameFromNAUO_ID,          /* First Field of NEXT_ASSEMBLY_USAGE_OCCURRENCE. */
	kA3DStepNameFromNAUO_NAME,        /* Second Field of NEXT_ASSEMBLY_USAGE_OCCURRENCE. */
	kA3DStepNameFromNAUO_DESCRIPTION, /* Third Field of NEXT_ASSEMBLY_USAGE_OCCURRENCE. */
} A3DEStepNameFromNAUO;

typedef enum
{
	kA3DJTTessLODLow,     /* Will load the lowest level of tessellation available in the JT file. */
	kA3DJTTessLODMedium,  /* Will load an in-between level of tessellation available in the JT file. */
	kA3DJTTessLODHigh,    /* Will load the highest level of tessellation available in the JT file. */
} A3DEJTReadTessellationLevelOfDetail;

typedef enum
{
	kA3DRevitPhysicalProperties_NotComputed, /* Not computed. Default value */
	kA3DRevitPhysicalProperties_Computed,    /* Computed during reading process, stored as attributes */
} EA3DRevitPhysicalProperties;

typedef enum
{
	kA3DCompressionLow,    /* Compression with tolerance set to 0.001 mm (low compression, high accuracy). */
	kA3DCompressionMedium, /* Compression with tolerance set to 0.01 mm (medium compression, medium accuracy). */
	kA3DCompressionHigh,   /* Compression with tolerance set to 0.1 mm (high compression, low accuracy). */
} A3DECompressBrepType;

typedef enum
{
	kA3DECMA1, /* ECMA-363, version 1 writing. (Acrobat Reader 7.0 compatible). */
	kA3DECMA3, /* ECMA-363, version 3 writing. */
} A3DEU3DVersion;

typedef enum
{
	kA3DStepAP203, /* AP 203 Ed 2 since \version 9.1 */
	kA3DStepAP214, /* AP 214. */
	kA3DStepAP242, /* AP 242. \version 9.1 */
} A3DEStepFormat;

typedef enum
{
	kA3DWriteGeomOnly,    /* Write only geometry. */
	kA3DWriteGeomAndTess, /* Mixed mode: write geometry and tessellation. */
	kA3DWriteTessOnly,    /* Write only tessellation. */
} A3DEWriteGeomTessMode;

typedef enum
{
	kA3DE_JT81,  /* JT version 8.1 writing. */
	kA3DE_JT95,  /* JT version 9.5 writing. */
	kA3DE_JT100, /* JT version 10.0 writing. */
} A3DEJTVersion;

typedef struct
{
    unsigned short m_usStructSize;
    double         m_dRed;
    double         m_dGreen;
    double         m_dBlue;
} A3DGraphRgbColorData;





typedef struct
{
	unsigned short       m_usStructSize;
	unsigned char        m_bReadSolids;
	unsigned char        m_bReadSurfaces;
	unsigned char        m_bReadWireframes;
	unsigned char        m_bReadPmis;
	unsigned char        m_bReadAttributes;
	unsigned char        m_bReadHiddenObjects;
	unsigned char        m_bReadConstructionAndReferences;
	unsigned char        m_bReadActiveFilter;
	A3DEReadingMode2D3D  m_eReadingMode2D3D;
	A3DEReadGeomTessMode m_eReadGeomTessMode;
	A3DEUnits            m_eDefaultUnit;
	unsigned char        m_bReadFeature;
	unsigned char        m_bReadConstraints;
	long                 m_iNbMultiProcess;
	unsigned long        m_uiSearchTextureDirectoriesSize;
	char**               m_ppcSearchTextureDirectories;
} A3DRWParamsGeneralData;

typedef struct
{
	unsigned short       m_usStructSize;
	unsigned char        m_bAlwaysSubstituteFont;
	char*                m_pcSubstitutionFont;
	int                  m_iNumberOfDigitsAfterDot;
	A3DEUnits            m_eDefaultUnit;
	unsigned long        m_uiProprietaryFontDirectoriesSize;
	char**               m_ppcProprietaryFontDirectories;
	A3DGraphRgbColorData m_sDefaultColor;
	unsigned char        m_bAlwaysUseDefaultColor;
} A3DRWParamsPmiData;

typedef struct
{
	unsigned short                m_usStructSize;
	A3DETessellationLevelOfDetail m_eTessellationLevelOfDetail;
	double                        m_dChordHeightRatio;
	double                        m_dAngleToleranceDeg;
	double                        m_dMinimalTriangleAngleDeg;
	double                        m_dMaxChordHeight;

	unsigned char                 m_bAccurateTessellation;
	unsigned char                 m_bAccurateTessellationWithGrid;
	double                        m_dAccurateTessellationWithGridMaximumStitchLength;
	unsigned char                 m_bAccurateSurfaceCurvatures;

	unsigned char                 m_bDoNotComputeNormalsInAccurateTessellation;

	unsigned char                 m_bKeepUVPoints;
	unsigned char                 m_bUseHeightInsteadOfRatio;
	double                        m_dMaximalTriangleEdgeLength;
} A3DRWParamsTessellationData;

typedef struct
{
	unsigned short                m_usStructSize;
	char*                         m_pcPhysicalPath;
	char*                         m_pcLogicalName;
	unsigned char                 m_bRecursive;
} A3DRWParamsSearchDirData;

typedef struct
{
	unsigned short             m_usStructSize;
	unsigned char              m_bUseRootDirectory;
	unsigned char              m_bRootDirRecursive;
	unsigned long              m_uiSearchDirectoriesSize;
	A3DRWParamsSearchDirData** m_ppcSearchDirectories;
	unsigned long              m_uiPathDefinitionsSize;
	char**                     m_ppcPathDefinitions;
} A3DRWParamsAssemblyData;

typedef struct
{
	unsigned short m_usStructSize;
	unsigned char  m_bLoadDefault;
	unsigned long  m_uiEntriesSize;
	char**         m_ppcEntries;
} A3DRWParamsMultiEntriesData;

typedef struct
{
	unsigned short m_usStructSize;
	char*          m_pcRootDirLogicalName;
	unsigned char  m_bAllowSearchInOtherLogicalNames;
} A3DRWParamsCatiaV4Data;

typedef struct
{
	unsigned short m_usStructSize;
	unsigned char  m_bCacheActivation;
	char*          m_pcCachePath;
	unsigned char  m_bUseMaterialRendering;
} A3DRWParamsCatiaV5Data;

typedef struct
{
	unsigned short m_usStructSize;
	unsigned char  m_bApplyToAllLevels;
	unsigned long  m_uiPreferredReferenceSetsSize;
	char**         m_ppcPreferredReferenceSets;
	unsigned char  m_bFitAllToUpdateViewCameras;
} A3DRWParamsUnigraphicsData;

typedef struct
{
	unsigned short               m_usStructSize;
	char*                        m_pcCodePageName;
	unsigned char                m_bDisplayTolerance;
	unsigned char                m_bDisplaySubpartAnnotations;
	A3DProESessionColorType      m_eSessionColorType;
	A3DProEFamilyTables          m_eFamilyTables;
	unsigned char                m_bBoolOpUseGenericIfNoTess;
	unsigned char                m_bFlexCompUseGenericIfNoTess;
	unsigned char                m_bHideSkeletons;
	unsigned char                m_bReadExplodeStateAsView;
	unsigned char                m_bDisplayVisibleDatum;
	A3DProEReadConstructEntities m_eReadConstructEntities;
	unsigned char                m_bComputeHomeView;
	unsigned char                m_bHandlePMIScreenLocation;
	unsigned char                m_bIsometricDefaultView;
} A3DRWParamsProEData;

typedef struct
{
	unsigned short m_usStructSize;

	// Geometry
	double         m_dGEOMPercentVolume;
	double         m_dGEOMPercentSurfaceArea;
	double         m_dGEOMPercentIndependentSurfaceArea;
	double         m_dGEOMPercentIndependentCurveLength;

	double         m_dGEOMDeviationCentroid;
	double         m_dGEOMDeviationIndependentSurfaceCentroid;
	double         m_dGEOMDeviationIndependentCurveCentroid;
	double         m_dGEOMDeviationPointSetCentroid;

	double         m_dGEOMDeviationBoundingBox;

	//PMI
	double         m_dPMIPercentAffectedArea;
	double         m_dPMIPercentAffectedCurveLength;
	double         m_dPMIPercentCurveLength;
	double         m_dPMIPercentSurfaceArea;
	double         m_dPMIDeviationCurveCentroid;
	double         m_dPMIDeviationSurfaceCentroid;

	//ASSEMBLY
	double         m_dASMDeviationNotionalSolidsCentroid;

} A3DRWValidationPropertiesThresholdData;

typedef struct
{
	unsigned short                         m_usStructSize;
	unsigned char                          m_bPreferProductName;
	unsigned char                          m_bPreferFirstColor;
	A3DEStepNameFromNAUO                   m_eNameFromNAUO;
	char*                                  m_pcCodePageName;
	unsigned char                          m_bSplitSHELL_BASED_SURFACE_MODEL;
	unsigned char                          m_bHealOrientations;
	unsigned char                          m_bReadValidationProperties;
	unsigned char                          m_bComputeValidationProperties;
	unsigned char                          m_bAddResultToName;
	A3DRWValidationPropertiesThresholdData m_sValidationPropertiesThreshold;
} A3DRWParamsStepData;

typedef struct
{
	unsigned short m_usStructSize;
	unsigned char  m_bSewBrepModels;
} A3DRWParamsIGESData;

typedef struct
{
	unsigned short m_usStructSize;
	char*          m_pcCodePageName;
	char*          m_pcXMLFilePathForAttributes;
	unsigned char  m_bIFCOWNERHISTORYOptimized;
	unsigned char  m_bFACETED_BREPAsOneFace;
	unsigned char  m_bAttributesOnlyGlobalId;
	unsigned char  m_bHideWireframes;
	unsigned char  m_bReadRelationships;
	unsigned char  m_bReadOpeningElements;
} A3DRWParamsIFCData;

typedef struct
{
	unsigned short                      m_usStructSize;
	A3DEJTReadTessellationLevelOfDetail m_eReadTessellationLevelOfDetail;
} A3DRWParamsJTData;

typedef struct
{
	unsigned short m_usStructSize;
	unsigned char  m_bKeepParsedEntities;
} A3DRWParamsParasolidData;

typedef struct
{
	unsigned short m_usStructSize;
	unsigned char  m_bLoadAllConfigsData;
	unsigned short m_usDisplayVisibleDatum;
} A3DRWParamsSolidworksData;

typedef struct
{
	unsigned short m_usStructSize;
	unsigned char  m_bUseTessForFile;
} A3DRWParamsInventorData;

typedef struct
{
	unsigned short m_usStructSize;
	unsigned char  m_bForceRenderedModeColors;
} A3DRWParamsRhinoData;

typedef struct
{
	unsigned short              m_usStructSize;
	EA3DRevitPhysicalProperties m_ePhysicalProperties;
	unsigned long               m_uiLinkFlags;
} A3DRWParamsRevitData;

typedef struct
{
	unsigned short             m_usStructSize;
	A3DRWParamsCatiaV4Data     m_sCatiaV4;
	A3DRWParamsCatiaV5Data     m_sCatiaV5;
	A3DRWParamsUnigraphicsData m_sUnigraphics;
	A3DRWParamsProEData        m_sProE;
	A3DRWParamsStepData        m_sStep;
	A3DRWParamsIGESData        m_sIGES;
	A3DRWParamsIFCData         m_sIFC;
	A3DRWParamsJTData          m_sJT;
	A3DRWParamsParasolidData   m_sParasolid;
	A3DRWParamsSolidworksData  m_sSolidworks;
	A3DRWParamsInventorData    m_sInventor;
	A3DRWParamsRhinoData       m_sRhino;
	A3DRWParamsRevitData       m_sRevit;
} A3DRWParamsSpecificLoadData;

typedef struct
{
	unsigned short            m_usStructSize;
	unsigned char             m_bLoadStructureOnly;
	unsigned char             m_bLoadNoDependencies;
	A3DAsmProductOccurrence*  m_pRootProductOccurrence;
	unsigned long             m_uiProductOccurrencesSize;
	A3DAsmProductOccurrence** m_ppProductOccurrences;
} A3DRWParamsIncrementalLoadData;

typedef struct
{
	unsigned short                 m_usStructSize;
	A3DRWParamsGeneralData         m_sGeneral;
	A3DRWParamsPmiData             m_sPmi;
	A3DRWParamsTessellationData    m_sTessellation;
	A3DRWParamsAssemblyData        m_sAssembly;
	A3DRWParamsMultiEntriesData    m_sMultiEntries;
	A3DRWParamsSpecificLoadData    m_sSpecifics;
	A3DRWParamsIncrementalLoadData m_sIncremental;
} A3DRWParamsLoadData;

typedef struct
{
	unsigned short m_usStructSize;
	unsigned long  m_uiStreamSize;
	char*          m_acStream;
	unsigned char  m_bIsPrc;
	unsigned long  m_uiIdentifier;
}A3DStream3DPDFData;

typedef struct
{
	unsigned short       m_usStructSize;
	unsigned char        m_bCompressBrep;
	unsigned char        m_bCompressTessellation;
	A3DECompressBrepType m_eCompressBrepType;
	unsigned char        m_bRemoveBRep;
	unsigned char        m_bRemoveAttributes;
} A3DRWParamsExportPrcData;

typedef struct
{
	unsigned short m_usStructSize;
	A3DEU3DVersion m_eU3DVersion;
	unsigned char  m_bMeshQuality;
	unsigned char  m_ucMeshQualityValue;
} A3DRWParamsExportU3DData;

typedef struct
{
	unsigned short m_usStructSize;
	A3DEStepFormat m_eStepFormat;
	unsigned char  m_bSaveFacetedToWireframe;
	unsigned char  m_bSaveAnalyticsToNurbs;
	unsigned char  m_bUseShortNames;
	char*          m_pcApplication;
	char*          m_pcVersion;
	unsigned char  m_bWritePMI;
	unsigned char  m_bWriteAttributes;
	unsigned char  m_bWriteUVCurves;
	char*          m_pcConfig;
	unsigned char  m_bWriteValidationProperties;
	unsigned char  m_bWritePMIWithSemantic;
	unsigned char  m_bWritePMIAsTessellated;
	char*          m_pcUser;
	char*          m_pcOrganisation;
	char*          m_pcAuthorisation;
} A3DRWParamsExportStepData;

typedef struct
{
	unsigned short        m_usStructSize;
	A3DEWriteGeomTessMode m_eWriteGeomTessMode;
	unsigned char         m_bWriteHiddenObjects;
	unsigned char         m_bWritePMI;
	A3DEJTVersion         m_eJTVersion;
} A3DRWParamsExportJTData;

typedef struct
{
	unsigned short m_usStructSize;
	unsigned char  m_bSaveAnalyticsToNurbs;
	unsigned char  m_bSaveFacetedToWireframe;
	unsigned char  m_bSaveSolidsAsFaces;
	unsigned char  m_bWriteHiddenObjects;
	unsigned char  m_bWriteTessellation;
	char*          m_pcApplication;
	char*          m_pcVersion;
} A3DRWParamsExportIgesData;

typedef struct
{
	unsigned short                m_usStructSize;
	unsigned char                 m_bBinaryFile;
	A3DETessellationLevelOfDetail m_eTessellationLevelOfDetail;
	double                        m_dChordHeightRatio;
	double                        m_dAngleToleranceDeg;
	double                        m_dMinimalTriangleAngleDeg;
	double                        m_dMaxChordHeight;

	unsigned char                 m_bAccurateTessellation;
	unsigned char                 m_bAccurateTessellationWithGrid;
	double                        m_dAccurateTessellationWithGridMaximumStitchLength;
	unsigned char                 m_bAccurateSurfaceCurvatures;

	unsigned char                 m_bKeepCurrentTessellation;
	unsigned char                 m_bUseHeightInsteadOfRatio;
	double                        m_dMaximalTriangleEdgeLength;
	A3DEUnits                     m_eExportUnit;
} A3DRWParamsExportStlData;

typedef struct
{
	unsigned short              m_usStructSize;
	unsigned char               m_bKeepCurrentTessellation;
	A3DRWParamsTessellationData m_sTessellation;
	char*                       m_pcDescription;
	char*                       m_pcCopyright;
	char*                       m_pcLicenseTerms;
} A3DRWParamsExport3mfData;

typedef struct
{
	unsigned short m_usStructSize;
	unsigned char  m_bExportMetadata;
	unsigned char  m_bExportTransformations;
	unsigned char  m_bExportColorMaterial;
	unsigned char  m_bExportProductInformationByFormat;
} A3DRWParamsExportXMLData;

typedef struct
{
	unsigned short m_usStructSize;
	char*          m_pcTextureFolder;
} A3DRWParamsExportObjData;

typedef struct
{
	unsigned short m_usStructSize;
	unsigned char  m_bAscii;
	char*          m_pcTextureFolder;
} A3DRWParamsExportFbxData;

typedef struct
{
	unsigned short m_usStructSize;
	unsigned char  m_bAscii;
	char*          m_pcTextureFolder;
} A3DRWParamsExportGltfData;
