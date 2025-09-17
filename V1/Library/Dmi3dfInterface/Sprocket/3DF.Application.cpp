#include "stdafx.h"

#include "3DF.Application.h"

#include <Common_Define.h>
#include <LogManager.h>

#include "../Signal/Signal.h"

// #include "3DF.Signal.Manager.h"
// #include "3DF.Signal.Interface.h"

#include "../3DF/3DF.Utility.h"

#include "../3DF/Database.h"
#include "../3DF/Portfolio.h"

// ----- HOOPS Header -----
#include <hoops_license.h>

#include <HDB.h>
#include <HErrorManager.h>
#include <hpserror.h>

// KEN
#include "../3DF/Facility.AppOptions.h"

#ifdef _DEBUG
#	define WRITE_3DF_ERROR_LOG
#endif

// #define WRITE_3DF_ERROR_LOG

using namespace H3DF;

namespace H3DF
{
	class ApplicationImpl : public Impl
	{
	public:
		std::unique_ptr<Impl> Clone() const override {
			auto pcClone = std::make_unique<ApplicationImpl>();
			pcClone->Copy(this);
			return pcClone;
		}
		
		void Copy(const ApplicationImpl * pcInThat) {
			m_pcHoopsDB = pcInThat->m_pcHoopsDB;
			m_dInDpiScale = pcInThat->m_dInDpiScale;
		}

		HDB * m_pcHoopsDB = nullptr;
		static void ErrorCallback(HErrorNode * pcNode, void * pcUserData);
		static CStringA ErrorCategoryString(int nId);
		static CStringA ErrorSpecificString(int nId);

		double m_dInDpiScale = 1.0;
	};
}

void H3DF::ApplicationImpl::ErrorCallback(HErrorNode * pcNode, void * pcUserData)
{
	CStringA strMessage, strText;
	CString strTraceMessage;

	switch (pcNode->m_severity) {
		case 0:
			strMessage = "[Info] ";
			break;

		case 1:
			strMessage = "[Warning] ";
			break;

		case 2:
			strMessage = L"[Error] ";
			break;

		case 3:
			strMessage = L"[Fatal Error] ";
			break;
	}

	strText.Format("Category: %s, ", ErrorCategoryString(pcNode->m_category));
	strMessage += strText;

	strText.Format("Specific: %s", ErrorSpecificString(pcNode->m_specific));
	strMessage += strText;

	LogManager::Log(LOGMANAGER_3DF_ERROR_LOG_ID, strMessage);

	LogManager::IncreaseTabIndex(LOGMANAGER_3DF_ERROR_LOG_ID);
	for (int nIndex = 0; nIndex < pcNode->m_messagelength; nIndex++) {
		strMessage.Format("ErrMsg: %s", pcNode->m_message[nIndex]);
		LogManager::Log(LOGMANAGER_3DF_ERROR_LOG_ID, strMessage);
		strTraceMessage.Format(L"%s\n", CString(strMessage));
		TRACE(strTraceMessage);
	}

	for (int nIndex = 0; nIndex < pcNode->m_stacklength; nIndex++) {
		strMessage.Format("Stack: %s", pcNode->m_stack[nIndex]);
		LogManager::Log(LOGMANAGER_3DF_ERROR_LOG_ID, strMessage);
		strTraceMessage.Format(L"%s\n", CString(strMessage));
		TRACE(strTraceMessage);
	}

	LogManager::DecreaseTabIndex(LOGMANAGER_3DF_ERROR_LOG_ID);
}

