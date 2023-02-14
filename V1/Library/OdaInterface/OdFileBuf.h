#pragma once

#include "TD_PackPush.h"
#include "OdaCommon.h"

#include "OdStreamBuf.h"
#include "RxObjectImpl.h"
#include "OdString.h"
#include "RxSystemServices.h"

//--------------------------------------------------------------------------------------------------

class OdBaseFileBuf;

/*
	This template class is a specialization of the OdSmartPtr class for OdBaseFileBuf object pointers.
*/
typedef OdSmartPtr<OdBaseFileBuf> OdBaseFileBufPtr;

/*
	This class is the base file I/O class.
	Source code provided.
	<group ExServices_Classes>
*/
class OdBaseFileBuf
	: public OdRxObjectImpl<OdStreamBuf>
{
private:

	bool m_bError;

protected:

	void enableErrorMode(bool bEnable = true)
	{
		m_bError = bEnable;
	}

	bool errorModeEnabled()
	{
		return m_bError;
	}

	void throwOdError(const OdError& pError)
	{
		enableErrorMode(true);
		throw pError;
	}

protected:

	HANDLE m_hFile;
	OdString m_sFileName;
	int m_iFileShare;
	bool m_bFileWritten;
	Oda::FileAccessMode m_accessMode;

	OdBaseFileBuf()
	{
		m_hFile = INVALID_HANDLE_VALUE;
		m_iFileShare = 0;
		m_bFileWritten = false;
		m_bError = false;
		m_accessMode = (Oda::FileAccessMode)0;
	}

public:

	/*
		Opens the specified file with this StreamBuf object.
		
		filename [in] Filename.
		shareMode [in] Share mode.
		accessMode [in] Access mode.
		creationDisposition [in] Creation disposition.
	*/
	virtual void open(const OdString& filename, Oda::FileShareMode shareMode, Oda::FileAccessMode accessMode, Oda::FileCreationDisposition creationDisposition);
#ifndef TD_UNICODE
	virtual void open(const OdCharW* filename, Oda::FileShareMode shareMode, Oda::FileAccessMode accessMode, Oda::FileCreationDisposition creationDisposition);
#endif

	/*
	*/
	virtual void close();
	/*
		Returns the name of the file associated with this StreamBuf object.
	*/
	virtual OdString fileName()
	{
		return m_sFileName;
	}
	/*
		Returns the share mode of this StreamBuf object.
	*/
	virtual OdUInt32 getShareMode()
	{
		return OdUInt32(m_iFileShare);
	}
	/*
		Returns the length in bytes of the file associated with this StreamBuf object.
	*/
	virtual OdUInt64 length();
	/*
		Moves the current position of the file pointer for this StreamBuf object to the specified location.

		offset [in] Number of bytes.
		seekType [in] Seek type.

		seekType must be one of the following:

		<table>
		Name                    Value       Description.
		OdDb::kSeekFromStart    SEEK_SET    Start of file.
		OdDb::kSeekFromCurrent  SEEK_CUR    Current position of file pointer.
		OdDb::kSeekFromEnd      SEEK_END    End of file.
		</table>
	*/
	virtual OdUInt64 seek(OdInt64 offset, OdDb::FilerSeekType seekType);
	/*
		Returns the current position of the file pointer for this StreamBuf object.
	*/
	virtual OdUInt64 tell();
	/*
		Returns true if and only if the current position of the file pointer for this StreamBuf object is at the end of file.
	*/
	virtual bool isEof();
	/*
		Returns the byte at the current position of the file pointer for this StreamBuf object, and increments the pointer.
	*/
	virtual OdUInt8 getByte();
	/*
		Returns the specified number of bytes, starting at the current position of the file pointer
		for this StreamBuf object, and increments the pointer by numBytes.

		buffer [in] Character buffer.
		numBytes [in] Number of bytes.
	*/
	virtual void getBytes(void* buffer, OdUInt32 numBytes);
	/*
		Writes the byte to the current position of the file pointer for this StreamBuf object, and increments the pointer.

		value [in] Byte to write.
	*/
	virtual void putByte(OdUInt8 value);
	/*
		Writes the specified number of bytes, starting at the current position of the file pointer
		for this StreamBuf object, and increments the pointer by numBytes.

		buffer [in] Character buffer.
		numBytes [in] Number of bytes.
	*/
	virtual void putBytes(const void* buffer, OdUInt32 numBytes);
	/*
		Sets the physical file size for the specified file associated with this
		StreamBuf object to the current position of the file pointer.
	*/
	virtual void truncate();
	/*
		Copies the specified bytes from this StreamBuf object to the specified StreamBuf object.
	
		pDestination [in] Pointer to the StreamBuf object to receive the data.
		sourceStart [in] Starting position of the file pointer of this StreamBuf object.
		sourceEnd [in] Ending position of the file pointer of this StreamBuf object.

		Bytes are copied from sourceStart to sourceEnd inclusive.
	*/
	virtual void copyDataTo(OdStreamBuf* pDestination, OdUInt64 sourceStart, OdUInt64 sourceEnd);
};

