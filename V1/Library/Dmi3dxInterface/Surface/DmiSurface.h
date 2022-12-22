#pragma once

#ifndef INITIALIZE_A3D_API
#include <A3DSDKIncludes.h>
#endif

class MbCartPoint;

class DmiSurface
{
protected:
	DmiSurface();

public:
	void SetUVParameterizationData(A3DUVParameterizationData & cParam, double dUScale, double dVScale);

	virtual void SetUVCoeffA(double dUCoeff, double dVCoeff);
	virtual void SetUVCoeffB(double dUCoeff, double dVCoeff);
	virtual void SetUCoeff(double dUCoeff);
	virtual void SetVCoeff(double dVCoeff);

	virtual double GetUCoeff() const;
	virtual double GetVCoeff() const;

	virtual void CheckPoint(MbCartPoint & cPoint) const;

	virtual bool IsSwapUV() const { return m_bSwapUVFlag; }
	virtual void SetSwapUV(bool bFlag) { m_bSwapUVFlag = bFlag; }

protected:
	double m_dUCoeffA, m_dVCoeffA;
	double m_dUCoeffB, m_dVCoeffB;

	bool m_bSwapUVFlag;
};

