#pragma once

#include "TD_PackPush.h"

#include "DbUserIO.h"
#include "Ed/EdFunctionIO.h"
#include "DbSSet.h"
#include "DbCommandContext.h"
#include "DbUnitsFormatterImpl.h"
#include "StaticRxObject.h"
#include "ExEdBaseIO.h"

//--------------------------------------------------------------------------------------------------
/*
	This class implements the interface for I/O and database access for custom commands during their execution.

	<group ExServices_Classes>
	Library: Source code provided.
*/

class ExDbCommandContext
	: public OdDbCommandContext
	, protected OdDbUserIO
	, protected OdEdFunctionIO
{
protected:

	ODRX_USING_HEAP_OPERATORS(OdDbCommandContext);

	OdSmartPtr<OdEdBaseIO> m_pIoStream;
	OdRxObject* m_pDb;
	OdGePoint3d m_LASTPOINT;
	mutable OdRxDictionaryPtr m_pArbDataDic;
	ExDbCommandContext* m_pOwner;

	/*
		Returns a SmartPointer to the arbitrary data dictionary for this CommandContext object.
		A dictionary is created if a dictionary does not exist.
	*/
	OdRxDictionaryPtr arbDataDic() const;
	/*
		Returns input of a string from the StreamBuf associated with this CommandContext object.

		options [in] Combination of OdEd::CommonInputOptions and OdEd::GetStringOptions enum values.
		prompt [in] Prompt output before pausing for user input.
		pTracker [in] Pointer to an IO Tracker for this user input.

		If prompt is empty, a default prompt is used.

		This function uses the underlying OdEdBaseIO function putString for low level output.
	*/
	OdString getStringInternal(const OdString& prompt, int options = OdEd::kInpDefault, OdEdStringTracker* pTracker = 0);

	struct RealInput
	{
		double val;
	};
	/*
		Returns input of a point or a real from the StreamBuf associated with this CommandContext object.

		options [in] Combination of OdEd::CommonInputOptions and OdEd::GetStringOptions enum values.
		prompt [in] Prompt output before pausing for user input.
		pPtTracker [in] Pointer to a Point Tracker for this user input.
		keywords [in] Keyword string.
		realType [in] Type of real.

		If prompt is empty, a default prompt is used.

		realType will be one of the following:
		OdResBuf::kDxfXdDist
		OdResBuf::kRtOrient

		This function uses the underlying OdEdBaseIO functions putString and getPoint for low level output.
	*/
	OdGePoint3d getPointOrReal(const OdString& prompt, int options, const OdString& keywords, OdResBuf::ValueType realType, OdEdPointTracker* pPtTracker);

	double getReal(const OdString& prompt, int options, double defValue, OdResBuf::ValueType realType, const OdString& keywords, OdEdRealTracker* pTracker);
	/*
		Returns the selection set specified by the user with a box.

		firstCorner [in] First corner of the box.
	*/
	virtual OdSelectionSetPtr box(const OdGePoint3d& firstCorner, OdDbVisualSelection::SubentSelectionMode ssm);
	/*
		Returns true if and only if specified string is an AutoLISP (handent) expression.
		In order to return true, input must be in the form (handent "<handleString>")
		If true is returned, the result object is set to the specified handle.

		input [in] String to try.
	*/
	virtual bool try_lisp(const OdString& input);

protected:

	ExDbCommandContext()
		: m_pDb(0)
	{}

public:

	/*
		Creates an instance of this CommandContext object.

		pStreamBuf [in] Pointer to the the stream buffer.
		pDb [in] Pointer to the database.

		Returns a SmartPointer to the newly created object.
	*/
	static OdDbCommandContextPtr createObject(OdEdBaseIO* pIOStream, OdDbDatabase* pDb);
	static OdDbCommandContextPtr createObject(OdEdBaseIO* pIOStream, OdRxObject* pRxDb = 0);

	OdRxObject* baseDatabase();

	void reset(OdEdBaseIO* pIOStream, OdRxObject* pRxDb);

	OdEdBaseIO* baseIO();

	virtual OdEdCommandContextPtr cloneObject(OdEdBaseIO* pIOStream = NULL, OdRxObject* pRxDb = NULL);

	OdEdUserIO* userIO() {
		return this;
	}

	OdEdFunctionIO* funcIO() {
		return this;
	}
	/*
		Sets the parameter object for this CommandContext object.

		pParamObj [in] Pointer to the parameter object.
	*/
	virtual void setParam(OdRxObject* pParamObj);
	/*
		Returns the parameter object for this CommandContext object.
	*/
	virtual OdRxObjectPtr param();
	/*
		Sets the result object for this CommandContext object.

		pResultObj [in] Pointer to the result object.
	*/
	virtual void setResult(OdRxObject* pResultObj);
	/*
		Returns the result object for this CommandContext object.
	*/
	virtual OdRxObjectPtr result();
	/*
		Writes arbitrary data to the specified file with this CommandContext object.

		fileName [in] Filename.
		pDataObj [in] Pointer to the data object.
	*/
	virtual void setArbitraryData(const OdString& fileName, OdRxObject* pDataObj);
	/*
		Reads arbitrary data from the specified file with this CommandContext object.

		fileName [in] Filename.

		Returns a SmartPointer to the data object.
	*/
	virtual OdRxObjectPtr arbitraryData(const OdString& fileName) const;
	/*
		Returns a selection set containing pickfirst data with this CommandContext object.
	*/
	virtual OdSelectionSetPtr pickfirst();
	/*
		Sets the pickfirst selection set for this CommandContext object.
		pSSet [in] Pointer to the pickfirst selection set.
	*/
	virtual void setPickfirst(OdSelectionSet* pSSet);
	/*
		Returns a selection set containing previous selection data with this CommandContext object.
	*/
	virtual OdSelectionSetPtr previousSelection();
	/*
		Sets the previous selection set for this CommandContext object.

		pSSet [in] Pointer to the previous selection set.
	*/
	virtual void setPreviousSelection(OdSelectionSet* pSSet);

public: // OdEdUserIO interface:

	virtual OdUnitsFormatter& baseFormatter();

	virtual OdDbUnitsFormatter& formatter();

	virtual void putString(const OdString& string);

	virtual int getInt(const OdString& prompt, int options = OdEd::kInpDefault, int defVal = 0, const OdString& keywords = OdString::kEmpty, OdEdIntegerTracker* pTracker = 0);

	virtual double getReal(const OdString& prompt, int options = OdEd::kInpDefault, double defVal = 0.0, const OdString& keywordList = OdString::kEmpty, OdEdRealTracker* pTracker = 0);

	virtual OdString getString(const OdString& prompt, int options = OdEd::kInpDefault, const OdString& pDefVal = OdString::kEmpty, const OdString& keywords = OdString::kEmpty, OdEdStringTracker* pTracker = 0);

	virtual int getKeyword(const OdString& prompt, const OdString& keywords, int defRes = -1, int options = OdEd::kInpDefault, OdEdIntegerTracker* pTracker = 0);

	virtual double getAngle(const OdString& prompt, int options = OdEd::kInpDefault, double defVal = 0.0, const OdString& keywords = OdString::kEmpty, OdEdRealTracker* pTracker = 0);
	/*
		Returns the LASTPOINT system variable for this CommandContext object.
	*/
	virtual OdGePoint3d getLASTPOINT() const;
	/*
		Sets the LASTPOINT system variable for this CommandContext object.

		val [in] New value for LASTPOINT.
	*/
	virtual void setLASTPOINT(const OdGePoint3d& val);
	/*
		Creates a rubberband line from the specified point to the user input.

		base [in] Base point.
	*/
	virtual OdEdPointDefTrackerPtr createRubberBand(const OdGePoint3d& base, OdGsModel* pModel = NULL) const;
	/*
		Creates a rectangular frame from the specified point to the user input.

		base [in] Base point.
	*/
	virtual OdEdPointDefTrackerPtr createRectFrame(const OdGePoint3d& base, OdGsModel* pModel = NULL) const;

	virtual OdGePoint3d getPoint(const OdString& prompt, int options = OdEd::kGptDefault, const OdGePoint3d* pDefVal = 0, const OdString& keywords = OdString::kEmpty, OdEdPointTracker* = 0);

	virtual double getDist(const OdString& prompt, int options = OdEd::kInpDefault, double defVal = 0.0, const OdString& keywords = OdString::kEmpty, OdEdRealTracker* pTracker = 0);

	virtual OdString getFilePath(const OdString& prompt, int options, const OdString& dialogCaption, const OdString& defExt, const OdString& fileName, const OdString& filter, const OdString& keywords = OdString::kEmpty, OdEdStringTracker* pTracker = 0);

	virtual OdSharedPtr<OdCmColorBase> getCmColor(const OdString& sPrompt, int options = OdEd::kInpDefault, const OdCmColorBase* pDefVal = 0, const OdString& sKeywords = OdString::kEmpty, OdEdColorTracker* pTracker = 0);

	virtual OdCmColor getColor(const OdString& prompt, int options = OdEd::kInpDefault, const OdCmColor* pDefVal = 0, const OdString& keywordList = OdString::kEmpty, OdEdColorTracker* pTracker = 0);

	virtual OdSelectionSetPtr select(const OdString& prompt = OdString::kEmpty, int options = OdEd::kSelDefault, const OdSelectionSet* pDefVal = 0, const OdString& keywords = OdString::kEmpty, OdSSetTracker* pTracker = 0, OdGePoint3dArray* ptsPointer = NULL);

protected:

	virtual bool merge(OdSelectionSet* pRes, int options, const OdSelectionSet* pSSet, OdSSetTracker* pTracker, OdEdUserIO* pIO = NULL);

	virtual bool isPickadd() const;

	virtual void highlight(OdSelectionSet* pSSet, bool bDoIt = true) const;

public:

	virtual void highlight(OdSelectionSetIterator* pIter, bool bDoIt = true) const;

	virtual void highlight(const OdDbBaseFullSubentPath& subEntPath, bool bDoIt = true) const;

protected:

	virtual bool interactive();
};

#include "TD_PackPop.h"
