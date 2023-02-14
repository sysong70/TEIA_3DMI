#include "stdafx.h"
#include "OdaCommon.h"

#if defined(ODA_WINDOWS)
#pragma warning(push)
#pragma warning(disable : 4018) // signed/unsigned mismatch
#pragma warning(disable : 4146) // unary minus operator applied to unsigned type, result still unsigned

#ifdef ODA_USE_FTIME
// Use standard ftime() function available for all systems except WinCE
#include <sys/timeb.h>
#endif

#ifdef OD_HAVE_SYS_TYPES_FILE
#include <sys/types.h>
#endif

#ifdef OD_HAVE_IO_FILE
#include <io.h>
#endif

#include <tchar.h>

#pragma warning(pop)
#endif

#ifdef OD_HAVE_SYS_STAT_FILE
#include <sys/stat.h>
#endif

#ifdef OD_HAVE_UNISTD_FILE
#include <unistd.h>
#endif

#include "RxSystemServicesImpl.h"
#include "OdFileBuf.h"
#include "RxObjectImpl.h"
#include "RxDefs.h"
#include "RxDictionary.h"
#include "RxModule.h"
#include "ExGiRasterImage.h"
#include "FlatMemStream.h"
#include "ColorMapping.h"
#include "Gs/GsBMPDevice.h"
#include "Gs/Gs.h"
#include "OdCharMapper.h"

#define STL_USING_ALGORITHM
#include "OdaSTL.h"
#include "DynamicLinker.h"
#include "Ed/EdCommandContext.h"
#include "Ed/EdUserIO.h"

//--------------------------------------------------------------------------------------------------

bool IsCompound(OdStreamBuf* pBuf);



RxSystemServicesImpl::RxSystemServicesImpl()
	: m_CodePageId(CP_UNDEFINED)
{
	switch (::GetACP()) {
	case 874:
		m_CodePageId = CP_ANSI_874; // Thai
		break;

	case 932:
		m_CodePageId = CP_ANSI_932; // Japanese
		break;

	case 936:
		m_CodePageId = CP_ANSI_936; // Chinese Simplified
		break;

	case 949:
		m_CodePageId = CP_ANSI_949; // Korean 
		break;

	case 950:
		m_CodePageId = CP_ANSI_950; // Chinese Traditional
		break;

	case 1200:
		m_CodePageId = CP_ANSI_1200; // CP_UNDEFINED
		break;

	case 1250:
		m_CodePageId = CP_ANSI_1250; // Eastern European
		break;

	case 1251:
		m_CodePageId = CP_ANSI_1251; // Cyrillic
		break;

	case 1252:
		m_CodePageId = CP_ANSI_1252; // US, Western Europe
		break;

	case 1253:
		m_CodePageId = CP_ANSI_1253; // Greek
		break;

	case 1254:
		m_CodePageId = CP_ANSI_1254; // Turkish
		break;

	case 1255:
		m_CodePageId = CP_ANSI_1255; // Hebrew
		break;

	case 1256:
		m_CodePageId = CP_ANSI_1256; // Arabic
		break;

	case 1257:
		m_CodePageId = CP_ANSI_1257; // Baltic - Estonian, Latvian and Lithuanian
		break;
	}
}


OdStreamBufPtr RxSystemServicesImpl::createFile(const OdString& path, Oda::FileAccessMode access, Oda::FileShareMode share, Oda::FileCreationDisposition dispos)
{
	OdSmartPtr<OdBaseFileBuf> pFile = OdRxSystemServices::createFile(path, access, share, dispos);
	if (pFile.isNull()) {
		if (!path.isEmpty() && path[0]) {
			if ((access & Oda::kFileWrite) != 0) {
				pFile = OdWrFileBuf::createObject();
			}
			else {
				pFile = OdRdFileBuf::createObject();
			}
			pFile->open(path, share, access, dispos);
		}
		else {
			throw OdError(eNoFileName);
		}
	}

	return OdStreamBufPtr(pFile);
}



template <class TChar>
bool isRxFSPath(const TChar* s)
{
	if (*s == '\0') {
		return false;
	}

	return (s[2] == ':' && s[0] == 'r' && s[1] == 'x');
}



struct OdString_Access : OdString
{
	static bool isConvertedToWide(const OdString& ws)
	{
		return !((OdString_Access&)ws).isUnicodeNotInSync();
	}
};



bool isRxFSPath(const OdString& path)
{
	if (OdString_Access::isConvertedToWide(path)) {
		return isRxFSPath<OdChar>(path);
	}

	return isRxFSPath<char>(path);
}



