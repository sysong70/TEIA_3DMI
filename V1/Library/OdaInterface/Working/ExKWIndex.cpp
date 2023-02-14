#include "stdafx.h"
#include "ExKWIndex.h"

#include <math.h>

//--------------------------------------------------------------------------------------------------

int partKey(const OdString& sKey)
{
	OdString key = sKey;
	key.makeLower();
	for (int i = 0; i < sKey.getLength(); ++i) {
		if (key[i] != sKey[i]) {
			return i;
        }
	}
	return 0;
}



KWIndex::KWIndex(const OdString& kwList)
{
	clear();
	int i = 0;
	const OdChar* szKwList = kwList;
	while (szKwList != nullptr && szKwList[i]) {
        // trim
		while (szKwList[i] == ' ') {
			++i;
        }

		if (!szKwList[i]) {
			break;
        }
		if (szKwList[i] == '_') {
			++i;
			break;
		}

		KWIndexData& data = *append();
		int j;
		for (j = i; szKwList[j] != ' ' && szKwList[j] != ',' && szKwList[j] != '\0'; ++j);
		data.m_sKword = data.m_sKey = OdString(szKwList + i, j - i);
		data.m_nPKey = partKey(data.m_sKey);

		i = j;
		if (szKwList[i] == ',') {
			++i;
            // trim
			while (szKwList[i] == ' ') {
				++i;
            }

			int k;
			for (k = i; szKwList[k] != ' ' && szKwList[k] != ',' && szKwList[k] != '\0'; ++k);
			data.m_sKey2 = OdString(szKwList + i, k - i);
			data.m_nPKey2 = partKey(data.m_sKey2);
		}
		else {
			data.m_sKey2.empty();
		}
	}

	unsigned int kwi = 0;
	while (szKwList != nullptr && szKwList[i] && kwi < size()) {
        // trim
		while (szKwList[i] == ' ') {
			++i;
        }

		if (!szKwList[i]) {
			break;
        }

		KWIndexData& data = at(kwi++);
		int j;
		for (j = i; szKwList[j] != ' ' && szKwList[j] != ',' && szKwList[j] != '\0'; ++j);
		data.m_sKword = OdString(szKwList + i, j - i);
		i = j;
		if (szKwList[i] == ',') {
            // trim
			++i;
			while (szKwList[i] == ' ') {
				++i;
            }

			int k;
			for (k = i; szKwList[k] != ' ' && szKwList[k] != ',' && szKwList[k] != '\0'; ++k);
			i = k;
		}
	}
}



int KWIndex::find(const OdString& input) const
{
	if (input.isEmpty()) {
		return -1;
    }

	for (unsigned int i = 0; i < size(); ++i) {
		const KWIndexData& data = at(i);
		if (data.match(input)) {
			return i;
        }
	}
	return -1;
}



void KWIndex::check(const OdString& input) const
{
	int n = find(input);
	if (n > -1) {
		throw ::OdEdKeyword(n, at(n).keyword());
    }
}
