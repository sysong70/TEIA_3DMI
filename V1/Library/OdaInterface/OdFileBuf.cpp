#include "stdafx.h"
#include "OdaCommon.h"
#include "OdFileBuf.h"

#define ODA_NON_TRACING   // Comment it to have trace

#pragma warning(disable: 4290)

//--------------------------------------------------------------------------------------------------

OdString winErrMessage(DWORD err) {
	OdString buf;
	FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, NULL, err,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPTSTR)buf.getBuffer(64 * 1024), 64 * 1024, NULL);
	buf.releaseBuffer();
	return buf;
}

//--------------------------------------------------------------------------------------------------

void OdBaseFileBuf::open(const OdString& wszFileName, Oda::FileShareMode nShareMode, Oda::FileAccessMode nDesiredAccess, Oda::FileCreationDisposition nCreationDisposition)
{
	close();
	ODA_TRACE(OD_T("open('%ls', 0x%x, 0x%x, %d)"), wszFileName.c_str(), nDesiredAccess, nShareMode, nCreationDisposition);

	// Check file name passed
	if (wszFileName.isEmpty()) {
		throw OdError(eNoFileName);
	}

	DWORD dwAccess = 0;
	DWORD dwAttr = 0;

	if (nDesiredAccess & Oda::kFileRead) {
		dwAccess |= GENERIC_READ;
		dwAttr |= FILE_FLAG_RANDOM_ACCESS;
	}
	if (nDesiredAccess & Oda::kFileWrite) {
		dwAccess |= GENERIC_WRITE;
		dwAttr |= FILE_ATTRIBUTE_NORMAL;
	}
	DWORD dwShare = 0;

	// special preparation for "temporary files" : enable optimal caching + "delete on close"
	if (nDesiredAccess & Oda::kFileTmp) {
		dwAttr |= FILE_ATTRIBUTE_TEMPORARY;
	}
	if (nDesiredAccess & Oda::kFileDelete) {
		dwAttr |= FILE_FLAG_DELETE_ON_CLOSE;
		dwShare = FILE_SHARE_DELETE;
	}
	if (nShareMode != Oda::kShareDenyRead && nShareMode != Oda::kShareDenyReadWrite) {
		dwShare |= FILE_SHARE_READ;
	}
	if (nShareMode != Oda::kShareDenyWrite && nShareMode != Oda::kShareDenyReadWrite) {
		dwShare |= FILE_SHARE_WRITE;
	}

	DWORD dwDisposition = 0;
	switch (nCreationDisposition) {
	case Oda::kCreateNew:
		dwDisposition = CREATE_NEW;
		break;

	case Oda::kCreateAlways:
		dwDisposition = CREATE_ALWAYS;
		break;

	case Oda::kOpenExisting:
		dwDisposition = OPEN_EXISTING;
		break;

	case Oda::kOpenAlways:
		dwDisposition = OPEN_ALWAYS;
		break;

	case Oda::kTruncateExisting:
		dwDisposition = TRUNCATE_EXISTING;
		break;
	}

	OdString sFileName = wszFileName;
#if defined(_WINRT)
	m_hFile = ::CreateFile2(wszFileName, dwAccess, dwShare, dwDisposition, /*dwAttr,*/ NULL);
#else
	m_hFile = ::CreateFileW(wszFileName, dwAccess, dwShare, NULL, dwDisposition, dwAttr, NULL);
#endif
	if (m_hFile == INVALID_HANDLE_VALUE) {
		DWORD winerr = ::GetLastError();
		// mapping sys error to corresponding OdResult codes and exception classes
		switch (winerr) {
		case ERROR_FILE_NOT_FOUND:
			throw OdError_FileNotFound(sFileName);

		case ERROR_ACCESS_DENIED:
			throw OdError_FileException(eFileAccessErr, sFileName, ::winErrMessage(winerr));

		case ERROR_SHARING_VIOLATION:
			throw OdError_FileException(eFileSharingViolation, sFileName, ::winErrMessage(winerr));

		default:
			throw OdError_FileException(eCantOpenFile, sFileName, ::winErrMessage(winerr));
		}
	}
	else {
		m_sFileName = sFileName;
		m_iFileShare = nShareMode;
		m_accessMode = nDesiredAccess;
	}
}



