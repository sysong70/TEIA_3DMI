#pragma once

#include "TD_PackPush.h"
#include "DbUndoController.h"
#include "UInt8Array.h"
#include "OdList.h"

/*
	This class implements platform-independent UndoController objects.
	<group ExServices_Classes> Library: Source provided.
*/
class ExUndoController : public OdDbUndoController
{
	OdList<OdUInt8Array> m_records;
	OdUInt32 m_nMemoryUsed;
	OdUInt32 m_nMaxSteps;
	OdUInt32 m_nMaxMemory;

protected:

	ExUndoController();
	/*
		Frees the first record of this UndoController object.
	*/
	void freeFrontRecord();
	/*
		Frees the last record of this UndoController object.
	*/
	void freeBackRecord();
	/*
		Returns the memory size (in bytes) of the first record of this UndoController object.
	*/
	OdUInt32 frontRecordMemory() const;
	/*
		Returns the memory size (in bytes) of the last record of this UndoController object.
	*/
	OdUInt32 backRecordMemory() const;
	/*
		Returns the memory size (in bytes) required for a record with the specified data size.

		dataSize [in] Data size (in bytes).
	*/
	static OdUInt32 recordMemory(OdUInt32 dataSize);
	/*
		Frees extra memory from this UndoController object.

		Frees front records until the memory size and number of records of the UndoControler list is less that that specified by the limits.
	*/
	void freeExtra();
	/*
		Adds a record of the specifed size to the end of this UndoController object.

		Returns true if and only if successful.
	*/
	bool pushRecord(OdUInt32 nSizeOfRecToAppend);

public:

	/*
		Sets the limits of this UndoController object.

		maxSteps [in] Maximum number of records.
		maxMemory [in] Maximum memory in bytes.
	*/
	void setLimits(OdUInt32 maxSteps, OdUInt32 maxMemory);
	/*
		Adds the specified number of bytes from the specified StreamBuf object to the end of this UndoController object.
	
		pStreamBuf [in] Pointer to the StreamBuf object from which the data are to be read.
		numBytes [in] Number of bytes to be read.
	*/
	void pushData(OdStreamBuf* pStreamBuf, OdUInt32 numBytes, OdUInt32 opt);
	/*
		Returns true if and only if this UndoController object is not empty.
	*/
	bool hasData() const;
	/*
		Writes the record at the end of this UndoController object to the specified StreamBuf object.
		pStreamBuf [in] Pointer to the StreamBuf object to which the data are to be written.
	*/
	OdUInt32 popData(OdStreamBuf* pStreamBuf);

	OdRxIteratorPtr newRecordStackIterator() const;

	void clearData();
};

#include "TD_PackPop.h"
