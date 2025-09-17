#include "StdAfx.h"

#include "3DF.Impl.h"

#include <atlstr.h>

using namespace H3DF;

void H3DF::Impl::Copy(const Impl * pcInThat)
{
	m_eType = pcInThat->m_eType; 
}

bool H3DF::Impl::Equal(const Impl * pcInThat) const
{ 
	if (m_eType != pcInThat->m_eType) {
		return false;
	}

	return true; 
}

std::vector<HoopsOption> H3DF::ParseHoopsOptionString(const CStringA & strInputOption)
{
	std::vector<HoopsOption> vcResult;
	std::map<CStringA, HoopsOption> maCommandMap;

	CStringA strTemp(strInputOption);
	strTemp.Replace("\r", "");
	strTemp.Replace("\n", "");

	// Tokenize respecting parentheses
	std::vector<CStringA> strTokens;
	CStringA strCurrent;
	int nParenDepth = 0; // 괄호 깊이

	for (int nIndex = 0; nIndex < strTemp.GetLength(); ++nIndex) {
		char ch = strTemp[nIndex];
		if (ch == ',' && nParenDepth == 0) {
			strCurrent.Trim();
			if (!strCurrent.IsEmpty())
				strTokens.push_back(strCurrent);
			strCurrent.Empty();
		}
		else {
			if (ch == '(') nParenDepth++;
			if (ch == ')') nParenDepth--;
			strCurrent += ch;
		}
	}

	if (!strCurrent.IsEmpty())
		strTokens.push_back(strCurrent);

	for (auto & raw : strTokens) {
		raw.Trim();

		if (raw.Left(3).CompareNoCase("no ") == 0) {
			CStringA key = raw.Mid(3).Trim();
			HoopsOption & cmd = maCommandMap[key];
			cmd.name = key;
			cmd.enabled = FALSE;
			continue;
		}

		int eq = raw.Find("=(");
		if (eq > 0 && raw.Right(1) == ")") {
			CStringA key = raw.Left(eq).Trim();
			CStringA inside = raw.Mid(eq + 2);
			inside = inside.Left(inside.GetLength() - 1);

			HoopsOption & cmd = maCommandMap[key];
			cmd.name = key;

			int pos = 0;
			CStringA token = inside.Tokenize(",", pos);
			while (!token.IsEmpty()) {
				token.Trim();
				if (token.CompareNoCase("on") == 0)
					cmd.enabled = TRUE;
				else if (token.CompareNoCase("off") == 0)
					cmd.enabled = FALSE;
				else if (token.Left(3).CompareNoCase("no ") == 0)
					cmd.options[token.Mid(3).Trim()] = "false";
				else {
					int eq2 = token.Find('=');
					if (eq2 >= 0) {
						CStringA k = token.Left(eq2).Trim();
						CStringA v = token.Mid(eq2 + 1).Trim();
						cmd.options[k] = v;
					}
					else {
						cmd.options[token] = "true";
					}
				}
				token = inside.Tokenize(",", pos);
			}
			continue;
		}

		eq = raw.Find('=');
		if (eq > 0) {
			CStringA key = raw.Left(eq).Trim();
			CStringA value = raw.Mid(eq + 1).Trim();

			HoopsOption & cmd = maCommandMap[key];
			cmd.name = key;

			if (value.CompareNoCase("on") == 0)
				cmd.enabled = TRUE;
			else if (value.CompareNoCase("off") == 0)
				cmd.enabled = FALSE;
			else
				cmd.options["value"] = value;

			continue;
		}

		HoopsOption & cmd = maCommandMap[raw];
		cmd.name = raw;
		cmd.enabled = TRUE;
	}

	for (auto & [_, cmd] : maCommandMap)
		vcResult.push_back(cmd);

	return vcResult;
}

/*

//== 객체 관리 ======================================================================================
void H3DF::Impl::SetImpl(Object * pcObject, Impl * pcImpl)
{
	if (nullptr == pcImpl) {
		DEBUG_RETURN;
	}

	if(nullptr != pcObject->m_pcImpl) {
		delete pcObject->m_pcImpl;
	}

	pcObject->m_pcImpl = pcImpl;
}*/