void OdBaseFileBuf::close()
{
	if (m_hFile != INVALID_HANDLE_VALUE) {
		ODA_TRACE(OD_T("close()"));
		if (!(m_accessMode & Oda::kNoFlushWhenClosed)) {
			// Flush buffer if writing was done
			if (m_bFileWritten) {
				::FlushFileBuffers(m_hFile);
			}
		}

		::CloseHandle(m_hFile);
		m_hFile = INVALID_HANDLE_VALUE;
	}

	m_sFileName.empty();
	m_iFileShare = 0;
	m_bFileWritten = false;
	m_bError = false;
	m_accessMode = (Oda::FileAccessMode)0;
}



OdUInt64 OdBaseFileBuf::length()
{
	ODA_TRACE(OD_T("length()"));
	ULARGE_INTEGER  uli;
	uli.QuadPart = 0;
	uli.LowPart = ::GetFileSize(m_hFile, &uli.HighPart);
	return uli.QuadPart;
}

#if !defined(INVALID_SET_FILE_POINTER) // for _MSC_VER <= 1200
#define INVALID_SET_FILE_POINTER ((DWORD)-1)
#endif

OdUInt64 OdBaseFileBuf::seek(OdInt64 offset, OdDb::FilerSeekType whence)
{
	ODA_TRACE(OD_T("seek(%d, %d)"), offset, whence);

	DWORD dwMethod = 0;
	switch (whence) {
	case OdDb::kSeekFromStart:
		dwMethod = FILE_BEGIN;
		break;

	case OdDb::kSeekFromCurrent:
		dwMethod = FILE_CURRENT;
		break;

	case OdDb::kSeekFromEnd:
		dwMethod = FILE_END;
		break;
	}

	LARGE_INTEGER li;
	li.QuadPart = offset;
	li.LowPart = ::SetFilePointer(m_hFile, li.LowPart, &li.HighPart, dwMethod);
	if (li.LowPart == INVALID_SET_FILE_POINTER && ::GetLastError() != NO_ERROR) {
		li.QuadPart = -1;
		throw OdError_FileException(eFileInternalErr, m_sFileName);
	}
	return li.QuadPart;
}



OdUInt64 OdBaseFileBuf::tell()
{
	ODA_TRACE(OD_T("tell()"));
	LARGE_INTEGER li;
	li.QuadPart = 0;
	li.LowPart = ::SetFilePointer(m_hFile, li.LowPart, &li.HighPart, FILE_CURRENT);

	return li.QuadPart;
}



bool OdBaseFileBuf::isEof()
{
	ODA_TRACE(OD_T("isEof()"));
	return (::GetFileSize(m_hFile, NULL) <= ::SetFilePointer(m_hFile, 0, NULL, FILE_CURRENT));
}



OdUInt8 OdBaseFileBuf::getByte()
{
	ODA_TRACE(OD_T("getByte()"));

	DWORD dwBytes = 0;
	OdUInt8 b = 0;
	if (::ReadFile(m_hFile, &b, 1, &dwBytes, NULL) == false) {
		ODA_TRACE(OD_T("getByte() can't read byte due to read error"));
		throw OdError_FileException(eFileInternalErr, m_sFileName);
	}
	if (dwBytes == 0) {
		ODA_TRACE(OD_T("getByte() can't read byte due to end-of-file"));
		throw OdError_FileException(eEndOfFile, m_sFileName);
	}

	return b;
}



void OdBaseFileBuf::getBytes(void* buffer, OdUInt32 nLen)
{
	ODA_TRACE(OD_T("getBytes(%p, %u)"), buffer, nLen);

	DWORD dwBytes = 0;
	if (::ReadFile(m_hFile, buffer, nLen, &dwBytes, NULL) == false) {
		ODA_TRACE(OD_T("getBytes() can't read byte due to read error"));
		throw OdError_FileException(eFileInternalErr, m_sFileName);
	}
	if (dwBytes < nLen) {
		ODA_TRACE(OD_T("getBytes() read only %u bytes due to end-of-file"), dwBytes);
		throw OdError_FileException(eEndOfFile, m_sFileName);
	}
}



void OdBaseFileBuf::putByte(OdUInt8 val)
{
	ODA_TRACE(OD_T("putByte(0x%x)"), val);

	DWORD dwBytes = 0;
	BYTE b = val;
	if (::WriteFile(m_hFile, &b, 1, &dwBytes, NULL) == false || dwBytes != 1) {
		ODA_TRACE(OD_T("putByte() can't write byte"));
		throwOdError(OdError_FileException(eFileWriteError, m_sFileName));
	}

	m_bFileWritten = true;
}