//--------------------------------------------------------------------------------------------------

class OdRdFileBuf;

/*
	This template class is a specialization of the OdSmartPtr class for OdRdFileBuf object pointers.
*/
typedef OdSmartPtr<OdRdFileBuf> OdRdFileBufPtr;

/*
	This class implements file input.

	Source code provided.
	<group ExServices_Classes>
*/
class OdRdFileBuf
	: public OdBaseFileBuf
{
	OdRdFileBuf(const OdRdFileBuf& source);

	OdRdFileBuf& operator = (const OdRdFileBuf& source);

protected:

	HANDLE m_hFileMap;
	LPVOID m_pFileMap;
	ULARGE_INTEGER m_ulSize;
	ULARGE_INTEGER m_ulPos;

	bool memBufferUsed() const
	{
		return m_pFileMap != 0;
	}

public:

	OdRdFileBuf(const OdString& filename)
	{
		m_hFileMap = INVALID_HANDLE_VALUE;
		m_pFileMap = NULL;
		m_ulSize.QuadPart = 0;
		m_ulPos.QuadPart = 0;

		open(filename);
	}

	/*
		Opens filename if specified.

		filename [in] Filename.
		shareMode [in] Share mode.
		accessMode [in] Access mode.
		creationDisposition [in] Creation disposition.
	*/
	OdRdFileBuf(const OdString& filename, Oda::FileShareMode shareMode, Oda::FileAccessMode accessMode, Oda::FileCreationDisposition creationDisposition)
	{
		m_hFileMap = INVALID_HANDLE_VALUE;
		m_pFileMap = NULL;
		m_ulSize.QuadPart = 0;
		m_ulPos.QuadPart = 0;

		open(filename, shareMode, accessMode, creationDisposition);
	}

	OdRdFileBuf();

	~OdRdFileBuf()
	{
		close();
	}
	/*
		Opens filename if specified.

		filename [in] Filename.
		shareMode [in] Share mode.
		accessMode [in] Access mode.
		creationDisposition [in] Creation disposition.
	*/
	static OdRdFileBufPtr createObject()
	{
		return OdRdFileBufPtr(new OdRdFileBuf(), kOdRxObjAttach);
	}

	static OdRdFileBufPtr createObject(const OdString& filename, Oda::FileShareMode shareMode = Oda::kShareDenyWrite, Oda::FileAccessMode accessMode = Oda::kFileRead, Oda::FileCreationDisposition creationDisposition = Oda::kOpenExisting)
	{
		return OdRdFileBufPtr(new OdRdFileBuf(filename, shareMode, accessMode, creationDisposition), kOdRxObjAttach);
	}

	virtual void open(const OdString& filename, Oda::FileShareMode shareMode = Oda::kShareDenyWrite, Oda::FileAccessMode accessMode = Oda::kFileRead, Oda::FileCreationDisposition creationDisposition = Oda::kOpenExisting);
#ifndef TD_UNICODE
	virtual void open(const OdCharW* filename, Oda::FileShareMode shareMode = Oda::kShareDenyWrite, Oda::FileAccessMode accessMode = Oda::kFileRead, Oda::FileCreationDisposition creationDisposition = Oda::kOpenExisting);
#endif

	virtual void close();

	virtual OdUInt64 length();

	virtual OdUInt64 seek(OdInt64 offset, OdDb::FilerSeekType seekType);

	virtual OdUInt64 tell();

	virtual bool isEof();

	virtual OdUInt8 getByte();

	virtual void getBytes(void* buffer, OdUInt32 numBytes);

	virtual void putByte(OdUInt8 value);

	virtual void putBytes(const void* buffer, OdUInt32 numBytes);

	virtual void truncate();

	virtual void copyDataTo(OdStreamBuf* pDestination, OdUInt64 sourceStart, OdUInt64 sourceEnd);
};