CStringA H3DF::ApplicationImpl::ErrorCategoryString(int nId)
{
	CStringA strMessage;
	switch (nId)
	{
		case HEC_UNDEFINED:
			strMessage = "Undefined";
			break;

		case HEC_DEBUG:
			strMessage = "Debug";
			break;

		case HEC_INTERNAL_ERROR:
			strMessage = "Internal Error";
			break;

		case HEC_MEMORY:
			strMessage = "Memory";
			break;

		case HEC_CALLBACK:
			strMessage = "Callback";
			break;

		case HEC_CAMERA:
			strMessage = "Camera";
			break;

		case HEC_COLOR:
			strMessage = "Color";
			break;

		case HEC_COLOR_MAP:
			strMessage = "Color Map";
			break;

		case HEC_DRIVER:
			strMessage = "Driver";
			break;

		case HEC_HANDEDNESS: // 9
			strMessage = "Handedness";
			break;

		case HEC_HEURISTIC: // 10
			strMessage = "Heuristic";
			break;

		case HEC_HOOPS_SYSTEM: // 11
			strMessage = "Hoops System";
			break;

		case HEC_METAFILE: // 12
			strMessage = "Metafile";
			break;

		case HEC_MODELLING_MATRIX:  // 13
			strMessage = "Modelling Matrix";
			break;

		case HEC_RENDERING: // 15
			strMessage = "Rendering";
			break;

		case HEC_IMAGE: // 30,
			strMessage = "Image";
			break;

		case HEC_LOCAL_LIGHT: // 31
			strMessage = "Local Light";
			break;

		case HEC_NORMAL: // 32
			strMessage = "Normal";
			break;

		case HEC_POLYGON: // 33
			strMessage = "Polygon";
			break;

		case HEC_POLYLINE: // 34
			strMessage = "Polyline";
			break;

		case HEC_TRISTRIP: // 35
			strMessage = "Tristrip";
			break;

		case HEC_VERTEX: // 36
			strMessage = "Vertex";
			break;

		case HEC_INK: // 37
			strMessage = "Ink";
			break;

		case HEC_MARKER: // 38
			strMessage = "Marker";
			break;

		case HEC_RENUMBER_KEY: // 39
			strMessage = "Renumber Key";
			break;

		case HEC_SPOT_LIGHT: // 40
			strMessage = "Spot Light";
			break;

		case HEC_COMPUTE: // 41
			strMessage = "Compute";
			break;

		case HEC_FILE_IO: // 42
			strMessage = "File IO";
			break;

		case HEC_INVALID_KEY: // 47
			strMessage = "Invalid key";
			break;

		case HEC_SEGMENT: // 48
			strMessage = "Segment";
			break;

		case HEC_SELECTION: // 49
			strMessage = "Selection";
			break;

		case HEC_SYNTAX:
			strMessage = "Syntax";
			break;

		case HEC_INCLUDE:
			strMessage = "Include";
			break;

		case HEC_GEOMETRY_OR_SEGMENT: // 58
			strMessage = "Geometry Or Segment";
			break;

		case HEC_LINE_STYLE: // 119
			strMessage = "Line Style";
			break;

		default:
			strMessage.Format("%d", nId);
			break;
	}
	
	return strMessage;
}