void OdBaseFileBuf::putBytes(const void* buffer, OdUInt32 nLen)
{
	ODA_TRACE(OD_T("putBytes(%p, %u)"), buffer, nLen);

	DWORD dwBytes = 0;
	if (::WriteFile(m_hFile, buffer, nLen, &dwBytes, NULL) == false || dwBytes != nLen) {
		ODA_TRACE(OD_T("putBytes() can't write bytes"));
		throwOdError(OdError_FileException(eFileWriteError, m_sFileName));
	}

	m_bFileWritten = true;
}



void OdBaseFileBuf::truncate()
{
	ODA_TRACE(OD_T("truncate()"));
	if (::SetEndOfFile(m_hFile) == false) {
		ODA_TRACE(OD_T("truncate() failed"));
		throw OdError_FileException(eFileInternalErr, m_sFileName);
	}
}

// Current method behavior description (so required behavior isn't documented yet):
//  - Move source stream to nSrcStart position
//  - Read (nSrcEnd-nSrcStart) bytes from source stream to temporary buffer
//  - Write buffer to target stream
//  - Set source stream to nSrcEnd position

void OdBaseFileBuf::copyDataTo(OdStreamBuf* pDest, OdUInt64 nSrcStart, OdUInt64 nSrcEnd)
{
	ODA_TRACE(OD_T("copyDataTo(%p, %u, %u)"), pDest, nSrcStart, nSrcEnd);

	if (nSrcStart == 0 && nSrcEnd == 0) {
		nSrcStart = tell();
		nSrcEnd = length();
	}

	// Do nothing if incorrect positions passed
	if (nSrcEnd <= nSrcStart) {
		return;
	}

	// Remember current position to restore on error
	OdUInt64 nPos = tell();
	if (nPos != nSrcStart) {
		// Move to start position
		if (seek(nSrcStart, OdDb::kSeekFromStart) != nSrcStart) {
			ODA_TRACE(OD_T("copyDataTo() can't move to start position"));
			throw OdError_FileException(eEndOfFile, m_sFileName);
		}
	}

	// Allocate temporary buffer
	OdUInt32 nLen = OdUInt32(nSrcEnd - nSrcStart);

	HLOCAL hBuf = ::LocalAlloc(LHND, nLen);
	if (hBuf == NULL) {
		ODA_TRACE(OD_T("copyDataTo() can't allocate temporary buffer"));
		seek(nPos, OdDb::kSeekFromStart);
		throw OdError_FileException(eOutOfMemory, m_sFileName);
	}

	void* pBuf = LocalLock(hBuf);
	if (pBuf == NULL) {
		ODA_TRACE(OD_T("copyDataTo() can't access to temporary buffer"));
		::LocalFree(hBuf);
		seek(nPos, OdDb::kSeekFromStart);
		throw OdError_FileException(eOutOfMemory, m_sFileName);
	}

	// Read to buffer (current position will be set to nSrcEnd after)
	getBytes(pBuf, nLen);

	// Write to destination
	pDest->putBytes(pBuf, nLen);

	LocalUnlock(hBuf);
	::LocalFree(hBuf);
}

//--------------------------------------------------------------------------------------------------

void OdRdFileBuf::open(const OdString& wszFileName, Oda::FileShareMode nShareMode, Oda::FileAccessMode nDesiredAccess, Oda::FileCreationDisposition nCreationDisposition)
{
	if (nDesiredAccess & Oda::kFileWrite) {
		throw OdError_FileException(eCantOpenFile, wszFileName);
	}

	OdBaseFileBuf::open(wszFileName, nShareMode, nDesiredAccess, nCreationDisposition);

	// Test open mode and file attributes to use fast mapping buffer instead of plain one
	if (nCreationDisposition == Oda::kOpenExisting || nCreationDisposition == Oda::kOpenAlways) {
		// Reject compressed and other special files
		// TODO Perhaps reject remote files also
		DWORD dwAttr = ::GetFileAttributesW(wszFileName);
		if (dwAttr != INVALID_FILE_SIZE &&
			!(dwAttr & (FILE_ATTRIBUTE_DIRECTORY | FILE_ATTRIBUTE_ENCRYPTED | FILE_ATTRIBUTE_SPARSE_FILE | FILE_ATTRIBUTE_REPARSE_POINT | FILE_ATTRIBUTE_COMPRESSED | FILE_ATTRIBUTE_OFFLINE))) {
			// Remember actual file size
			m_ulSize.LowPart = ::GetFileSize(m_hFile, &m_ulSize.HighPart);
			DWORD dwProtect = PAGE_READONLY, dwAccess = FILE_MAP_READ;

			// Map file into memory
			m_hFileMap = ::CreateFileMapping(m_hFile, NULL, dwProtect, 0, 0, NULL);
			if (m_hFileMap != NULL) {
				m_pFileMap = ::MapViewOfFile(m_hFileMap, dwAccess, 0, 0, 0);
			}
			else {
				// Fall down to plain buffer use
				// disable cache
			}
		}
	}
}