//--------------------------------------------------------------------------------------------------

#define WRITING_BUFFER_LENGTH 1024*8

class OdWrFileBuf;

/*
	This template class is a specialization of the OdSmartPtr class for OdWrFileBuf object pointers.
*/
typedef OdSmartPtr<OdWrFileBuf> OdWrFileBufPtr;

/*
	This class implements file output.
	Source code provided.
	<group ExServices_Classes>
*/
class OdWrFileBuf : public OdBaseFileBuf
{
	OdWrFileBuf(const OdWrFileBuf& source);

	OdWrFileBuf& operator = (const OdWrFileBuf& source);

	OdUInt8 m_pBuffer[WRITING_BUFFER_LENGTH + 1]; // +1 is to suppress /analyse false positive

	OdUInt32 m_nBufferedSize;

	void flush()
	{
		if (m_nBufferedSize == 0) {
			return;
		}
		OdBaseFileBuf::putBytes(m_pBuffer, m_nBufferedSize);
		m_nBufferedSize = 0;
	}

public:

	/*
		Opens filename if specified.

		filename [in] Filename.
		shareMode [in] Share mode.
		accessMode [in] Access mode.
		creationDisposition [in] Creation disposition.
	*/
	OdWrFileBuf(const OdString& filename)
		: m_nBufferedSize(0)
	{
		open(filename);
	}

	OdWrFileBuf(const OdString& filename, Oda::FileShareMode shareMode, Oda::FileAccessMode accessMode, Oda::FileCreationDisposition creationDisposition)
		: m_nBufferedSize(0)
	{
		open(filename, shareMode, accessMode, creationDisposition);
	}

	OdWrFileBuf();

	~OdWrFileBuf();
	/*
		Opens filename if specified.

		filename [in] Filename.
		shareMode [in] Share mode.
		accessMode [in] Access mode.
		creationDisposition [in] Creation disposition.
	*/
	static OdWrFileBufPtr createObject()
	{
		return OdWrFileBufPtr(new OdWrFileBuf(), kOdRxObjAttach);
	}

	static OdWrFileBufPtr createObject(const OdString& filename, Oda::FileShareMode shareMode = Oda::kShareDenyNo, Oda::FileAccessMode accessMode = Oda::kFileWrite, Oda::FileCreationDisposition creationDisposition = Oda::kCreateAlways)
	{
		return OdWrFileBufPtr(new OdWrFileBuf(filename, shareMode, accessMode, creationDisposition), kOdRxObjAttach);
	}

	virtual void close();

	virtual OdUInt64 seek(OdInt64 offset, OdDb::FilerSeekType seekType);

	virtual void copyDataTo(OdStreamBuf* pDestination, OdUInt64 sourceStart, OdUInt64 sourceEnd);

	virtual OdUInt64 tell();

	virtual OdUInt64 length();

	virtual void putByte(OdUInt8 value);

	virtual void putBytes(const void* buffer, OdUInt32 numBytes);

	virtual OdUInt8 getByte();

	virtual void getBytes(void* buffer, OdUInt32 numBytes);

	virtual void open(const OdString& filename, Oda::FileShareMode shareMode = Oda::kShareDenyNo, Oda::FileAccessMode accessMode = Oda::kFileWrite, Oda::FileCreationDisposition creationDisposition = Oda::kCreateAlways);
#ifndef TD_UNICODE
	virtual void open(const OdCharW* filename, Oda::FileShareMode shareMode = Oda::kShareDenyNo, Oda::FileAccessMode accessMode = Oda::kFileWrite, Oda::FileCreationDisposition creationDisposition = Oda::kCreateAlways);
#endif
};

#include "TD_PackPop.h"