CStringA H3DF::ApplicationImpl::ErrorSpecificString(int nId)
{
	CStringA strMessage;
	switch (nId)
	{
		case HES_UNDEFINED:
			strMessage = "Undefined";
			break;

		case HES_DEBUG:
			strMessage = "Debug";
			break;

		case HES_PROCEDURAL_ERROR:
			strMessage = "Procedural Error";
			break;

		case HES_DATA_ERROR:
			strMessage = "Data Error";
			break;
		
		case HES_OUT_OF_MEMORY:
			strMessage = "Out of Memory";
			break;

		case HES_BUFFER_OVERFLOW:
			strMessage = "Buffer Overflow";
			break;

		case HES_ALLOCATE_MEMORY_FAILED:
			strMessage = "Allocate Memory Failed";
			break;

		case HES_MEMORY_USAGE:
			strMessage = "Memory Usage";
			break;

		case HES_SYSTEM_REJECTED_FREED_MEMORY:
			strMessage = "System Rejected Freed Memory";
			break;

		case HES_MEMORY_PROFILE_NOT_COMPILED:
			strMessage = "Memory Profile Not Compiled";
			break;

		case HES_ABORT_FUNCTION:
			strMessage = "Abort Function";
			break;

		case HES_ABORT_REQUESTED_BY_APPLICATION:
			strMessage = "Abort Requested By Application";
			break;

		case HES_INVALID_KEY: // 202
			strMessage = "Invalid key";
			break;

		case HES_INVALID_PROJECTION: // 213
			strMessage = "Invalid Projection";
			break;

		case HES_INVALID_RANGE: // 214
			strMessage = "Invalid Range";
			break;

		case HES_INVALID_REFERENCE: // 215
			strMessage = "Invalid Reference";
			break;
		
		case HES_NON_UNIFORM_HANDEDNESS: // 256
			strMessage = "Non Uniform Handedness";
			break;

		case HES_NORMAL: // 257
			strMessage = "Normal";
			break;

		case HES_NOT_AVAILABLE: // 258
			strMessage = "Not Available";
			break;

		case HES_NOT_A_CAMERA_VOLUME:
			strMessage = "Not A Camera Volume";
			break;

		case HES_NO_COLOR_TYPE_FOR_DISPLAY: // 265
			strMessage = "No Color Type For Display";
			break;

		case HES_NO_DIFFUSE_COLOR_COMPONENT: // 266
			strMessage = "No Diffuse Color Component";
			break;

		case HES_NO_LOCAL_SETTING: // 267
			strMessage = "No Local Setting";
			break;

		case HES_NO_MAPPED_VISAUL_TYPE: // 268
			strMessage = "No Mapped Visual Type";
			break;

		case HES_NO_OPEN_SEGMENT: // 269
			strMessage = "No Open Segment";
			break;

		case HES_NO_X_WINDOW_FOR_GL_WINDOW: // 270
			strMessage = "No X Window For GL Window";
			break;

		case HES_NULL_COLOR_NAME: // 271
			strMessage = "Null Color Name";
			break;

		case HES_NULL_INPUT: // 272
			strMessage = "Null Input";
			break;

		case HES_OPEN_SEGMENT: // 282
			strMessage = "Open Segment";
			break;

		case HES_PARSE_STRING: // 309
			strMessage = "Parse string";
			break;

		case HES_PATTERN: // 310
			strMessage = "Pattern";
			break;

		case HES_PLAIN_TEXTURE_ONLY: // 311
			strMessage = "Plain Texture Only";
			break;

		case HES_POLYGON: // 312
			strMessage = "Polygon";
			break;

		case HES_POLYLINE: // 313
			strMessage = "Polyline";
			break;

		case HES_PRINT_VERSION_MESSAGE: // 314
			strMessage = "Print Version Message";
			break;

		case HES_PURE_COLOR_OR_TEXTURE_ONLY: // 315
			strMessage = "Pure Color Or Texture Only";
			break;

		case HES_REBUILDING_TRISTRIP: // 316
			strMessage = "Rebuilding Tristrip";
			break;

		case HES_RECEIVED_EXIT_REQUEST: // 317
			strMessage = "Received Exit Request";
			break;

		case HES_REDEFINING_NAME: // 318
			strMessage = "Redefining Name";
			break;

		case HES_RENDITION_FREED: // 319
			strMessage = "Rendition Freed";
			break;

		case HES_RENDITION_NOT_FREED: // 320
			strMessage = "Rendition Not Freed";
			break;

		case HES_REPLACEMENT_OVERFLOW: // 321
			strMessage = "Replacement Overflow";
			break;

		case HES_REQUIRES_COLOR_MAP: // 322
			strMessage = "Requires Color Map";
			break;

		case HES_RGB_TYPE_IMAGE_REQUIRED: // 323
			strMessage = "RGB Type Image Required";
			break;

		case HES_SEGMENT: // 324
			strMessage = "Segment";
			break;

		case HES_SEGMENT_EXISTS: // 325
			strMessage = "Segment Exists";
			break;

		case HES_SELECTION_EVENT_NOT_ENABLED: // 326
			strMessage = "Selection Event Not Enabled";
			break;

		case HES_SELF_INTERSECTING_FACE: // 327
			strMessage = "Self Intersecting Face";
			break;

		case HES_SET_SB_X_SHARED_CMAP_ENV_VAR: // 328
			strMessage = "Set SB X Shared Cmap Env Var";
			break;

		case HES_SINGULAR_MATRIX: // 329
			strMessage = "Singular Matrix";
			break;

		case HES_SINGULAR_PLANE: // 330
			strMessage = "Singular Plane";
			break;

		case HES_SIZE: // 331
			strMessage = "Size";
			break;

		case HES_ZERO_LENGTH: // 386
			strMessage = "Zero Length";
			break;

		case HES_Z_BUFFERED_IMAGE: // 387
			strMessage = "Z Buffered Image";
			break;

		default:
			strMessage.Format("%d", nId);
			break;
	}


	return strMessage;
}