OdRdFileBuf::OdRdFileBuf()
{
	m_hFileMap = NULL;
	m_pFileMap = NULL;
	m_ulSize.QuadPart = 0;
	m_ulPos.QuadPart = 0;
}



void OdRdFileBuf::close()
{
	if (m_pFileMap) {
		::UnmapViewOfFile(m_pFileMap);
		m_pFileMap = NULL;
	}
	if (m_hFileMap != NULL) {
		::CloseHandle(m_hFileMap);
		m_hFileMap = NULL;
	}

	m_ulSize.QuadPart = 0;
	m_ulPos.QuadPart = 0;

	OdBaseFileBuf::close();
}



OdUInt64 OdRdFileBuf::length()
{
	return memBufferUsed() ? OdUInt64(m_ulSize.QuadPart) : OdBaseFileBuf::length();
}



OdUInt64 OdRdFileBuf::seek(OdInt64 offset, OdDb::FilerSeekType whence)
{
	if (memBufferUsed() == false) {
		return OdBaseFileBuf::seek(offset, whence);
	}

	switch (whence) {
	// it is ok to seek beyond the end of a file, read() will return 0 in this case
	case OdDb::kSeekFromStart:
		if (offset < 0) {
			throw OdError_FileException(eFileInternalErr, m_sFileName);
		}
		m_ulPos.QuadPart = offset;
		break;

	case OdDb::kSeekFromCurrent:
		if (offset < 0 && m_ulPos.QuadPart < (OdUInt64)(-offset)) {
			throw OdError_FileException(eFileInternalErr, m_sFileName);
		}
		m_ulPos.QuadPart += offset;
		break;

	case OdDb::kSeekFromEnd:
		if (offset < 0 && m_ulSize.QuadPart < (OdUInt64)(-offset)) {
			throw OdError_FileException(eFileInternalErr, m_sFileName);
		}
		m_ulPos.QuadPart = m_ulSize.QuadPart + offset;
		break;
	}

	return m_ulPos.QuadPart;
}



OdUInt64 OdRdFileBuf::tell()
{
	if (memBufferUsed() == false) {
		return OdBaseFileBuf::tell();
	}

	return OdUInt64(m_ulPos.QuadPart);
}



bool OdRdFileBuf::isEof()
{
	if (memBufferUsed() == false) {
		return OdBaseFileBuf::isEof();
	}

	return (m_ulPos.QuadPart >= m_ulSize.QuadPart);
}



OdUInt8 OdRdFileBuf::getByte()
{
	if (memBufferUsed() == false) {
		return OdBaseFileBuf::getByte();
	}
	if (m_ulPos.QuadPart >= m_ulSize.QuadPart) {
		throw OdError_FileException(eEndOfFile, m_sFileName);
	}

	return ((OdUInt8*)m_pFileMap)[m_ulPos.QuadPart++];
}



void OdRdFileBuf::getBytes(void* buffer, OdUInt32 nLen)
{
	if (memBufferUsed() == false) {
		OdBaseFileBuf::getBytes(buffer, nLen);
		return;
	}
	if ((m_ulPos.QuadPart + nLen) > m_ulSize.QuadPart) {
		throw OdError_FileException(eEndOfFile, m_sFileName);
	}

	::CopyMemory(buffer, ((OdUInt8*)m_pFileMap) + m_ulPos.QuadPart, nLen);

	m_ulPos.QuadPart += nLen;
}



void OdRdFileBuf::putByte(OdUInt8 val)
{
	if (memBufferUsed() == false) {
		OdBaseFileBuf::putByte(val);
		return;
	}

	if (m_ulPos.QuadPart > m_ulSize.QuadPart) {
		throw OdError_FileException(eFileWriteError, m_sFileName);
	}

	((OdUInt8*)m_pFileMap)[m_ulPos.QuadPart++] = val;

	// TODO? m_bFileWritten = true;
}



void OdRdFileBuf::putBytes(const void* buffer, OdUInt32 nLen)
{
	if (memBufferUsed() == false) {
		OdBaseFileBuf::putBytes(buffer, nLen);
		return;
	}

	if ((m_ulPos.QuadPart + nLen) > m_ulSize.QuadPart) {
		throw OdError_FileException(eFileWriteError, m_sFileName);
	}

	::CopyMemory(((OdUInt8*)m_pFileMap) + m_ulPos.QuadPart, buffer, nLen);

	m_ulPos.QuadPart += nLen;

	// TODO? m_bFileWritten = true;
}



