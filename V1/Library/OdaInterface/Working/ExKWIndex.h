#pragma once

#include "TD_PackPush.h"

#include "OdString.h"
#include "Ed/EdUserIO.h"
#include "OdArray.h"

/*
	This class implements keyword index data for ExDbCommandContext objects.
	<group ExServices_Classes>
*/
class KWIndexData
{
public:

	OdString m_sKey;   // Key1
	OdString m_sKey2;  // Key2
	OdString m_sKword; // Keyword
	int m_nPKey;       // part Key1 start
	int m_nPKey2;      // part Key2 start

	KWIndexData()
	{}
	/*
		n [in] maximum number of characters to return.

		return the leftmost n characters of Key1.
	*/
	OdString key1(int n) const
	{
		return m_sKey.left(odmin(n, m_sKey.getLength()));
	}
	/*
		n [in] maximum number of characters to return.

		return the leftmost n characters of partial Key1.
	*/
	OdString pKey1(int n) const
	{
		if (m_nPKey == 0) {
			return OdString::kEmpty;
		}

		return m_sKey.mid(m_nPKey, odmin(n, m_sKey.getLength() - m_nPKey));
	}
	/*
		n [in] maximum number of characters to return.

		return the leftmost n characters of Key2.
	*/
	OdString key2(int n) const
	{
		return m_sKey2.left(odmin(n, m_sKey2.getLength()));
	}
	/*
		n [in] maximum number of characters to return.

		return the leftmost n characters of partial Key1.
	*/
	OdString pKey2(int n) const
	{
		if (m_nPKey2 == 0) {
			return OdString::kEmpty;
		}

		return m_sKey2.mid(m_nPKey2, odmin(n, m_sKey2.getLength() - m_nPKey2));
	}
	/*
		true if and only if the specified string matches the beginning of Key1 or it's part Key2 or it's part.
		Start of partial key is encoded by uppercase letter.

		str [in] string to test keyword matching.

		The comparison is case-insensitive.

		Example:
		if (Key1 == "noRth") || (Key2 == "South") {
			ASSERT(match("NOR") == true && match("SOUTHWEST") == false);
			ASSERT(match("RT") == true && match("UT") == false);
		}
	*/
	bool match(const OdString& str) const {
		int len = str.getLength();

		return key1(len).iCompare(str) == 0 || pKey1(len).iCompare(str) == 0 || key2(len).iCompare(str) == 0 || pKey2(len).iCompare(str) == 0;
	}
	/*
		the keyword for this KWIndexData object.
	*/
	OdString keyword() const
	{
		return m_sKword;
	}
};

/*
	This class implements a keyword index for ExDbCommandContext objects.
	<group ExServices_Classes>
*/
class KWIndex : public OdArray<KWIndexData>
{
public:

	/*
		keywords [in] Keyword list.
	*/
	KWIndex(const OdString& keywords);
	/*
		input [in] Input to be matched.

		the index of the keyword matched by the specified input or -1 if no match was found.
	*/
	int find(const OdString& input) const;
	/*
		Throws an exception if and only if the specified input matches a keyword.

		input [in] Input to be checked.

		Throws:
		::OdEdKeyword(keywordIndex, Keyword)
	*/
	void check(const OdString& input) const;
};

#include "TD_PackPop.h"
