#pragma once

#include <templ_p_array.h>
#include <templ_s_array_rw.h>

#include <topology.h>

class DmiOrientedEdge : public MbOrientedEdge
{
public:
	DmiOrientedEdge(const MbCurveEdge & edge, bool orient, bool bUsedCurveOneFlag);

	bool IsUsedCurveOne() const { return m_bUsedCurveOneFlag; }

private:
	bool m_bUsedCurveOneFlag;
};