void OdRdFileBuf::truncate()
{
	if (memBufferUsed() == false) {
		OdBaseFileBuf::truncate();
		return;
	}

	throw OdError_FileException(eFileWriteError, m_sFileName);
}

// Current method behavior description (so required behavior isn't documented yet):
//  - Move source stream to nSrcStart position
//  - Read (nSrcEnd-nSrcStart) bytes from source stream to temporary buffer
//  - Write buffer to target stream
//  - Set source stream to nSrcEnd position

void OdRdFileBuf::copyDataTo(OdStreamBuf* pDest, OdUInt64 nSrcStart, OdUInt64 nSrcEnd)
{
	if (memBufferUsed() == false) {
		OdBaseFileBuf::copyDataTo(pDest, nSrcStart, nSrcEnd);
		return;
	}
	if (pDest == nullptr) {
		throw OdError_FileException(eNullObjectPointer, m_sFileName);
	}

	if (nSrcStart == 0 && nSrcEnd == 0) {
		nSrcStart = tell();
		nSrcEnd = length();
	}

	// Do nothing if incorrect positions passed
	if (nSrcEnd <= nSrcStart) {
		return;
	}
	if (nSrcEnd > m_ulSize.QuadPart) {
		throw OdError_FileException(eEndOfFile, m_sFileName);
	}

	pDest->putBytes(((OdUInt8*)m_pFileMap) + nSrcStart, OdUInt32(nSrcEnd - nSrcStart));
}

//--------------------------------------------------------------------------------------------------

OdWrFileBuf::OdWrFileBuf() : m_nBufferedSize(0)
{
}



OdWrFileBuf::~OdWrFileBuf()
{
	close();
}



void OdWrFileBuf::open(const OdString& wszFileName, Oda::FileShareMode nShareMode, Oda::FileAccessMode nDesiredAccess, Oda::FileCreationDisposition nCreationDisposition)
{
	if (nDesiredAccess == Oda::kFileRead) {
		throw OdError_FileException(eCantOpenFile, wszFileName);
	}

	OdBaseFileBuf::open(wszFileName, nShareMode, nDesiredAccess, nCreationDisposition);
}



OdUInt64 OdWrFileBuf::seek(OdInt64 offset, OdDb::FilerSeekType whence)
{
	flush();

	return OdBaseFileBuf::seek(offset, whence);
}



void OdWrFileBuf::putByte(OdUInt8 val)
{
	if (m_nBufferedSize >= WRITING_BUFFER_LENGTH) {
		flush();
	}

	m_pBuffer[m_nBufferedSize] = val;
	++m_nBufferedSize;
}



void OdWrFileBuf::putBytes(const void* buffer, OdUInt32 nLen)
{
	if (nLen > WRITING_BUFFER_LENGTH) {
		flush();
		OdBaseFileBuf::putBytes(buffer, nLen);
		return;
	}
	if ((m_nBufferedSize + nLen) > WRITING_BUFFER_LENGTH) {
		flush();
	}

	memcpy(m_pBuffer + m_nBufferedSize, buffer, nLen);
	m_nBufferedSize += nLen;
}



OdUInt64 OdWrFileBuf::tell()
{
	return OdBaseFileBuf::tell() + m_nBufferedSize;
}



void OdWrFileBuf::close()
{
	// to prevent second exception in putBytes. cr 3861
	if (errorModeEnabled() == false) {
		flush();
	}

	OdBaseFileBuf::close();
}



OdUInt8 OdWrFileBuf::getByte()
{
	flush();

	return OdBaseFileBuf::getByte();
}



void OdWrFileBuf::getBytes(void* buffer, OdUInt32 nLen)
{
	flush();

	OdBaseFileBuf::getBytes(buffer, nLen);
}



void OdWrFileBuf::copyDataTo(OdStreamBuf* pDest, OdUInt64 nSrcStart, OdUInt64 nSrcEnd)
{
	flush();
	
	OdBaseFileBuf::copyDataTo(pDest, nSrcStart, nSrcEnd);
}



OdUInt64 OdWrFileBuf::length()
{
	OdUInt64 l = OdBaseFileBuf::length();
	OdUInt64 pos = OdBaseFileBuf::tell();
	if (pos + m_nBufferedSize > l) {
		return pos + m_nBufferedSize;
	}
	else {
		return l;
	}
}