bool RxSystemServicesImpl::accessFile(const OdString& pcFilename, int mode)
{
	if (isRxFSPath(pcFilename)) {
		return OdRxSystemServices::accessFile(pcFilename, mode);
	}

	const char* fName = (const char*)pcFilename;
	bool res = (access(fName,
		(GETBIT(mode, Oda::kFileRead) ? 0x04 : 0x00) |
		(GETBIT(mode, Oda::kFileWrite) ? 0x02 : 0x00)) == 0);
	if (res) {
		struct stat st = { 0 };
		if (stat(fName, &st) == -1) {
			return false;
		}
		if ((st.st_mode & _S_IFDIR) != 0) {
			return false;
		}
	}

	return res;
}



OdInt64 RxSystemServicesImpl::getFileCTime(const OdString&)
{
	return (-1);
}



OdInt64 RxSystemServicesImpl::getFileMTime(const OdString&)
{
	return (-1);
}



OdInt64 RxSystemServicesImpl::getFileSize(const OdString& name)
{
	struct stat st;
	OdString tmp(name);
	if (stat(static_cast<const char*>(tmp), &st)) {
		return OdInt64(-1);
	}

	return OdInt64(st.st_size);
}



bool IsCompound(OdStreamBuf* pBuf)
{
	static OdUInt8 docID[8] = { 0xd0, 0xcf, 0x11, 0xe0, 0xa1, 0xb1, 0x1a, 0xe1 };
	OdUInt8  b8[8];
	int   i;

	pBuf->seek(128, OdDb::kSeekFromStart);
	pBuf->getBytes(b8, 8);
	pBuf->seek(0, OdDb::kSeekFromStart);

	for (i = 0; i < 8; i++) {
		if (b8[i] != docID[i]) {
			return false;
		}
	}

	return true;
}



OdString RxSystemServicesImpl::formatMessage(unsigned int code, va_list* argList)
{
	static const OdChar* message[] =
	{
  #define OD_ERROR_DEF(cod, desc)  desc,
  #include "ErrorDefs.h"
  #undef OD_ERROR_DEF

  #define OD_MESSAGE_DEF(cod, desc) desc,
  #include "MessageDefs.h"
  #undef OD_MESSAGE_DEF
		OD_T("") // DummyLastMassage
	};

	OdString msg;
	if (code >= sidDummyLastMessage) {
		msg.format(OD_T("Unknown message (code: %d)"), code);
		return msg;
	}

	if (argList) {
		msg.formatV(message[code], *argList);
	}
	else {
		msg = message[code];
	}

	return msg;
}



OdCodePageId RxSystemServicesImpl::systemCodePage() const
{
	return m_CodePageId;
}



void RxSystemServicesImpl::setSystemCodePage(OdCodePageId id)
{
	m_CodePageId = id;
}



OdResult RxSystemServicesImpl::getEnvVar(const OdString& varName, OdString& value)
{
	int lenStr = varName.getLength();
	OdAnsiCharArray dstBuf;
	dstBuf.reserve(lenStr * 4); // UTF-8 can use up to 4 bytes per character
	OdCharMapper::unicodeToUtf8(varName.c_str(), lenStr, dstBuf);

	char* envVal = getenv(dstBuf.asArrayPtr());
	if (envVal == NULL) {
		return eKeyNotFound;
	}

	OdAnsiString asCmd(envVal);
	OdCharArray buf;
	OdCharMapper::utf8ToUnicode(asCmd.c_str(), asCmd.getLength(), buf);
	value = OdString(buf.getPtr(), buf.size() - 1);

	return eOk;
}



OdResult RxSystemServicesImpl::setEnvVar(const OdString& varName, const OdString& newValue)
{
	OdString strVal = varName + OD_T("=") + newValue;
	int lenStr = strVal.getLength();

	OdAnsiCharArray dstBuf;
	// UTF-8 can use up to 4 bytes per character
	dstBuf.reserve(lenStr * 4);

	OdCharMapper::unicodeToUtf8(strVal.c_str(), lenStr, dstBuf);
	// according to description of putenv() this array shouldn't be deleted as it becomes env var.
	char* mStrVal = new char[lenStr * 4];
	memcpy(mStrVal, (char*)dstBuf.asArrayPtr(), lenStr * 4);
	if (putenv(mStrVal) != 0) {
		return eCantSetEnvVar;
	}

	return eOk;
}