//== CWinApp에서 전달되는 메시지 처리 ==================================================================

// 1. Application이 실행될때 최초 처리 CWinApp::InitInstance에서 메시지 전달 받음.
void H3DF::Application::InitInstance()
{
	m_pcImpl = std::make_unique<ApplicationImpl>();
	DEBUG_VALID(m_pcImpl);

	auto pcImpl = static_cast<ApplicationImpl *>(m_pcImpl.get());
	DEBUG_VALID(pcImpl);

	//----- Construct에서 처리 하는 부분 -----
	// HOOPS License 처리
	HC_Define_System_Options("license = `" HOOPS_LICENSE "`");

	// Error 및 Warning 관련 설정
#if defined( WRITE_3DF_ERROR_LOG )
	HDB::EnableErrorManager();
	//HC_Define_System_Options("fatal errors, errors, warnings, info, no message limit");
	HErrorManager::AllowAllErrors();
	HErrorManager::SetErrorCallback(H3DF::ApplicationImpl::ErrorCallback, this);
	LogManager::SetWriteLog(LOGMANAGER_3DF_ERROR_LOG_ID, true);
	LogManager::SetCreateFile(LOGMANAGER_3DF_ERROR_LOG_ID, true);
	LogManager::SetFilePathName(LOGMANAGER_3DF_ERROR_LOG_ID, LogManager::GetExecuteDirectory() + L"Log\\3DF_Error.txt");
	LogManager::Log(LOGMANAGER_3DF_ERROR_LOG_ID, L"Log Create");
#else
	HC_Define_System_Options("errors, info, no message limit");
	//HC_Define_System_Options("no warnings, no info, no errors, no fatal errors, no message limit");
#endif

	//----- InitInstance에서 처리하는 부분 -----
	HANDLE heaps[1025];
	DWORD nheaps = GetProcessHeaps(1024, heaps);

	for(DWORD i = 0; i < nheaps; i++) {
		ULONG  HeapFragValue = 2;
		HeapSetInformation(heaps[i], HeapCompatibilityInformation, &HeapFragValue, sizeof(HeapFragValue));
	}

	// HOOPS DB 초기화
	pcImpl->m_pcHoopsDB = new HDB;
	pcImpl->m_pcHoopsDB->Init();

	// set the font directory
	TCHAR fontDirectory[MAX_PATH + 32];
	::GetWindowsDirectory(fontDirectory, MAX_PATH);
//	
	_tcscat(fontDirectory, _T("\\Fonts"));

	CString strFontDirectory = Utility::GetExecuteDirectory() + L"Fonts";

	CStringA strBuffer;
	strBuffer.Format("font directory = (%s, .)", Utility::ToChar(strFontDirectory));
	strBuffer.Format("%s, multi-threading=%s", strBuffer, "full");

/*
	char buf[4096];
 	sprintf(buf, "font directory = (%s, .)", Utility::ToChar(strFontDirectory));
	sprintf(buf, "%s, multi-threading=%s", buf, "full");
*/

	HC_Define_System_Options(strBuffer);
}

// 2. CWinApp::OnExitInstance() 처리
void H3DF::Application::ExitInstance()
{
	auto pcImpl = static_cast<ApplicationImpl *>(m_pcImpl.get());
	DEBUG_VALID(pcImpl);

	if (nullptr != pcImpl->m_pcHoopsDB) {
		delete pcImpl->m_pcHoopsDB;
		pcImpl->m_pcHoopsDB = nullptr;
	}

	HErrorManager::Cleanup();
}