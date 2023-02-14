#pragma once

#include "TD_PackPush.h"
#include "RxSystemServices.h"

//--------------------------------------------------------------------------------------------------
/*
	This class implements platform-dependent file operations for Kernel API.
	<group ExServices_Classes>
	Library: Source code provided.
*/

class RxSystemServicesImpl : public OdRxSystemServices
{
public:

	RxSystemServicesImpl();

	OdStreamBufPtr createFile(const OdString& filename, Oda::FileAccessMode accessMode = Oda::kFileRead, Oda::FileShareMode shareMode = Oda::kShareDenyNo, Oda::FileCreationDisposition creationDisposition = Oda::kOpenExisting);

	bool accessFile(const OdString& filename, int accessMode);

	OdInt64 getFileCTime(const OdString& filename);

	OdInt64 getFileMTime(const OdString& filename);

	OdInt64 getFileSize(const OdString& filename);

	OdString formatMessage(unsigned int formatId, va_list* argList = 0);
	/*
		Returns the system code page.

		On Windows platforms system code page is initialized based on computer's Regional Settings.
		On other platforms with CP_UNDEFINED.
		It can be altered by setSystemCodePage()
	*/
	OdCodePageId systemCodePage() const;
	/*
		Sets the system code page.
	*/
	void setSystemCodePage(OdCodePageId id);
	/*
		Get environment variable.
		If varName doesn't exist, returns eKeyNotFound.
	*/
	virtual OdResult getEnvVar(const OdString& varName, OdString& value);
	/*
		Set environment variable with new value.
	*/
	virtual OdResult setEnvVar(const OdString& varName, const OdString& newValue);

protected:

	OdCodePageId m_CodePageId;
};

#include "TD_PackPop.h"
