#pragma once

class RtCurve
{
public:
	RtCurve();

	bool IsUsedLoopCurveFlag() { return m_bUsedLoopCurveFlag; }
	void SetUsedLoopCurveFlag(bool bFlag) { m_bUsedLoopCurveFlag = bFlag; }

protected:
	bool m_bUsedLoopCurveFlag;
};
