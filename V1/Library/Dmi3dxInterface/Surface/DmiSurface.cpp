#include "stdafx.h"
#include "DmiSurface.h"

#include <mb_cart_point.h>

DmiSurface::DmiSurface()
{
	m_dUCoeffA = 1.0;
	m_dVCoeffA = 1.0;

	m_dUCoeffB = 0.0;
	m_dVCoeffB = 0.0;

	m_bSwapUVFlag = false;
}

void DmiSurface::SetUVParameterizationData(A3DUVParameterizationData & cParam, double dUScale, double dVScale)
{
	m_dUCoeffA = cParam.m_dUCoeffA; // *dUScale;
	m_dVCoeffA = cParam.m_dVCoeffA; // *dVScale;

	m_dUCoeffB = cParam.m_dUCoeffB;
	m_dVCoeffB = cParam.m_dVCoeffB;

	m_bSwapUVFlag = (1 == cParam.m_bSwapUV) ? true : false;
}

void DmiSurface::SetUVCoeffA(double dUCoeff, double dVCoeff)
{
	m_dUCoeffA = dUCoeff;
	m_dVCoeffA = dVCoeff;
}

void DmiSurface::SetUVCoeffB(double dUCoeff, double dVCoeff)
{
	m_dUCoeffB = dUCoeff;
	m_dVCoeffB = dVCoeff;
}

void DmiSurface::SetUCoeff(double dUCoeff)
{
	m_dUCoeffA = dUCoeff;
}

void DmiSurface::SetVCoeff(double dVCoeff)
{
	m_dVCoeffA = dVCoeff;
}

double DmiSurface::GetUCoeff() const
{
	return m_dUCoeffA;
}

double DmiSurface::GetVCoeff() const
{
	return m_dVCoeffA;
}

void DmiSurface::CheckPoint(MbCartPoint & cPoint) const
{
	cPoint.x = cPoint.x * m_dUCoeffA + m_dUCoeffB;
	cPoint.y = cPoint.y * m_dVCoeffA + m_dVCoeffB;

	if(true == m_bSwapUVFlag) {
		std::swap(cPoint.x, cPoint.y);
	}
}